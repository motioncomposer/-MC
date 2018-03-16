
#include "communication.h"
#include "tracking.h"


#include "defines.h"
#include "structures.h"


#include <iostream>
#include <string>
#include <chrono>


int main(int ac, char** av)
{
	static_assert(std::numeric_limits<float>::is_iec559, "IEEE 754 required");

	const mc::communication::CommunicationCoreFileAccess communicationCoreFileAccess(mc::defines::filenameNetworkConfiguration,
		mc::defines::keyNetworkConfiguration,
		mc::defines::filenameCM2TM,
		mc::defines::keyCM2TM,
		mc::defines::filenameTM2CM,
		mc::defines::keyTM2CM,
		mc::defines::filenameME2TM,
		mc::defines::keyME2TM,
		mc::defines::filenameTM2ME,
		mc::defines::keyTM2ME);

	const mc::tracking::TrackingCoreFileAccess trackingCoreFileAccess(mc::defines::filenamePylonList,
		mc::defines::keyPylonList,
		mc::defines::filenamePylonParameter,
		mc::defines::keyPylonParameter,
		mc::defines::filenameCalibrationResult,
		mc::defines::keyCalibrationResult,
		mc::defines::filenameMatchingParameter,
		mc::defines::keyMatchingParameter,
		mc::defines::filenameBlobFinderParameter,
		mc::defines::keyBlobFinderParameter,
		mc::defines::filenameActivityEstimatorParameter,
		mc::defines::keyActivityEstimatorParameter,
		mc::defines::filenameLocationEstimatorParameter,
		mc::defines::keyLocationEstimatorParameter,
		mc::defines::filenamePositionEstimatorParameter,
		mc::defines::keyPositionEstimatorParameter,
		mc::defines::filenameGestureDetectorParameter,
		mc::defines::keyGestureDetectorParameter,
		mc::defines::filenameStreamConfiguration,
		mc::defines::keyStreamConfiguration);	



	mc::communication::CommunicationCore communicationCore(communicationCoreFileAccess);

	if (!communicationCore.isReady())
	{
		// write to log file ...
		std::cout << "-- error while initializing communication" << std::endl;
		std::cout << "-- error message: " << mc::communication::getCommunicationCoreErrorString(communicationCore.getStateCode()) << std::endl;
		std::cout << "-- tracking module shut down ..." << std::endl;
		return -1;
	}


	std::cout << "-- communication successfully initialized" << std::endl;
	communicationCore.notifyLoaded();


	while (true)
	{
		mc::tracking::TrackingCore trackingCore(trackingCoreFileAccess);

		if (trackingCore.isReady())
		{
			std::cout << "-- tracking successfully initialized" << std::endl;

			communicationCore.notifyReady();

			// we should also have somthing to retreive the environment from control module so the user can adjust the depth range and the floor level
			// there for we also need a apply environment function within the mc::blob::BlobFinder

			mc::structures::Activation activation;
			mc::structures::Selection selection;
			mc::structures::Result result;

			// frames per second is hardcoded
			const std::chrono::milliseconds time_sleep(mc::defines::second / mc::defines::fps);

			std::chrono::high_resolution_clock::time_point start, end;
			std::chrono::microseconds time_needed;
			std::chrono::microseconds time_rest;


			while (true)
			{

				if (!communicationCore.keepRunning())
					break;

				communicationCore.updateActivation(activation);
				communicationCore.updateSelection(selection);

				// here we may be can make the selection a constant ...
				start = std::chrono::high_resolution_clock::now();
				trackingCore.apply(activation, selection, result);
				end = std::chrono::high_resolution_clock::now();


				if (!trackingCore.isReady())
					break;
				

				// here we need a separate thread under the hood? --> sending queue
				communicationCore.sendResult(selection, result);

				time_needed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
				time_rest = time_sleep - time_needed;

				std::this_thread::sleep_for(time_rest);
			}

		}


		if (!trackingCore.isReady() && communicationCore.keepRunning())
		{
			std::cout << "-- error within tracking core" << std::endl;
			std::cout << "-- error message: " << mc::tracking::getTrackingCoreErrorString(trackingCore.getStateCode()) << std::endl;
			std::cout << "-- error code: " << mc::tracking::getTrackingCoreErrorCode(trackingCore.getStateCode()) << std::endl;
			std::cout << "-- wait for commands ..." << std::endl;

			communicationCore.sendError(mc::tracking::getTrackingCoreErrorCode(trackingCore.getStateCode()));
			communicationCore.waitForStateUpdate();
		}


		if (communicationCore.stopRequested())
			break;

		communicationCore.resetState();
	}


	// we may need a special sendAllZero;
	//communicationCore.sendResult(mc::structures::Result());
	communicationCore.notifyStopped();

	return 0;
}