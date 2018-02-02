#pragma once

#include <string>
#include <memory>


// motioncomposer
#include "tracking/blob.h"
#include "tracking/location.h"
#include "tracking/position.h"
#include "tracking/activity.h"
#include "tracking/gesture.h"
//#inlcude "tracking/zone.h"
#include "tracking/stream.h"

// motioncomposer
#include "defines.h"
#include "structures.h"

// fusion
#include "core.h"

#ifdef PYLON_CAMERA_ACCESS
#include "pylon.h"
#endif

#include "stereo.h"


#include "opencv2\core.hpp"

namespace mc
{
	namespace tracking
	{



		// ==========================================================================================================================


		enum class TrackingCoreError
		{
			TC_NO_ERROR,
			TC_READING_PYLON_PARAMETER_FAIL,
			TC_READING_PYLON_LIST_FAIL,
			TC_ACCESSING_PYLON_CAMERA_FAIL,
			TC_GRABBING_PYLON_CAMERA_FAIL,
			TC_READING_CALIBRATION_RESULT_FAIL,
			TC_READING_MATCHING_PARAMETER_FAIL,
			TC_READING_BLOBFINDER_PARAMETER_FAIL,
			TC_READING_ACTIVITY_PARAMTER_FAIL,
			TC_READING_LOCATION_PARAMTER_FAIL,
			TC_READING_POSITION_PARAMETER_FAIL,
			TC_READING_GESTURE_PARAMETER_FAIL,
			TC_READING_STREAMING_PARAMETER_FAIL,
			TC_STREAM_DATA_FAIL
		};


		// ==========================================================================================================================


		std::string getTrackingCoreErrorString(const TrackingCoreError& state);

		int getTrackingCoreErrorCode(const TrackingCoreError& state);


		// ==========================================================================================================================


		struct TrackingCoreFileAccess
		{

			std::string filenamePylonList;
			std::string keyPylonList;

			std::string filenamePylonParameter;
			std::string keyPylonParameter;

			//
			// aka calibration result ...
			std::string filenameCalibrationResult;
			std::string keyCalibrationResult; // this key does not exist ... we first have to adjust the calibration stuff and the stereo camera ...

			// this is may be bundled with the previous one to fs::core::StereoCamConfiguration ...
			std::string filenameMatchingParameter;
			std::string keyMatchingParameter;
			//

			std::string filenameBlobFinderParameter;
			std::string keyBlobFinderParameter;

			std::string filenameActivityEstimatorParameter;
			std::string keyActivityEstimatorParameter;

			std::string filenameLocationEstimatorParameter;
			std::string keyLocationEstimatorParameter;

			std::string filenamePositionEstimatorParameter;
			std::string keyPositionEstimatorParameter;			

			std::string filenameGestureDetectorParameter;
			std::string keyGestureDetectorParameter;

			std::string filenameStreamConfiguration;
			std::string keyStreamConfiguration;

			TrackingCoreFileAccess(const std::string& filenamePylonList,
				const std::string& keyPylonList,
				const std::string& filenamePylonParameter,
				const std::string& keyPylonParameter,
				const std::string& filenameCalibrationResult,
				const std::string& keyCalibrationResult,
				const std::string& filenameMatchingParameter,
				const std::string& keyMatchingParameter,
				const std::string& filenameBlobFinderParameter,
				const std::string& keyBlobFinderParameter,
				const std::string& filenameActivityEstimatorParameter,
				const std::string& keyActivityEstimatorParameter,
				const std::string& filenameLocationEstimatorParameter,
				const std::string& keyLocationEstimatorParameter,
				const std::string& filenamePositionEstimatorParameter,
				const std::string& keyPositionEstimatorParameter,
				const std::string& filenameGestureDetectorParameter,
				const std::string& keyGestureDetectorParameter,
				const std::string& filenameStreamConfiguration,
				const std::string& keyStreamConfiguration) : filenamePylonList(filenamePylonList),
				keyPylonList(keyPylonList),
				filenamePylonParameter(filenamePylonParameter),
				keyPylonParameter(keyPylonParameter),
				filenameCalibrationResult(filenameCalibrationResult),
				keyCalibrationResult(keyCalibrationResult),
				filenameMatchingParameter(filenameMatchingParameter),
				keyMatchingParameter(keyMatchingParameter),
				filenameBlobFinderParameter(filenameBlobFinderParameter),
				keyBlobFinderParameter(keyBlobFinderParameter),
				filenameActivityEstimatorParameter(filenameActivityEstimatorParameter),
				keyActivityEstimatorParameter(keyActivityEstimatorParameter),
				filenameLocationEstimatorParameter(filenameLocationEstimatorParameter),
				keyLocationEstimatorParameter(keyLocationEstimatorParameter),
				filenamePositionEstimatorParameter(filenamePositionEstimatorParameter),
				keyPositionEstimatorParameter(keyPositionEstimatorParameter),
				filenameGestureDetectorParameter(filenameGestureDetectorParameter),
				keyGestureDetectorParameter(keyGestureDetectorParameter),
				filenameStreamConfiguration(filenameStreamConfiguration),
				keyStreamConfiguration(keyStreamConfiguration)
			{}
		};


		// ==========================================================================================================================


		class TrackingCore
		{
		public:

			TrackingCore(const TrackingCoreFileAccess& access);

			bool isReady() const;

			TrackingCoreError getStateCode() const;

			void apply(const mc::structures::Activation activation, mc::structures::Selection& selection, mc::structures::Result& result);


		private:

			TrackingCoreError state;

			cv::Mat rawFrameLeft, rawFrameRight;
			cv::Mat rawFrameLeftResized, rawFrameRightResized;
			cv::Mat rectifiedGreyLeft;
			cv::Mat disparityMap;

			mc::structures::SharedData shared;
			

#ifdef PYLON_CAMERA_ACCESS
			std::shared_ptr<fs::pylon::PylonAccess> pylonAccess;
#else
			std::shared_ptr<cv::VideoCapture> capLeft, capRight;
#endif

			std::shared_ptr<fs::stereo::StereoCam> stereoCam;

			
			std::shared_ptr<mc::blob::BlobFinder> blobFinder;
			std::shared_ptr<mc::location::LocationEstimator> locationEstimator;
			std::shared_ptr<mc::position::PositionEstimator> positionEstimator;
			
			/*
			std::shared_ptr<mc::activity::ActivityEstimator> activityEstimator;
			std::shared_ptr<mc::gesture::GestureDetector> getureDetector;			
			*/

			std::shared_ptr<mc::stream::ImageAndContoursStreamServer> streamServer;

		};


		// ==========================================================================================================================


	}
}
