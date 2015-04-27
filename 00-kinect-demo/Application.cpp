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

#define INVERT(x) (cv::Scalar::all(255) - (x))

#define BRIGHTEN_FACTOR 10
bool output = false;

void Application::processFrame()
{
	// Bei erstem Bild kalibrieren
	if (!m_bBackgroundInitialized) {
		m_backgroundImage = m_depthImage.clone() * BRIGHTEN_FACTOR;
		m_bBackgroundInitialized = true;
	}

	// Bild aufhellen
	m_depthImage *= BRIGHTEN_FACTOR;

	// Hintergrund abziehen
	m_outputImage = INVERT(m_depthImage - m_backgroundImage);

	m_outputImage.convertTo(m_outputImage, CV_32FC1);

	if (output)
		cv::imwrite("screenshots/output1.png", m_outputImage);
	cv::threshold(m_outputImage, m_outputImage, 3, 255, cv::THRESH_TOZERO_INV);
	if (output)
		cv::imwrite("screenshots/output2.png", m_outputImage);
	cv::blur(m_outputImage, m_outputImage, cv::Size(12, 12));
	if (output)
		cv::imwrite("screenshots/output3.png", m_outputImage);
	cv::threshold(m_outputImage, m_outputImage, 2, 255, cv::THRESH_BINARY);
	if (output)
		cv::imwrite("screenshots/output4.png", m_outputImage);
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
	cv::imshow("depth", m_depthImage);
	cv::imshow("output", m_outputImage);
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

    // create work buffer
	m_bgrImage = cv::Mat(480, 640, CV_8UC3);
	m_depthImage = cv::Mat(480, 640, CV_16UC1);
	m_outputImage = cv::Mat(480, 640, CV_8UC1);
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
