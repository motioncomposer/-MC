#pragma once

#include "opencv2\core.hpp"

#include "defines.h"


namespace mc
{

	namespace test
	{
		
		// we also need to a language container ... so that we can create messages in different languages?

		// we should make this an enum --> this looks kind of ugly
		enum class TrackingModuleState
		{
			STOP = 2,
			RESTART = 1,
			READY = 0
		};

		// actually this should be a struct ...



		// here we may be add some other structures as well ...
	}




	// ==========================================================================================================================
	//
	// namespace "result" contains structures to store the result values of the tracking system
	//
	// ==========================================================================================================================



	namespace result
	{



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
		// ==========================================================================================================================


		struct ActivityResult
		{
			DiscreteResult discrete;
			NormalResult normal;
			FlowResult flow;
		};


		// ==========================================================================================================================
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
		// ==========================================================================================================================


		template<size_t player_count, size_t zone_count>
		struct _ResultBundle
		{
			PlayerResult playerResults[player_count];
			ZoneResult zoneResults[zone_count];

			_ResultBundle() = default;
		};


		using ResultBundle = _ResultBundle<mc::defines::maxPlayer, mc::defines::maxZones>;


	} // end namespace result




	// ==========================================================================================================================
	//
	// namespace "command" contains structures to for controlling the tracking system and its sending behaviour
	//
	// ==========================================================================================================================

	

	namespace command
	{


		// the stuff in here should be renamed?

		// ==========================================================================================================================


		template<size_t player_count, size_t zone_count>
		struct _CMCommandBundle
		{

			int32_t stateControl;


			bool setPlayerTracking[player_count];
			bool setZoneTracking[zone_count];

			int32_t setPlayerBlob[player_count];
			cv::Rect setZoneBlob[zone_count];

			_CMCommandBundle() : stateControl(0)
			{
				for (auto&& c = 0; c < mc::defines::maxPlayer; ++c)
				{
					setPlayerTracking[c] = false;
					setPlayerBlob[c] = -1;
				}

				for (auto&& c = 0; c < mc::defines::maxZones; ++c)
				{
					setZoneTracking[c] = false;
					setZoneBlob[c] = cv::Rect(-1, -1, 0, 0);
				}
			}

		};


		using CMCommandBundle = _CMCommandBundle<mc::defines::maxPlayer, mc::defines::maxZones>;


		// ==========================================================================================================================
		// ==========================================================================================================================


		// we have to prepend the activity
		// peak is maybe moved to position
		struct MECommandPlayer
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

			bool sendPeak; // 1

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

			MECommandPlayer() : sendDiscreteHandLeft(false),
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
				sendPeak(false),
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



		struct MECommandZone
		{
			bool sendDiscrete; // 1
			bool sendNormal; // 1
			bool sendFlowLeftwards; // 2
			bool sendFlowRightwards; // 2
			bool sendFlowUpwards; // 2
			bool sendFlowDownwards; // 2

			MECommandZone() : sendDiscrete(false),
				sendNormal(false),
				sendFlowLeftwards(false),
				sendFlowRightwards(false),
				sendFlowUpwards(false),
				sendFlowDownwards(false)
			{}

		};


		// ==========================================================================================================================


		template<size_t player_count, size_t zone_count>
		struct _MECommandBundle
		{
			MECommandPlayer playerCommands[player_count];
			MECommandZone zoneCommands[zone_count];

			_MECommandBundle() = default;
		};


		using MECommandBundle = _MECommandBundle<mc::defines::maxPlayer, mc::defines::maxZones>;


	} // end namespace command

} // end namespace mc
