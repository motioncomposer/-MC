#pragma once

#include <string>

#include "defines.h"
#include "structures.h" // move this to shared

namespace mc
{
	namespace tracking
	{



		// ==========================================================================================================================


		enum class TrackingCoreError
		{
			NO_ERROR,
			READING_PYLON_PARAMETER_FAIL,
			READING_PYLON_LIST_FAIL,
			ACCESSING_PYLON_CAMERA_FAIL,
			GRABBING_PYLON_CAMERA_FAIL,
			READING_STEREO_PARAMETER_FAIL,
			READING_BLOBFINDER_PARAMETER_FAIL,
			READING_ACTIVITY_PARAMTER_FAIL,
			READING_LOCATION_PARAMTER_FAIL,
			READING_POSITION_PARAMETER_FAIL,
			READING_GESTURE_PARAMETER_FAIL,
			READING_STREAMING_PARAMETER_FAIL,
			STREAM_DATA_FAIL
		};


		// ==========================================================================================================================


		struct TrackingCoreFileAccess
		{
			// here we put in all the filenames and keys we need to load the parameter for each module ...
		};


		// ==========================================================================================================================


		class TrackingCore
		{
		public:

			TrackingCore(const TrackingCoreFileAccess& access);

			bool isReady() const;

			TrackingCoreError getStateCode() const;


			// this function should return an error state
			void apply(mc::result::ResultBundle& result);


		private:

			TrackingCoreError state;

			// here all the member variables are placed



		};


		// ==========================================================================================================================


		std::string getTrackingCoreErrorString(const TrackingCoreError& state);

		int getTrackingCoreErrorCode(const TrackingCoreError& state);

	}
}
