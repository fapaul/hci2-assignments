#include "Application.h"

#include <iostream>

#include "framework/DepthCameraException.h"

int main()
{
	for (int i = 0; i < 10; i++) {
		try
		{
			Application application;
		
			while (!application.isFinished())
				application.loop();

			break;
		}
		catch (DepthCameraException dce)
		{
			std::cerr << std::endl << "[DepthCamera Error] " << dce.what() << std::endl;
			std::cout << std::endl << std::endl << "Press Enter to close the application...";
			std::cout << std::endl << "Versuch " << (i+1) << std::endl << std::endl;
		}
	}

	return EXIT_SUCCESS;
}