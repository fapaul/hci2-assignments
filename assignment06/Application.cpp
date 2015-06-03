///////////////////////////////////////////////////////////////////////////
//
// Main class for HCI2 assignments
// Authors:
//		Stefan Neubert (2015)
//		Stephan Richter (2011)
//		Patrick Lühne (2012)
//
///////////////////////////////////////////////////////////////////////////

#include "Application.h"

#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "framework/DepthCamera.h"
#include "framework/KinectMotor.h"
#include "framework/SkeletonTracker.h"

#include "Calibration.h"

#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#include <boost/thread.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include "uist-game/GameServer.h"
#include "uist-game/GameClient.h"
#include "uist-game/Game.h"
#include "uist-game/GameUnit.h"

const int Application::uist_level = 1;
const char* Application::uist_server = "127.0.0.1";

using namespace cv;
using namespace std;

Mat background;
Mat ausgabe;
int selectedPlayer = -1;

#define INVERT(x) (cv::Scalar::all(255) - (x))

void Application::warpImage()
{
	Mat homography = m_calibration->projectorToPhysical();

	flip(m_gameImage, m_gameImage, 1);

	warpPerspective(
		m_gameImage,
		m_outputImage,
		homography,
		Size(m_outputImage.cols, m_outputImage.rows),
		INTER_NEAREST);

}

Point last;

void Application::processFrame()
{	
	Mat temp;
	absdiff(background, m_depthImage, temp);

	temp *= 10;
	flip(temp, temp, 1);

	temp.convertTo(temp, CV_8UC1, 1.0 / 256);

	threshold(temp, temp, 3, 255, THRESH_TOZERO_INV);
	erode(temp, temp, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(20, 20)));
	dilate(temp, temp, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(20, 20)));

	vector<vector<Point>> conts;
	Mat temp2 = temp.clone();
	findContours(temp2, conts, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	for (int i = 0; i< conts.size(); i++)
	{
		Scalar color = Scalar(255, 255, 255);
		drawContours(temp, conts, i, color, 2, 8);
	}

	if (conts.size() > 0) {
		vector<Point> c = conts.at(0);

		RotatedRect r = fitEllipse(c);

		Matx33f m = m_calibration->physicalToCamera();
		Point3f touch = m * Point3f(r.center.x, r.center.y, 1);
		Point touch2(touch.x / touch.z, touch.y / touch.z);
		circle(m_gameImage, touch2, 10, Scalar(0, 0, 255), 3);

		GameUnitPtr player;

		int old = selectedPlayer;

		Point d = last - touch2;
		if (selectedPlayer >= 0 && sqrt(d.x*d.x + d.y*d.y) < 50) {
			// DAS MUSS SO
		}
		else {
			selectedPlayer = -1;
			for (int i = 0; i < 5; i++) {
				GameUnitPtr p = m_gameClient->game()->unitByIndex(i);

				Point pos = touch2 - p->position();
				float diff = sqrt(pos.x*pos.x + pos.y*pos.y);
				
				if (diff < 30) {
					circle(m_gameImage, p->position(), 10, Scalar(255, 0, 255), 3);
					last = p->position();
					selectedPlayer = i;
					cout << "FOUND " << i << endl;
					break;
				}
			}

			if (selectedPlayer < 0) {
				cout << "NOTHING" << endl;
			}

			if (selectedPlayer < 0 && old >= 0) {
				cout << "MOVE " << old << endl;
				Point oldPos = m_gameClient->game()->unitByIndex(old)->position();

				Point delta = touch2 - oldPos;

				float angle = atan2(-delta.y, delta.x);
				m_gameClient->game()->moveUnit(old, angle, 1);

			}
		}
	}


	ausgabe = temp.clone();

	warpImage();
}

void Application::processSkeleton(XnUserID userId)
{
	///////////////////////////////////////////////////////////////////////////
	//
	// To do:
	//
	// This method will be called every frame of the camera for each tracked user.
	// Insert code here in order to fulfill the assignment.
	//
	// Below is an example how to retrieve a skeleton joint position and
	// (if needed) how to convert its position into image space.
	//
	///////////////////////////////////////////////////////////////////////////

	m_skeletonTracker->drawSkeleton(m_bgrImage, userId);

	xn::SkeletonCapability skeletonCap = m_skeletonTracker->getSkeletonCapability();

	XnSkeletonJointPosition leftHand;
	skeletonCap.GetSkeletonJointPosition(userId, XN_SKEL_LEFT_HAND, leftHand);
	cv::Point2f imagePosition = m_skeletonTracker->getProjectedPosition(leftHand);
}

void Application::loop()
{
	int key = cv::waitKey(20);

	// If projector and camera aren't calibrated, do this and nothing else
	if (!m_calibration->hasTerminated())
	{
		if (key == 'q')
			m_isFinished = true;

		if(m_depthCamera)
		{
			m_depthCamera->getFrame(m_bgrImage, m_depthImage);
		}
		m_calibration->loop(m_bgrImage, m_depthImage);
		background = m_depthImage.clone();

		return;
	}

	switch (key)
	{
	case 'q': // quit
		m_isFinished = true;
		break;
	case 'p': // screenshot
		makeScreenshots();
		break;
	// run the loaded level
	case 'r':
		if(m_gameServer)
			m_gameServer->startGame();
		break;
	// Move fist (0) game unit with wasd
	case 'w': // north
		if (m_gameClient && m_gameClient->game() && selectedPlayer > 0)
			m_gameClient->game()->moveUnit(selectedPlayer, (float)M_PI_2, 1.f);
		break;
	case 'a': // west
		if (m_gameClient && m_gameClient->game() && selectedPlayer > 0)
			m_gameClient->game()->moveUnit(selectedPlayer, (float)M_PI, 1.f);
		break;
	case 's': // south
		if (m_gameClient && m_gameClient->game() && selectedPlayer > 0)
			m_gameClient->game()->moveUnit(selectedPlayer, 3 * (float)M_PI_2, 1.f);
		break;
	case 'd': // east
		if (m_gameClient && m_gameClient->game() && selectedPlayer > 0)
			m_gameClient->game()->moveUnit(selectedPlayer, 0.f, 1.f);
		break;
	// stop the first (0) game unit
	case ' ':
		if (m_gameClient && m_gameClient->game() && selectedPlayer > 0)
			m_gameClient->game()->moveUnit(selectedPlayer, 0.f, 0.f);
		break;
	// highlight the first (0) game unit
	case 'h':
		if (m_gameClient && m_gameClient->game() && selectedPlayer > 0)
			m_gameClient->game()->highlightUnit(selectedPlayer, true);
		break;
	case 'u':
		if (m_gameClient && m_gameClient->game() && selectedPlayer > 0)
			m_gameClient->game()->highlightUnit(selectedPlayer, false);
		break;
	}
	
	if(m_isFinished) return;

	if(m_gameClient && m_gameClient->game())
		m_gameClient->game()->render(m_gameImage);

	if(m_depthCamera)
	{
		m_depthCamera->getFrame(m_bgrImage, m_depthImage);
		processFrame();
	}

	if(m_skeletonTracker)
	{
		std::set<XnUserID>& users = m_skeletonTracker->getTrackedUsers();
		for (std::set<XnUserID>::iterator i = users.begin(); i != users.end(); ++i)
			processSkeleton(*i);
	}


	if (selectedPlayer >= 0) {
		GameUnitPtr p = m_gameClient->game()->unitByIndex(selectedPlayer);
		//circle(m_gameImage, p->position(), 10, Scalar(255, 0, 255), 3);
		for (int i = 0; i < 5; i++) {
			m_gameClient->game()->highlightUnit(i, (i == selectedPlayer));
		}

	}

	cv::imshow("bgr", m_bgrImage);
	cv::imshow("depth", m_depthImage);
	cv::imshow("output", m_outputImage);
	cv::imshow("UIST game", m_gameImage);
	cv::imshow("AUSGABE", ausgabe);
}

void Application::makeScreenshots()
{
	cv::imwrite("color.png", m_bgrImage);
	cv::imwrite("depth.png", m_depthImage);
	cv::imwrite("output.png", m_outputImage);
}

Application::Application()
	: m_isFinished(false)
	, m_depthCamera(nullptr)
	, m_kinectMotor(nullptr)
	, m_skeletonTracker(nullptr)
	, m_gameClient(nullptr)
	, m_gameServer(nullptr)
	, m_calibration(nullptr)
{
	// If you want to control the motor / LED
	// m_kinectMotor = new KinectMotor;

	// Not used for UIST game demo, uncomment for skeleton assignment
	m_depthCamera = new DepthCamera;
	//m_skeletonTracker = new SkeletonTracker(m_depthCamera);

	// open windows
	cv::namedWindow("output", CV_WINDOW_NORMAL);
	cv::namedWindow("depth", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("bgr", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("UIST game", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("AUSGABE", CV_WINDOW_AUTOSIZE);

	// create work buffer
	m_bgrImage = cv::Mat(480, 640, CV_8UC3);
	m_depthImage = cv::Mat(480, 640, CV_16UC1);
	m_outputImage = cv::Mat(480, 640, CV_8UC1);
	m_gameImage = cv::Mat(480, 480, CV_8UC3);
	ausgabe = cv::Mat(480, 480, CV_8UC3);

//	if(uist_server == "127.0.0.1") {
		m_gameServer = new GameServer;
		m_gameClient = new GameClient;
		m_gameServer->run();
		m_gameServer->loadGame(uist_level);
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
//	}
	m_gameClient->run();
	m_gameClient->connectToServer(uist_server);
	std::cout << "[Info] Connected to " << uist_server << std::endl;

	m_calibration = new Calibration();
}

Application::~Application()
{
	// Fails due to race conditions in UIST game
	/*
	if (m_gameClient)
	{
		m_gameClient->stop();
		delete m_gameClient;
	}
	if (m_gameServer)
	{
		m_gameServer->stop();
		delete m_gameServer;
	}*/

	if (m_skeletonTracker) delete m_skeletonTracker;
	if (m_depthCamera) delete m_depthCamera;
	if (m_kinectMotor) delete m_kinectMotor;
	if (m_calibration) delete m_calibration;
}

bool Application::isFinished()
{
	return m_isFinished;
}
