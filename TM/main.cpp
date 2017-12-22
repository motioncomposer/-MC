

#define NOMINMAX
//#define PYLON_CAMERA_ACCESS
//#define BUILD_WITH_GUI

// this will be part of the tracking core
#include "core.h"

#ifdef PYLON_CAMERA_ACCESS
#include "pylon.h"
#endif

#include "stereo.h"


#include "defines.h"
#include "structures.h"



#include "communication.h"
#include "tracking.h"


// this stuff will be part of the tracking core ...
#include "tracking/blob.h"
//#include "tracking/stream.h"


// this will be not used anymore at this level

#include "opencv2\core.hpp"
#include "opencv2\highgui.hpp"



// this stuff will be included in the lower layers ...
#include <iostream>
#include <string>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>
#include <algorithm>
#include <memory>




int main(int ac, char** av)
{

	static_assert(std::numeric_limits<float>::is_iec559, "IEEE 754 required");

	// this stuff is not needed ...
	std::cout << "-- MotionComposer Tracking Module" << std::endl;
	//

	mc::communication::CommunicationCoreFileAccess fileAccess;
	mc::communication::CommunicationCore communicationCore(fileAccess);


	if (!communicationCore.isReady())
	{
		// here we can write into a log file ... so we need a log file writer ...
		std::cout << "-- error while initializing communication!" << std::endl;
		std::cout << "-- error message: " << mc::communication::getCommunicationCoreErrorString(communicationCore.getStateCode()) << std::endl;
		std::cout << "-- tracking module shut down" << std::endl;
		return -1;
	}






	// would be cool if we always can use a send function ... and we simply pass

	// now we send loaded
	// send loaded is simply a message that the communication module is alive ...
	// we can say ... a test message
	communicationCore.notifyControlModule();


	// from here ... the tracking starts ...

	// ==========================================================================================================================
	//
	// tracking core ... we need to put this into a separate object
	//
	// ==========================================================================================================================


	// this comes into an extra while loop that restarts tracking core in case of error or init request


#ifdef PYLON_CAMERA_ACCESS

	// here we also need the option to read in the pylon parameter
	fs::pylon::PylonParameter pylonPara;
	pylonPara.sync = true;

	fs::pylon::PylonAccess pylonCap;
	pylonCap.openCameras(mc::defines::filenamePylonCameras/*, mc::defines::filenamePylonParameter*/, pylonPara);

	if (!pylonCap.isOpened())
	{
		std::cout << "-- could not initialize cameras" << std::endl;
		std::cout << "-- error: " << pylonCap.getErrMessage() << std::endl;
		std::cout << "-- program abort ..." << std::endl;
		return -1;
	}

#else

	cv::VideoCapture cap_left, cap_right;
	cap_left.open(mc::defines::filenameLeftVideo);
	cap_right.open(mc::defines::filenameRightVideo);

	if (!cap_left.isOpened() || !cap_right.isOpened())
	{
		std::cout << "-- could not open video files" << std::endl;
		std::cout << "-- program abort ..." << std::endl;
		return -1;
	}

#endif
	

	// here we also need the option to read in the matching parameter
	fs::stereo::MatchingParameter matchingPara;
	matchingPara.speckleWindowSize = 400;
	matchingPara.speckleRange = 120;

	fs::stereo::StereoCam stereo(mc::defines::filenameCalibrationResults, matchingPara);

	if (!stereo.isCalibrated())
	{
		std::cout << "-- loading stereo configuration failed" << std::endl;
		std::cout << "-- program abort ..." << std::endl;
		// here in this point we need to send back an error state to control module
		return -1;
	}

	fs::core::BasicStereoValues values;
	stereo.getBasicStereoValues(values);



	// this stuff we better bundle into a specific struct ...
	const cv::Size targetSize(mc::defines::operatingWidth, mc::defines::operatingHeight);


	cv::Mat frame[2];
	cv::Mat rectified_grey_left;
	cv::Mat rectified_grey_rgb_left;
	cv::Mat rectified_grey_rgb_left_overlay;
	cv::Mat rectified_grey_rgb_left_result;
	cv::Mat disparity;



	std::chrono::high_resolution_clock::time_point start, end;


	//
	mc_test::blob::BlobFinder blobFinder;
	blobFinder.getParameter().floorLvl = 370;
	blobFinder.getParameter().minDisparity = 16 * values.focalLength * values.baseLineLength / 46;
	blobFinder.getParameter().maxDisparity = 16 * values.focalLength * values.baseLineLength / 10;


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// stuff for the streaming

	// this comes to networkConfiguration or to streaming configuration ... put this to stream configuration ...
	std::string port("50000");
	std::string host("127.0.0.1");

	// we some how need to provide the stream parameter also in the constructor

	// ha :) we are not sending anything
	//mc::stream::VideoStreamServer sender(port);


	cv::RNG rng(12345);
	std::vector<cv::Scalar> colors(40);

	for (auto& it : colors)
		it = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));

	double alpha = 0.5;


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// we need also some other structures to store the timing ...

	int fps = 25;
	int sleep = 1000 / fps;
	int time_needed;

	bool loop(true);
	bool step(false);
	bool video(false);


	while (loop)
	{


#ifdef PYLON_CAMERA_ACCESS

		if (!pylonCap.readFrames(frame[0], frame[1]))
		{
			std::cout << "-- could not read frames" << std::endl;
			std::cout << "-- error: " << pylonCap.getErrMessage() << std::endl;
			std::cout << "-- program abort ..." << std::endl;
			return -1;
		}

#else

		if (!cap_left.read(frame[0]) || !cap_right.read(frame[1]))
		{
			cap_left.set(CV_CAP_PROP_POS_FRAMES, 0);
			cap_right.set(CV_CAP_PROP_POS_FRAMES, 0);
			continue;
		}

#endif

		start = std::chrono::high_resolution_clock::now();

		stereo.putRawViews(frame[0], frame[1], true);

		// apply the operating image size
		cv::resize(stereo.getRectifiedLeftGrey(), rectified_grey_left, targetSize);
		cv::resize(stereo.getDisparityImage(), disparity, targetSize);

		cv::cvtColor(rectified_grey_left, rectified_grey_rgb_left, CV_GRAY2BGR);
		rectified_grey_rgb_left.copyTo(rectified_grey_rgb_left_overlay);


		// find the blobs
		blobFinder.apply(disparity, values);


		// here we have to get the players

		// position estimator ...
		

		blobFinder.printTrackers(rectified_grey_rgb_left_overlay, colors);

		cv::addWeighted(rectified_grey_rgb_left, alpha, rectified_grey_rgb_left_overlay, 1.0 - alpha, 0., rectified_grey_rgb_left_result);

		//sender.storeMat(ground);

		end = std::chrono::high_resolution_clock::now();
		time_needed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cout << "-- time needed: " << time_needed << " ms" << std::endl;

		cv::imshow("Left", rectified_grey_rgb_left_result);

		switch (static_cast<char>(cv::waitKey(step ? 0 : (time_needed < sleep ? (sleep - time_needed) : 5))))
		{
		case 's':
		case 'S':
			step = !step;
			break;
		case 'q':
		case 'Q':
			loop = false;
			break;
		default:
			loop = true;
		}
	}




	/*
	bool retry(true);
	while (retry)
	{
		mc::tracking::TrackingCore trackingCore(mc::tracking::TrackingCoreFileAccess());


	}
	*/



	// here we also need to specify 


	// ok
	//mc::result::ResultBundle result;



	//while (true)
	//{
	//	start = std::chrono::high_resolution_clock::now();

		// send the tracking results (osc messages) to ME
	//	senderToME.send(meCommandBundle, result);

		// our ME is the MaxMSP

	//	end = std::chrono::high_resolution_clock::now();

	//	std::this_thread::sleep_for(period - (end - start));
	//}

	return 0;
}