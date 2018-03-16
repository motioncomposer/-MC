#pragma once

#include <array>
#include <condition_variable>

#include "opencv2\core.hpp"

namespace mc
{

	namespace structures
	{


		// ==========================================================================================================================
		//
		// structure that bundles the activation information provided by control module to be passed to the tracking core
		//
		// ==========================================================================================================================


		using PlayerActivation = std::array<bool, 2>;
		using ZoneActivation = std::array<bool, 6>;


		struct Activation
		{
			PlayerActivation player;
			ZoneActivation zone;

			Activation() : player{ false, false }, zone{ false, false, false, false, false, false }
			{}
		};


		// ==========================================================================================================================
		//
		// structure that bundles the selection information provided by control module to be passed to the tracking core
		//
		// ==========================================================================================================================
		

		using PlayerUpdateTime = std::array<std::chrono::high_resolution_clock::time_point, 2>;

		using PlayerSelection = std::array<int32_t, 2>;
		using ZoneSelection = std::array<cv::Rect, 6>;


		struct Selection
		{
			PlayerUpdateTime time;
			PlayerSelection player;
			
			ZoneSelection zone;

			Selection() : time{ std::chrono::high_resolution_clock::now(), std::chrono::high_resolution_clock::now() }, player{ -1, -1 },
				zone{ cv::Rect(-1, -1, 0, 0), cv::Rect(-1, -1, 0, 0), cv::Rect(-1, -1, 0, 0), cv::Rect(-1, -1, 0, 0), cv::Rect(-1, -1, 0, 0), cv::Rect(-1, -1, 0, 0) }
			{}
		};


		// ==========================================================================================================================
		//
		// structure for exchanging some environment specific values
		//
		// ==========================================================================================================================


		struct Environment
		{
			float maxDistance;
			float minDistance;
			float floorLvl;

			Environment() : maxDistance(10000.f), minDistance(0.f), floorLvl(0.f)
			{}
		};


		// ==========================================================================================================================
		//
		// structure contains the raw result provided by the mc::blob::Blobfinder to be passed to following estimators
		//
		// ==========================================================================================================================


		struct TrackerData
		{

			cv::Point2f tl;
			cv::Point2f br;
			cv::Point2f cp;

			float distance;

			cv::Vec3f metricTL;
			cv::Vec3f metricBR;
			cv::Vec3f metricCP;

			std::vector<uint32_t> contourIdx;

			TrackerData() : tl(-1.f, -1.f), br(-1.f, -1.f), cp(-1.f, -1.f), distance(0.f),
				metricTL(-1.f, -1.f, -1.f), metricBR(-1.f, -1.f, -1.f), metricCP(-1.f, -1.f, -1.f)
			{}
		};


		// ==========================================================================================================================
		//
		// structure contains some location related results to be passed to the following estimators
		//
		// ==========================================================================================================================


		struct LocationData
		{
			bool present;
			bool ready;
			bool outOfRange;

			LocationData() : present(false), ready(false), outOfRange(false)
			{}
		};


		// ==========================================================================================================================
		//
		// structure contains some position related results to be passed to the following estimators
		//
		// ==========================================================================================================================


		struct PositionData
		{

			// I believe I only have this data in here because we need to share this to the code of the main function ...
			// oh wait, we can use it for the activity estimation in order to find the head area ...
			float metricHeight;
			float pixelHeight;

			uint32_t topLvl;

			float metricShoulder;
			float pixelShoulder;

			uint32_t shoulderLvl;

			float metricCenter;
			float pixelCenter;

			uint32_t midLvl;

			// here we have to add some data for the gesture detection

			PositionData() : metricHeight(0.f),
				pixelHeight(0.f),
				topLvl(0.f),
				metricShoulder(0.f),
				pixelShoulder(0.f),
				shoulderLvl(0.f),
				metricCenter(0.f),
				pixelCenter(0.f),
				midLvl(0.f)
			{}
		};


		// ==========================================================================================================================
		//
		// structure bundles all shared data
		//
		// ==========================================================================================================================


		struct SharedData
		{
			Environment environment;

			std::array<TrackerData, 2> trackerData;
			std::array<LocationData, 2> locationData;
			std::array<PositionData, 2> positionData;

			SharedData() = default;
		};


		// ==========================================================================================================================
		//
		// structure bundles tracker and contour information for streaming purpose
		//
		// ==========================================================================================================================


		// may be we can design this in some other way ...
		struct StreamData
		{
			std::vector<uint32_t> trackerIDs;
			std::vector<std::vector<uint32_t>> associatedBlobs;

			StreamData() = default;
		};


		// ==========================================================================================================================
		//
		// structure stores the discrete results of a player
		//
		// ==========================================================================================================================


		struct DiscreteResult
		{
			bool handLeft;
			bool handRight;
			bool head;
			bool legLeft;
			bool legRight;
			bool bodyUpper;
			bool bodyLower;
			bool bodyLeft;
			bool bodyRight;

			DiscreteResult() : handLeft(false),
				handRight(false),
				head(false),
				legLeft(false),
				legRight(false),
				bodyUpper(false),
				bodyLower(false),
				bodyLeft(false),
				bodyRight(false)
			{}
		};


		// ==========================================================================================================================
		//
		// structure stores the normal results of a player
		//
		// ==========================================================================================================================


		struct NormalResult
		{
			float handLeft;
			float handRight;
			float head;
			float legLeft;
			float legRight;
			float bodyUpper;
			float bodyLower;
			float bodyLeft;
			float bodyRight;

			NormalResult() : handLeft(0.f),
				handRight(0.f),
				head(0.f),
				legLeft(0.f),
				legRight(0.f),
				bodyUpper(0.f),
				bodyLower(0.f),
				bodyLeft(0.f),
				bodyRight(0.f)
			{}
		};


		// ==========================================================================================================================
		//
		// structure stores the flow results of a player
		//
		// ==========================================================================================================================


		struct FlowResult
		{
			float leftwardsLeft;
			float leftwardsRight;
			float rightwardsLeft;
			float rightwardsRight;
			float upwardsLeft;
			float upwardsRight;
			float downwardsLeft;
			float downwardsRight;

			FlowResult() : leftwardsLeft(0.f),
				leftwardsRight(0.f),
				rightwardsLeft(0.f),
				rightwardsRight(0.f),
				upwardsLeft(0.f),
				upwardsRight(0.f),
				downwardsLeft(0.f),
				downwardsRight(0.f)
			{}
		};


		// ==========================================================================================================================
		//
		// structure groups the discrete, normal and flow results of a player
		//
		// ==========================================================================================================================


		struct ActivityResult
		{
			DiscreteResult discrete;
			NormalResult normal;
			FlowResult flow;
		};


		// ==========================================================================================================================
		//
		// structure stores the location results of a player
		//
		// ==========================================================================================================================


		struct LocationResult
		{
			bool ready;
			bool present;
			bool outOfRange;

			float centerX;
			float centerZ;

			LocationResult() : ready(false),
				present(false),
				outOfRange(false),
				centerX(0.f),
				centerZ(0.f)
			{}
		};


		// ==========================================================================================================================
		//
		// structure stores the position results of a player
		//
		// ==========================================================================================================================


		struct PositionResult
		{
			float height;
			int heightLevel;

			float verticalHandLeft;
			float verticalHandRight;
			float sideHandLeft;
			float sideHandRight;
			float sideFootLeft;
			float sideFootRight;
			float frontHandLeft;
			float frontHandRight;
			float frontFootLeft;
			float frontFootRight;

			float width;

			bool peak;

			PositionResult() : height(0.f),
				heightLevel(0),
				verticalHandLeft(0.f),
				verticalHandRight(0.f),
				sideHandLeft(0.f),
				sideHandRight(0.f),
				sideFootLeft(0.f),
				sideFootRight(0.f),
				frontHandLeft(0.f),
				frontHandRight(0.f),
				frontFootLeft(0.f),
				frontFootRight(0.f),
				width(0.f),
				peak(false)
			{}
		};


		// ==========================================================================================================================
		//
		// structure stores the gesture results of a player
		//
		// ==========================================================================================================================


		struct GestureResult
		{
			bool hitOverhead;
			bool hitSideLeft;
			bool hitSideRight;
			bool hitDownLeft;
			bool hitDownRight;
			bool hitForwardLeft;
			bool hitForwardRight;
			bool kickSideLeft;
			bool kickSideRight;
			bool kickForwardLeft;
			bool kickForwardRight;
			bool doubleArmSide;
			bool doubleArmSideClose;
			bool jump;
			bool clap;

			GestureResult() : hitOverhead(false),
				hitSideLeft(false),
				hitSideRight(false),
				hitDownLeft(false),
				hitDownRight(false),
				hitForwardLeft(false),
				hitForwardRight(false),
				kickSideLeft(false),
				kickSideRight(false),
				kickForwardLeft(false),
				kickForwardRight(false),
				doubleArmSide(false),
				doubleArmSideClose(false),
				jump(false),
				clap(false)
			{}
		};


		// ==========================================================================================================================
		//
		// structure bundles all player results
		//
		// ==========================================================================================================================


		struct PlayerResult
		{
			ActivityResult activity;
			LocationResult location;
			PositionResult position;
			GestureResult gesture;

			PlayerResult() = default;
		};


		// ==========================================================================================================================
		//
		// structure bundles all zone results
		//
		// ==========================================================================================================================


		struct ZoneResult
		{
			bool discrete;
			float normal;
			float flowLeftwards;
			float flowRightwards;
			float flowUpwards;
			float flowDownwards;

			ZoneResult() : discrete(false),
				normal(0.f),
				flowLeftwards(0.f),
				flowRightwards(0.f),
				flowUpwards(0.f),
				flowDownwards(0.f)
			{}
		};


		// ==========================================================================================================================
		//
		// overall result structure that contains all estimated results
		//
		// ==========================================================================================================================


		struct Result
		{
			std::array<PlayerResult, 2> player;
			std::array<ZoneResult, 6> zone;

			Result() = default;
		};


		// ==========================================================================================================================
		//
		// structure stores the retrieved player selection from control module
		//
		// ==========================================================================================================================


		struct ControlPlayer
		{

			std::chrono::high_resolution_clock::time_point time;
			
			bool tracking;
			int32_t blob;


			ControlPlayer() : tracking(false), blob(-1)
			{}
		};


		// ==========================================================================================================================
		//
		// structure stores the retrieved zone selection from control module
		//
		// ==========================================================================================================================


		struct ControlZone
		{
			bool tracking;
			cv::Rect blob;

			ControlZone() : tracking(false), blob(-1, -1, 0, 0)
			{}
		};


		// ==========================================================================================================================
		//
		// overall control structure that contains everything retrieved from control module
		//
		// ==========================================================================================================================


		struct ControlBundle
		{

			std::mutex mtx;
			std::condition_variable cond;

			int32_t state;

			std::array<ControlPlayer, 2> player;
			std::array<ControlZone, 6> zone;

			ControlBundle() : state(0)
			{}
		};


		// ==========================================================================================================================
		//
		// structure stores the retrieved player message requests from music environment
		//
		// ==========================================================================================================================


		struct MusicPlayer
		{

			bool sendDiscreteHandLeft; // 1
			bool sendDiscreteHandRight; // 1
			bool sendDiscreteHead; // 1
			bool sendDiscreteLegLeft; // 1
			bool sendDiscreteLegRight; // 1
			bool sendDiscreteBodyUpper; // 1
			bool sendDiscreteBodyLower; // 1
			bool sendDiscreteBodyLeft; // 1
			bool sendDiscreteBodyRight; // 1

			bool sendNormalHandLeft; // 1
			bool sendNormalHandRight; // 1
			bool sendNormalHead; // 1
			bool sendNormalLegLeft; // 1
			bool sendNormalLegRight; // 1
			bool sendNormalBodyUpper; // 1
			bool sendNormalBodyLower; // 1
			bool sendNormalBodyLeft; // 1
			bool sendNormalBodyRight; // 1

			bool sendFlowLeftwardsLeft; // 2
			bool sendFlowLeftwardsRight; // 2
			bool sendFlowRightwardsLeft; // 2
			bool sendFlowRightwardsRight; // 2
			bool sendFlowUpwardsLeft; // 2
			bool sendFlowUpwardsRight; // 2
			bool sendFlowDownwardsLeft; // 2
			bool sendFlowDownwardsRight; // 2

			bool sendLocationReady; // 1
			bool sendLocationPresent; // 1
			bool sendLocationCenterX; // 1
			bool sendLocationCenterZ; // 3
			bool sendLocationOutOfRange; // 2

			bool sendPositionHeight; // 1
			bool sendPositionHeightLevel; // 1
			bool sendPositionPeak; // 1
			bool sendPositionVerticalHandLeft; // 1
			bool sendPositionVerticalHandRight; // 1
			bool sendPositionSideHandLeft; // 1
			bool sendPositionSideHandRight; // 1
			bool sendPositionSideFootLeft; // 2
			bool sendPositionSideFootRight; // 2
			bool sendPositionFrontHandLeft; // 2
			bool sendPositionFrontHandRight; // 2
			bool sendPositionFrontFootLeft; // 2
			bool sendPositionFrontFootRight; // 2
			bool sendPositionWidth; // 1

			bool sendGestureHitOverhead; // 1
			bool sendGestureHitSideLeft; // 1
			bool sendGestureHitSideRight; // 1
			bool sendGestureHitDownLeft; // 2
			bool sendGestureHitDownRight; // 2
			bool sendGestureHitForwardLeft; // 1
			bool sendGestureHitForwardRight; // 1
			bool sendGestureKickSideLeft; // 1
			bool sendGestureKickSideRight; // 1
			bool sendGestureKickForwardLeft; // 1
			bool sendGestureKickForwardRight; // 1
			bool sendGestureDoubleArmSide; // 1
			bool sendGestureDoubleArmSideClose; // 1
			bool sendGestureJump; // 2
			bool sendGestureClap; // 3

			MusicPlayer() : sendDiscreteHandLeft(false),
				sendDiscreteHandRight(false),
				sendDiscreteHead(false),
				sendDiscreteLegLeft(false),
				sendDiscreteLegRight(false),
				sendDiscreteBodyUpper(false),
				sendDiscreteBodyLower(false),
				sendDiscreteBodyLeft(false),
				sendDiscreteBodyRight(false),
				sendNormalHandLeft(false),
				sendNormalHandRight(false),
				sendNormalHead(false),
				sendNormalLegLeft(false),
				sendNormalLegRight(false),
				sendNormalBodyUpper(false),
				sendNormalBodyLower(false),
				sendNormalBodyLeft(false),
				sendNormalBodyRight(false),
				sendFlowLeftwardsLeft(false),
				sendFlowLeftwardsRight(false),
				sendFlowRightwardsLeft(false),
				sendFlowRightwardsRight(false),
				sendFlowUpwardsLeft(false),
				sendFlowUpwardsRight(false),
				sendFlowDownwardsLeft(false),
				sendFlowDownwardsRight(false),
				sendLocationReady(false),
				sendLocationPresent(false),
				sendLocationCenterX(false),
				sendLocationCenterZ(false),
				sendLocationOutOfRange(false),
				sendPositionHeight(false),
				sendPositionHeightLevel(false),
				sendPositionPeak(false),
				sendPositionVerticalHandLeft(false),
				sendPositionVerticalHandRight(false),
				sendPositionSideHandLeft(false),
				sendPositionSideHandRight(false),
				sendPositionSideFootLeft(false),
				sendPositionSideFootRight(false),
				sendPositionFrontHandLeft(false),
				sendPositionFrontHandRight(false),
				sendPositionFrontFootLeft(false),
				sendPositionFrontFootRight(false),
				sendPositionWidth(false),
				sendGestureHitOverhead(false),
				sendGestureHitSideLeft(false),
				sendGestureHitSideRight(false),
				sendGestureHitDownLeft(false),
				sendGestureHitDownRight(false),
				sendGestureHitForwardLeft(false),
				sendGestureHitForwardRight(false),
				sendGestureKickSideLeft(false),
				sendGestureKickSideRight(false),
				sendGestureKickForwardLeft(false),
				sendGestureKickForwardRight(false),
				sendGestureDoubleArmSide(false),
				sendGestureDoubleArmSideClose(false),
				sendGestureJump(false),
				sendGestureClap(false)
			{}

		};


		// ==========================================================================================================================
		//
		// structure stores the retrieved zone message requests from music environment
		//
		// ==========================================================================================================================


		struct MusicZone
		{
			bool sendDiscrete; // 1
			bool sendNormal; // 1
			bool sendFlowLeftwards; // 2
			bool sendFlowRightwards; // 2
			bool sendFlowUpwards; // 2
			bool sendFlowDownwards; // 2

			MusicZone() : sendDiscrete(false),
				sendNormal(false),
				sendFlowLeftwards(false),
				sendFlowRightwards(false),
				sendFlowUpwards(false),
				sendFlowDownwards(false)
			{}

		};


		// ==========================================================================================================================
		//
		// overall control structure that contains everything retrieved from music environment
		//
		// ==========================================================================================================================


		struct MusicBundle
		{
			std::array<MusicPlayer, 2> player;
			std::array<MusicZone, 6> zone;

			MusicBundle() = default;
		};

	}
}
