
#include "tracking.h"

namespace mc
{
	namespace tracking
	{


		TrackingCore::TrackingCore(const TrackingCoreFileAccess& access)
		{
			bool ok(true);

			// here we need to read in all the stuff for the different modules ... we have to specify some
			// read in function for each parameter (not a problem)

		}


		bool TrackingCore::isReady() const
		{
			return state == TrackingCoreError::NO_ERROR;
		}



		TrackingCoreError TrackingCore::getStateCode() const
		{
			return state;
		}


		// this function should return an error state
		void TrackingCore::apply(mc::result::ResultBundle& result)
		{
			// here we put in the whole tracking stuff from the main loop
		}


		// ==========================================================================================================================


		std::string getTrackingCoreErrorString(const TrackingCoreError& state)
		{
			switch (state)
			{
			case TrackingCoreError::NO_ERROR:
				return{ "loading completed" };
				break;
			case TrackingCoreError::READING_PYLON_PARAMETER_FAIL:
				return{ "loading pylon parameter failed" };
				break;
			case TrackingCoreError::READING_PYLON_LIST_FAIL:
				return{ "no pylon cameras specified" };
				break;
			case TrackingCoreError::ACCESSING_PYLON_CAMERA_FAIL:
				return{ "access pylon camera failed" };
				break;
			case TrackingCoreError::GRABBING_PYLON_CAMERA_FAIL:
				return{ "grabbing frames from pylon cameras failed" };
				break;
			case TrackingCoreError::READING_STEREO_PARAMETER_FAIL:
				return{ "loading stereo parameter failed" };
				break;
			case TrackingCoreError::READING_BLOBFINDER_PARAMETER_FAIL:
				return{ "loading blobfinder parameter failed" };
				break;
			case TrackingCoreError::READING_ACTIVITY_PARAMTER_FAIL:
				return{ "loading activity estimator parameter failed" };
				break;
			case TrackingCoreError::READING_LOCATION_PARAMTER_FAIL:
				return{ "loading location estimator parameter failed" };
				break;
			case TrackingCoreError::READING_POSITION_PARAMETER_FAIL:
				return{ "loading position estimator parameter failed" };
				break;
			case TrackingCoreError::READING_GESTURE_PARAMETER_FAIL:
				return{ "loading gesture estimator parameter failed" };
				break;
			case TrackingCoreError::READING_STREAMING_PARAMETER_FAIL:
				return{ "loading streaming parameter failed" };
				break;
			case TrackingCoreError::STREAM_DATA_FAIL:
				return{ "streaming data failed" };
				break;
			default:
				return{ "undefined error code" };
			}
		}


		int getTrackingCoreErrorCode(const TrackingCoreError& state)
		{
			switch (state)
			{
			case TrackingCoreError::NO_ERROR:
				return 0;
				break;
			case TrackingCoreError::READING_PYLON_PARAMETER_FAIL:
				return 10;
				break;
			case TrackingCoreError::READING_PYLON_LIST_FAIL:
				return 11;
				break;
			case TrackingCoreError::ACCESSING_PYLON_CAMERA_FAIL:
				return 12;
				break;
			case TrackingCoreError::GRABBING_PYLON_CAMERA_FAIL:
				return 13;
				break;
			case TrackingCoreError::READING_STEREO_PARAMETER_FAIL:
				return 20;
				break;
			case TrackingCoreError::READING_BLOBFINDER_PARAMETER_FAIL:
				return 30;
				break;
			case TrackingCoreError::READING_ACTIVITY_PARAMTER_FAIL:
				return 40;
				break;
			case TrackingCoreError::READING_LOCATION_PARAMTER_FAIL:
				return 50;
				break;
			case TrackingCoreError::READING_POSITION_PARAMETER_FAIL:
				return 60;
				break;
			case TrackingCoreError::READING_GESTURE_PARAMETER_FAIL:
				return 70;
				break;
			case TrackingCoreError::READING_STREAMING_PARAMETER_FAIL:
				return 80;
				break;
			case TrackingCoreError::STREAM_DATA_FAIL:
				return 83;
				break;
			default:
				return -10;
			}
		}



	}
}
