#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#define PYLON_CAMERA_ACCESS

#include <string>
#include <vector>


namespace mc
{
	namespace defines
	{

		// ==========================================================================================================================
		//
		// general specification
		//
		// ==========================================================================================================================
		

		//const size_t maxPlayer(2);
		//const size_t maxZones(6);


		// specifies the image size the tracking module will work on
		const int operatingWidth(640);
		const int operatingHeight(480);

		//const int operatingWidth(480);
		//const int operatingHeight(360);

		const uint32_t second = 1000;
		const uint32_t fps = 25;


		// ==========================================================================================================================
		//
		// default filenames and key for parameter access
		//
		// ==========================================================================================================================


		// problem is ... how to detect if the reading is not working? (file opens but we cannot find the entry) in this case the default value is assigned to the
		// read result ... may be we need some "wrong" default values ... or we create for each object a special function that returns an error object ...
		// (like opencv's noArray())

		// we have basically 2 different errors: wrong filename and wrong key

		// we use the stuff with the xml because we later can merge every think into on file ...

		// we can make the filenames with capital letters at the beginning ...



		// test videos
		const std::string filenameLeftVideo("ressources//left.avi");
		const std::string filenameRightVideo("ressources//right.avi");


		//
		// mc::tracking::TrackingCore
		//


		// fs::pylon::PylonAccess
		const std::string filenamePylonList("configs//pylonList.xml");
		const std::string keyPylonList("pylonCamera"); // we need to change this to pylonList ...

		const std::string filenamePylonParameter("configs//pylonParameter.xml");
		const std::string keyPylonParameter("pylonParameter");


		// fs::stereo::StereoCam
		// this can be combined to fs::stereo::StereoCamParameter
		// this is not used yet as it is diffucult for the storing and reading
		const std::string filenameCalibrationResult("configs//calibrationResult.xml");
		const std::string keyCalibrationResult("calibrationResult");
		// it looks like that we don't have any keys for calibration stuff
		// the problem is here that we don't have a class StereoCalibrationResult that contains the 2 camera parameters and the stereo configuration
		// these 3 elements are plain written in there ...
	

		const std::string filenameMatchingParameter("configs//matchingParameter.xml");
		const std::string keyMatchingParameter("matchingParameter");


		// mc::blob::BlobFinder
		const std::string filenameBlobFinderParameter("configs//blobFinderParameter.xml");
		const std::string keyBlobFinderParameter("blobFinderParameter");


		// mc::activity::ActivityEstimator (does not exist yet)
		const std::string filenameActivityEstimatorParameter("configs//activityEstimatorParameter.xml");
		const std::string keyActivityEstimatorParameter("activityEstimatorParameter");


		// mc::location::LocationEstimator (does not exist yet)
		const std::string filenameLocationEstimatorParameter("configs//locationEstimatorParameter.xml");
		const std::string keyLocationEstimatorParameter("locationEstimatorParameter");


		// mc::position::PositionEstimator (does not exist yet)
		const std::string filenamePositionEstimatorParameter("configs//positionEstimatorParameter.xml");
		const std::string keyPositionEstimatorParameter("positionEstimatorParameter");


		// mc::gesture::GestureDetector (exists as a depreacated version)
		const std::string filenameGestureDetectorParameter("configs//gestureDetectorParameter.xml");
		const std::string keyGestureDetectorParameter("gestureDetectorParameter");


		// mc::stream::ImageAndContoursStreamServer
		const std::string filenameStreamConfiguration("configs//streamConfiguration.xml");
		const std::string keyStreamConfiguration("streamConfiguration");

		
		//
		// mc::communication::CommunicationCore
		//

		
		// mc::utils::NetworkConfiguration
		const std::string filenameNetworkConfiguration("configs//networkConfiguration.xml");
		const std::string keyNetworkConfiguration("networkConfig");

		
		// mc::receiver::ReceiverFromCM
		const std::string filenameCM2TM("configs//cm2TMConfiguration.xml");
		const std::string keyCM2TM("CM2TM");
		
		// mc::receiver::ReceiverFromME
		const std::string filenameME2TM("configs//me2TMConfiguration.xml");
		const std::string keyME2TM("ME2TM");
		
		
		// mc::sender::SenderToCM
		const std::string filenameTM2CM("configs//tm2CMConfiguration.xml");
		const std::string keyTM2CM("TM2CM");


		// mc::sender::SenderToME
		const std::string filenameTM2ME("configs//tm2MEConfiguration.xml");
		const std::string keyTM2ME("TM2ME");


		// ==========================================================================================================================
		//
		// default network configuration
		//
		// ==========================================================================================================================


		const std::string ip_localhost("127.0.0.1");

		const std::string port_ID_CM("65");
		const std::string port_ID_TM("61");
		const std::string port_ID_ME("60");

		const std::string port_TM_to_CM = port_ID_TM + port_ID_CM;
		const std::string port_TM_to_ME = port_ID_TM + port_ID_ME;
		const std::string port_TM_from_CM = port_ID_CM + port_ID_TM;
		const std::string port_TM_from_ME = port_ID_ME + port_ID_TM;


		// ==========================================================================================================================
		//
		// osc specs
		//
		// ==========================================================================================================================


		const char delimiter('/');
		const char placeholder('#');


		// ==========================================================================================================================
		//
		// message pattern
		//
		// ==========================================================================================================================


		const size_t enumeration(3);


		// pattern received from CM
		const std::string preludeCM2TM("/set");
		const std::vector<std::string> listCM2TM{ "/initialize",
			"/stop",
			"/player/#/blob",
			"/player/#/tracking",
			"/player/#/reset",
			"/zone/#/blob",
			"/zone/#/tracking",
			"/zone/#/reset"
		};


		// pattern received from ME
		const std::string preludeME2TM("/set/alphabet");
		const std::vector<std::string> listME2TM{ "/player/#/activity/discrete/hand/left",
			"/player/#/activity/discrete/hand/right",
			"/player/#/activity/discrete/head",
			"/player/#/activity/discrete/leg/left",
			"/player/#/activity/discrete/leg/right",
			"/player/#/activity/discrete/body/upper",
			"/player/#/activity/discrete/body/lower",
			"/player/#/activity/discrete/body/left",
			"/player/#/activity/discrete/body/right",
			"/player/#/activity/normal/hand/left",
			"/player/#/activity/normal/hand/right",
			"/player/#/activity/normal/head",
			"/player/#/activity/normal/leg/left",
			"/player/#/activity/normal/leg/right",
			"/player/#/activity/normal/body/upper",
			"/player/#/activity/normal/body/lower",
			"/player/#/activity/normal/body/left",
			"/player/#/activity/normal/body/right",
			"/player/#/activity/peak",
			"/player/#/activity/flow/leftwards/left",
			"/player/#/activity/flow/leftwards/right",
			"/player/#/activity/flow/rightwards/left",
			"/player/#/activity/flow/rightwards/right",
			"/player/#/activity/flow/upwards/left",
			"/player/#/activity/flow/upwards/right",
			"/player/#/activity/flow/downwards/left",
			"/player/#/activity/flow/downwards/right",
			"/player/#/location/ready",
			"/player/#/location/present",
			"/player/#/location/centerX",
			"/player/#/location/centerZ",
			"/player/#/location/outOfRange",
			"/player/#/position/height",
			"/player/#/position/heightLevel",
			"/player/#/position/vertical/hand/left",
			"/player/#/position/vertical/hand/right",
			"/player/#/position/side/hand/left",
			"/player/#/position/side/hand/right",
			"/player/#/position/side/foot/left",
			"/player/#/position/side/foot/right",
			"/player/#/position/front/hand/left",
			"/player/#/position/front/hand/right",
			"/player/#/position/front/foot/left",
			"/player/#/position/front/foot/right",
			"/player/#/position/width",
			"/player/#/gesture/hit/overhead",
			"/player/#/gesture/hit/side/left",
			"/player/#/gesture/hit/side/right",
			"/player/#/gesture/hit/down/left",
			"/player/#/gesture/hit/down/right",
			"/player/#/gesture/hit/forward/left",
			"/player/#/gesture/hit/forward/right",
			"/player/#/gesture/kick/side/left",
			"/player/#/gesture/kick/side/right",
			"/player/#/gesture/kick/forward/left",
			"/player/#/gesture/kick/forward/right",
			"/player/#/gesture/doubleArmSide",
			"/player/#/gesture/doubleArmSideClose",
			"/player/#/gesture/jump",
			"/player/#/gesture/clap",
			"/zones/#/activity/discrete",
			"/zones/#/activity/normal",
			"/zones/#/activity/flow/leftwards",
			"/zones/#/activity/flow/rightwards",
			"/zones/#/activity/flow/upwards",
			"/zones/#/activity/flow/downwards"
		};


		// pattern sended to CM
		const std::string preludeTM2CM("/set/TM");
		const std::vector<std::string> listTM2CM{ "/loaded",
			"/ready",
			"/error",
			"/stopped"
		};


		// pattern sended to ME
		const std::string preludeTM2ME("");
		const std::vector<std::string> listTM2ME{ "/player/#/activity/discrete/hand/left",
			"/player/#/activity/discrete/hand/right",
			"/player/#/activity/discrete/head",
			"/player/#/activity/discrete/leg/left",
			"/player/#/activity/discrete/leg/right",
			"/player/#/activity/discrete/body/upper",
			"/player/#/activity/discrete/body/lower",
			"/player/#/activity/discrete/body/left",
			"/player/#/activity/discrete/body/right",
			"/player/#/activity/normal/hand/left",
			"/player/#/activity/normal/hand/right",
			"/player/#/activity/normal/head",
			"/player/#/activity/normal/leg/left",
			"/player/#/activity/normal/leg/right",
			"/player/#/activity/normal/body/upper",
			"/player/#/activity/normal/body/lower",
			"/player/#/activity/normal/body/left",
			"/player/#/activity/normal/body/right",
			"/player/#/activity/peak",
			"/player/#/activity/flow/leftwards/left",
			"/player/#/activity/flow/leftwards/right",
			"/player/#/activity/flow/rightwards/left",
			"/player/#/activity/flow/rightwards/right",
			"/player/#/activity/flow/upwards/left",
			"/player/#/activity/flow/upwards/right",
			"/player/#/activity/flow/downwards/left",
			"/player/#/activity/flow/downwards/right",
			"/player/#/location/ready",
			"/player/#/location/present",
			"/player/#/location/centerX",
			"/player/#/location/centerZ",
			"/player/#/location/outOfRange",
			"/player/#/position/height",
			"/player/#/position/heightLevel",
			"/player/#/position/vertical/hand/left",
			"/player/#/position/vertical/hand/right",
			"/player/#/position/side/hand/left",
			"/player/#/position/side/hand/right",
			"/player/#/position/side/foot/left",
			"/player/#/position/side/foot/right",
			"/player/#/position/front/hand/left",
			"/player/#/position/front/hand/right",
			"/player/#/position/front/foot/left",
			"/player/#/position/front/foot/right",
			"/player/#/position/width",
			"/player/#/gesture/hit/overhead",
			"/player/#/gesture/hit/side/left",
			"/player/#/gesture/hit/side/right",
			"/player/#/gesture/hit/down/left",
			"/player/#/gesture/hit/down/right",
			"/player/#/gesture/hit/forward/left",
			"/player/#/gesture/hit/forward/right",
			"/player/#/gesture/kick/side/left",
			"/player/#/gesture/kick/side/right",
			"/player/#/gesture/kick/forward/left",
			"/player/#/gesture/kick/forward/right",
			"/player/#/gesture/doubleArmSide",
			"/player/#/gesture/doubleArmSideClose",
			"/player/#/gesture/jump",
			"/player/#/gesture/clap",
			"/zones/#/activity/discrete",
			"/zones/#/activity/normal",
			"/zones/#/activity/flow/leftwards",
			"/zones/#/activity/flow/rightwards",
			"/zones/#/activity/flow/upwards",
			"/zones/#/activity/flow/downwards"
		};

	}
}
