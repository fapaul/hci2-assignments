///////////////////////////////////////////////////////////////////////////
//
// Main class for HCI2 assignments
// Authors:
//		Stefan Neubert (2015)
//		Stephan Richter (2011)
//		Patrick Lühne (2012)
//
///////////////////////////////////////////////////////////////////////////

#pragma warning( disable : 4018 )

#include "Application.h"

#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "framework/DepthCamera.h"
#include "framework/KinectMotor.h"

#define INVERT(x) (cv::Scalar::all(255) - (x))

#define BRIGHTEN_FACTOR 10
bool output = false;

void Application::processFrame()
{
	cv::Mat imageBase;
	cv::Mat imageRanged;

	// Bei erstem Bild kalibrieren
	if (!m_bBackgroundInitialized) {
		m_backgroundImage = m_depthImage.clone() * BRIGHTEN_FACTOR;
		m_bBackgroundInitialized = true;
	}

	// Bild aufhellen
	m_depthImage *= BRIGHTEN_FACTOR;

	// Hintergrund abziehen
	imageBase = INVERT(m_depthImage - m_backgroundImage);

	imageBase.convertTo(imageBase, CV_8UC1, 1.0/256);

	// MAGIC
	cv::threshold(imageBase, imageBase, 7, 255, cv::THRESH_TOZERO_INV);
	cv::threshold(imageBase, imageBase, 5, 255, cv::THRESH_BINARY);

	cv::blur(imageBase, imageBase, cv::Size(7, 7));

	cv::inRange(imageBase, 1, 7, imageRanged);

	cv::dilate(imageRanged, imageRanged, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(20, 20)));

	// Konturen finden
	cv::Mat temp = INVERT(imageRanged.clone());
	cv::vector<cv::vector<cv::Point>> contours;
	cv::findContours(temp, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	// ggf. Konturen finden
	if (output) {
		std::cout << contours.size() << std::endl;
		for (int i =0 ; i<contours.size(); i++) {
			std::cout << "size: " << contours[i].size() << std::endl;
		}
	}

	cv::Mat final = cv::Mat(480, 640, CV_8UC1);

	// Konturen prüfen
	if (contours.size() > 1 || (contours.size() == 1 && contours[0].size() != 2228)) {
		// größte Kontur finden
		cv::vector<cv::Point> contour = contours[0];
		int maxLength = contours[0].size();

		for (int c = 1; c < contours.size(); c++) {
			if (contours[c].size() > maxLength && contours[c].size() != 2228) {
				contour = contours[c];
				maxLength = contours[c].size();
			}
		}

		// Konturen zeichnen
		for (int i = 0; i < contours.size(); i++) {
			cv::Scalar color = cv::Scalar(255, 255, 255);
			cv::drawContours(final, contours, i, color, 2, 8);
		}

		// Konturmittelpunkt finden
		cv::Point2f middle;
		float radius;
		cv::minEnclosingCircle(contour, middle, radius);

		// Kreis zeichnen
		cv::circle(m_resultImage, middle, 20, cv::Scalar(255, 255, 255), -1);
	}

	// Bild ausgeben
	m_outputImage = final.clone();
}

void Application::loop()
{
	int key = cv::waitKey(20);
	switch (key)
	{
	case 'q': // quit
		m_isFinished = true;
		break;
	case 's': // screenshot
		makeScreenshots();
		break;
	}
	output = (key == 's');

	m_depthCamera->getFrame(m_bgrImage, m_depthImage);
	processFrame();

	cv::imshow("bgr", m_bgrImage);
	//cv::imshow("depth", m_depthImage);
	//cv::imshow("output", m_outputImage);
	cv::imshow("result", m_resultImage);
}

void Application::makeScreenshots()
{
	cv::imwrite("screenshots/color.png", m_bgrImage);
	cv::imwrite("screenshots/depth.png", m_depthImage);
	cv::imwrite("screenshots/output.png", m_outputImage);
}

Application::Application()
	: m_isFinished(false)
	, m_depthCamera(nullptr)
	, m_kinectMotor(nullptr)
{
	// If you want to control the motor / LED
	// m_kinectMotor = new KinectMotor;

	m_depthCamera = new DepthCamera;

	// open windows
	cv::namedWindow("output", 1);
	cv::namedWindow("depth", 1);
	cv::namedWindow("bgr", 1);
	cv::namedWindow("result", 1);

	// create work buffer
	m_bgrImage = cv::Mat(480, 640, CV_8UC3);
	m_depthImage = cv::Mat(480, 640, CV_16UC1);
	m_outputImage = cv::Mat(480, 640, CV_8UC1);
	m_resultImage = cv::Mat(480, 640, CV_8UC1);
	m_bBackgroundInitialized = false;
}

Application::~Application()
{
	if (m_depthCamera) delete m_depthCamera;
	if (m_kinectMotor) delete m_kinectMotor;
}

bool Application::isFinished()
{
	return m_isFinished;
}
