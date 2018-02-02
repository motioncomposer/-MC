
#include "gesture.h"

namespace mc
{
	namespace gesture
	{

		// the FlowResult comes into a separate header file
		void GestureDetection::apply(const mc::structures::FlowResult& flowResult, mc::structures::GestureResult& gestureResult)
		{
			gestureResult.hitDownLeft = isHitDownLeft(flowResult);
			gestureResult.hitDownRight = isHitDownRight(flowResult);
		}

		// this will become internal functions afterwards
		bool GestureDetection::isHitDownLeft(const mc::structures::FlowResult &flowResult)
		{

			if (isReadyLeft)
			{
				if (flowResult.downwardsLeft > minFlow)
					++curDownLeft;

				if (curDownLeft > minDown)
				{
					isReadyLeft = false;
					curDownLeft = 0;
					return true;
				}
			}
			else
			{
				if (flowResult.upwardsLeft > minFlow)
					++curUpLeft;

				if (curUpLeft > minUp)
				{
					isReadyLeft = true;
					curUpLeft = 0;
				}
			}

			return false;
		}

		bool GestureDetection::isHitDownRight(const mc::structures::FlowResult &flowResult)
		{

			if (isReadyRight)
			{
				if (flowResult.downwardsRight > minFlow)
					++curDownRight;

				if (curDownRight > minDown)
				{
					isReadyRight = false;
					curDownRight = 0;
					return true;
				}
			}
			else
			{
				if (flowResult.upwardsRight > minFlow)
					++curUpRight;

				if (curUpRight > minUp)
				{
					isReadyRight = true;
					curUpRight = 0;
				}
			}

			return false;
		}

		bool GestureDetection::isHitSideLeft()
		{

		}

		bool GestureDetection::isHitSideRight()
		{

		}

		bool GestureDetection::isHitFrontLeft()
		{

		}

		bool GestureDetection::isHitFrontRight()
		{

		}

		bool GestureDetection::isKickLeft()
		{

		}

		bool GestureDetection::isKickRight()
		{

		}

		void GestureDetection::setMinDown(ushort minDown)
		{
			GestureDetection::minDown = minDown;
		}

		void GestureDetection::setMinUp(ushort minUp)
		{
			GestureDetection::minUp = minUp;
		}

		void GestureDetection::setMinFlow(float minFlow)
		{
			GestureDetection::minFlow = minFlow;
		}

	}
}