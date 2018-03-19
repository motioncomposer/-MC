
#include "tracking.h"

namespace mc
{
	namespace tracking
	{



		// ==========================================================================================================================


		std::string getTrackingCoreErrorString(const TrackingCoreError& state)
		{
			switch (state)
			{
			case TrackingCoreError::TC_NO_ERROR:
				return{ "loading completed" };
				break;
			case TrackingCoreError::TC_READING_PYLON_PARAMETER_FAIL:
				return{ "loading pylon parameter failed" };
				break;
			case TrackingCoreError::TC_READING_PYLON_LIST_FAIL:
				return{ "no pylon cameras specified" };
				break;
			case TrackingCoreError::TC_ACCESSING_PYLON_CAMERA_FAIL:
				return{ "access pylon camera failed" };
				break;
			case TrackingCoreError::TC_GRABBING_PYLON_CAMERA_FAIL:
				return{ "grabbing frames from pylon cameras failed" };
				break;
			case TrackingCoreError::TC_READING_CALIBRATION_RESULT_FAIL:
				return{ "loading calibration result failed" };
				break;
			case TrackingCoreError::TC_READING_MATCHING_PARAMETER_FAIL:
				return{ "loading matching parameter failed" };
				break;
			case TrackingCoreError::TC_READING_BLOBFINDER_PARAMETER_FAIL:
				return{ "loading blobfinder parameter failed" };
				break;
			case TrackingCoreError::TC_READING_ACTIVITY_PARAMTER_FAIL:
				return{ "loading activity estimator parameter failed" };
				break;
			case TrackingCoreError::TC_READING_LOCATION_PARAMTER_FAIL:
				return{ "loading location estimator parameter failed" };
				break;
			case TrackingCoreError::TC_READING_POSITION_PARAMETER_FAIL:
				return{ "loading position estimator parameter failed" };
				break;
			case TrackingCoreError::TC_READING_GESTURE_PARAMETER_FAIL:
				return{ "loading gesture estimator parameter failed" };
				break;
			case TrackingCoreError::TC_READING_STREAMING_PARAMETER_FAIL:
				return{ "loading streaming parameter failed" };
				break;
			case TrackingCoreError::TC_STREAM_DATA_FAIL:
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
			case TrackingCoreError::TC_NO_ERROR:
				return 0;
				break;
			case TrackingCoreError::TC_READING_PYLON_PARAMETER_FAIL:
				return 10;
				break;
			case TrackingCoreError::TC_READING_PYLON_LIST_FAIL:
				return 11;
				break;
			case TrackingCoreError::TC_ACCESSING_PYLON_CAMERA_FAIL:
				return 12;
				break;
			case TrackingCoreError::TC_GRABBING_PYLON_CAMERA_FAIL:
				return 13;
				break;
			case TrackingCoreError::TC_READING_CALIBRATION_RESULT_FAIL:
				return 20;
				break;
			case TrackingCoreError::TC_READING_MATCHING_PARAMETER_FAIL:
				return 30;
				break;
			case TrackingCoreError::TC_READING_BLOBFINDER_PARAMETER_FAIL:
				return 40;
				break;
			case TrackingCoreError::TC_READING_ACTIVITY_PARAMTER_FAIL:
				return 50;
				break;
			case TrackingCoreError::TC_READING_LOCATION_PARAMTER_FAIL:
				return 60;
				break;
			case TrackingCoreError::TC_READING_POSITION_PARAMETER_FAIL:
				return 70;
				break;
			case TrackingCoreError::TC_READING_GESTURE_PARAMETER_FAIL:
				return 80;
				break;
			case TrackingCoreError::TC_READING_STREAMING_PARAMETER_FAIL:
				return 90;
				break;
			case TrackingCoreError::TC_STREAM_DATA_FAIL:
				return 93;
				break;
			default:
				return -10;
			}
		}


		// ==========================================================================================================================


		TrackingCore::TrackingCore(const TrackingCoreFileAccess& access)
		{
			bool ok(true);			


#ifdef PYLON_CAMERA_ACCESS

			//
			//

			// this read serials from file is actually a stupid function ... cause it is designed as a counter part for the function readSerialsFromString(...)
			std::vector<std::string> serials; // this below looks weird ... may be we need to change this ...
			if (!fs::pylon::readSerialsFromFile(access.filenamePylonList, serials, access.keyPylonList))
			{
				state = TrackingCoreError::TC_READING_PYLON_LIST_FAIL;
				ok = false;
			}


			fs::pylon::PylonParameter pylonPara;
			if (!ok || !fs::pylon::readPylonParameter(pylonPara, access.filenamePylonParameter, access.keyPylonParameter))
			{
				state = ok ? TrackingCoreError::TC_READING_PYLON_PARAMETER_FAIL : state;
				ok = false;
			}
			
			if(ok)
				pylonAccess.reset(new fs::pylon::PylonAccess());


			if (!ok || !pylonAccess->openCameras(serials, pylonPara))
			{
				state = ok ? TrackingCoreError::TC_ACCESSING_PYLON_CAMERA_FAIL : state;
				ok = false;
			}

#else

			capLeft.reset(new cv::VideoCapture());
			capRight.reset(new cv::VideoCapture());

			if (!ok || !capLeft->open(mc::defines::filenameLeftVideo) || !capRight->open(mc::defines::filenameRightVideo))
			{
				// here we simply say that the cv::VideoCapture is replacing the use of real pylon cameras
				state = ok ? TrackingCoreError::TC_ACCESSING_PYLON_CAMERA_FAIL : state;
				ok = false;
			}

#endif

			fs::core::CamParameter camParaLeft, camParaRight;
			fs::core::StereoConfig stereoConfig;

			// at the moment we don't hand in a key for reading the xml data as we have hard coded this within the read function ...
			// we need to make a structure CalibrationParameter and StereoCalibrationResult (also CalibrationResult, MultiStereoCalibrationResult)
			if (!ok || !fs::core::stereoReadCalibration(camParaLeft, camParaRight, stereoConfig, access.filenameCalibrationResult))
			{
				state = ok ? TrackingCoreError::TC_READING_CALIBRATION_RESULT_FAIL : state;
				ok = false;
			}


			fs::stereo::MatchingParameter matchingPara;
			if (!ok || !fs::stereo::readMatchingParameter(matchingPara, access.filenameMatchingParameter, access.keyMatchingParameter))
			{
				state = ok ? TrackingCoreError::TC_READING_MATCHING_PARAMETER_FAIL : state;
				ok = false;
			}

			fs::core::BasicStereoValues values;

			if (ok)
			{
				fs::core::scaleCamParameter(camParaLeft, { mc::defines::operatingWidth, mc::defines::operatingHeight });
				fs::core::scaleCamParameter(camParaRight, { mc::defines::operatingWidth, mc::defines::operatingHeight });
				stereoCam.reset(new fs::stereo::StereoCam(camParaLeft, camParaRight, stereoConfig, matchingPara));
				stereoCam->getBasicStereoValues(values);
			}


			mc::blob::BlobFinderParameter blobPara;
			if (!ok || !mc::blob::readBlobFinderParameter(blobPara, access.filenameBlobFinderParameter, access.keyBlobFinderParameter))
			{
				state = ok ? TrackingCoreError::TC_READING_BLOBFINDER_PARAMETER_FAIL : state;
				ok = false;
			}
			else
				blobFinder.reset(new mc::blob::BlobFinder(blobPara, values));


			mc::location::LocationEstimatorParameter locationPara;
			if (!ok || !mc::location::readLocationEstimatorParameter(locationPara, mc::defines::filenameLocationEstimatorParameter, mc::defines::keyLocationEstimatorParameter))
			{
				state = ok ? TrackingCoreError::TC_READING_LOCATION_PARAMTER_FAIL : state;
				ok = false;
			}
			else
				locationEstimator.reset(new mc::location::LocationEstimator(locationPara));


			mc::position::PositionEstimatorParameter positionPara;
			if (!ok || !mc::position::readPositionEstimatorParameter(positionPara, mc::defines::filenamePositionEstimatorParameter, mc::defines::keyPositionEstimatorParameter))
			{
				state = ok ? TrackingCoreError::TC_READING_POSITION_PARAMETER_FAIL : state;
				ok = false;
			}
			else
				positionEstimator.reset(new mc::position::PositionEstimator(positionPara, values));


			mc::activity::ActivityEstimatorParameter activityPara;
			if (!ok || !mc::activity::readActivityEstimatorParameter(activityPara, mc::defines::filenameActivityEstimatorParameter, mc::defines::keyActivityEstimatorParameter))
			{
				state = ok ? TrackingCoreError::TC_READING_ACTIVITY_PARAMTER_FAIL : state;
				ok = false;
			}
			else
				activityEstimator.reset(new mc::activity::ActivityEstimator(activityPara));



			/*
			* so at this position we have to read in the parameter for all the other modules:
			*
			* - GestureDetector
			* - ZoneEstimator
			*
			*/


			/*
			mc::stream::ImageAndContoursStreamParameter streamPara;
			if (!ok || !mc::stream::readImageAndContoursStreamParameter(streamPara, access.filenameStreamConfiguration, access.keyStreamConfiguration))
			{
				state = ok ? TrackingCoreError::TC_READING_STREAMING_PARAMETER_FAIL : state;
				ok = false;
			}
			else
				streamServer.reset(new mc::stream::ImageAndContoursStreamServer(streamPara));
				*/


			mc::stream::VideoStreamParameter streamPara;
			if (!ok || !mc::stream::readVideoStreamParameter(streamPara, access.filenameStreamConfiguration, access.keyStreamConfiguration))
			{
				state = ok ? TrackingCoreError::TC_READING_STREAMING_PARAMETER_FAIL : state;
				ok = false;
			}
			else
				streamServer.reset(new mc::stream::VideoStreamServer(streamPara));



			//
			// if no error, set state to loaded
			if (ok)
				state = TrackingCoreError::TC_NO_ERROR;

		}


		bool TrackingCore::isReady() const
		{
			return state == TrackingCoreError::TC_NO_ERROR;
		}


		TrackingCoreError TrackingCore::getStateCode() const
		{
			return state;
		}


		void TrackingCore::apply(mc::structures::Activation activation, mc::structures::Selection& selection, mc::structures::Result& result)
		{

#ifdef PYLON_CAMERA_ACCESS

			if (!pylonAccess->readFrames(rawFrameLeft, rawFrameRight))
			{
				// this error messages needs to be removed ...
				std::cout << "-- could not read frames" << std::endl;
				std::cout << "-- error: " << pylonAccess->getErrMessage() << std::endl;
				std::cout << "-- program abort ..." << std::endl;

				// would be cool if we some how can pass the fs::pylon::PylonAccess error message to front
				state = TrackingCoreError::TC_GRABBING_PYLON_CAMERA_FAIL;
				return;
			}

#else

			if (!capLeft->read(rawFrameLeft) || !capRight->read(rawFrameRight))
			{
				capLeft->set(CV_CAP_PROP_POS_FRAMES, 0);
				capRight->set(CV_CAP_PROP_POS_FRAMES, 0);
				return;
			}
#endif

			// target size is hardcoded
			cv::resize(rawFrameLeft, rawFrameLeftResized, { mc::defines::operatingWidth, mc::defines::operatingHeight });
			cv::resize(rawFrameRight, rawFrameRightResized, { mc::defines::operatingWidth, mc::defines::operatingHeight });

			stereoCam->putRawViews(rawFrameLeftResized, rawFrameRightResized, true);

			// find the blobs
			blobFinder->apply(stereoCam->getDisparityImage());

			///////

			// auto selection ... can may be build this into the mc::blob::BlobFinder
			if (selection.player[0] == -1)
			{
				auto biggest = blobFinder->getOrdering()[0];

				for (auto& it : blobFinder->getTrackers())
				{
					for (auto& it2 : it.getAssociatedBlobs())
					{
						if (it2 == biggest)
						{
							selection.player[0] = it.getBlobTrackerID();
							activation.player[0] = true;
						}
					}
				}
			}

			////////


			blobFinder->updateSharedData(selection.player, shared);
			blobFinder->updateStreamData(selection.player, stream);

			locationEstimator->apply({mc::defines::operatingWidth, mc::defines::operatingHeight}, activation.player, selection.player, shared, result);
			positionEstimator->apply(stereoCam->getDisparityImage(), activation.player, selection.player, blobFinder->getContours(), shared, result);
			activityEstimator->apply(stereoCam->getRectifiedLeftGrey(), blobFinder->getMask(), activation.player, selection.player, shared, result);

			// gesture estimation

			// and of course the zones ... we can put this to activity ...


#ifdef SHOW_FRAMES

			cv::imshow("Frame", stereoCam->getRectifiedLeftGrey());
			cv::waitKey(1);
#endif


			// actually we have here a special error state that we can set ...
			streamServer->storeMat(stereoCam->getRectifiedLeftGrey());
			//streamServer->storeData(stereoCam->getRectifiedLeftGrey(), stream, blobFinder->getContours());

		}


		// ==========================================================================================================================


	}
}