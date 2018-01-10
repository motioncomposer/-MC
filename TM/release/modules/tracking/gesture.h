#pragma once

#include "core.h"

#include "result.h"

#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"

#include <array>

namespace mc
{
	namespace gesture
	{
		struct GestureParameter;

		//
		// !!! this should not depend on flow result !!!
		//
		class GestureDetection
		{
		public:

			GestureDetection(ushort minDown, ushort minUp, float minFlow) : minDown(minDown), minUp(minUp), minFlow(minFlow), curDownLeft(0),
				curDownRight(0), curUpLeft(0), curUpRight(0), isReadyLeft(false), enableHitDownLeft(false),
				isReadyRight(false), enableHitDownRight(false) {}

			// this should not depend on flow as flow is just of priority 2
			void apply(const mc::result::FlowResult& flowResult, mc::result::GestureResult& gestureResult);

			// this will become internal functions afterwards
			// switch order so that they match the ordering in the spec
			bool isHitDownLeft(const mc::result::FlowResult &flowResult);

			bool isHitDownRight(const mc::result::FlowResult &flowResult);

			bool isHitSideLeft();
			
			bool isHitSideRight();

			bool isHitFrontLeft();

			bool isHitFrontRight();

			bool isKickLeft();
			
			bool isKickRight();



			void setMinDown(ushort minDown);

			void setMinUp(ushort minUp);

			void setMinFlow(float minFlow);

		private:

			// we can assume that these two are the same
			ushort minDown;
			ushort minUp;
			float minFlow;

			ushort curDownLeft;
			ushort curDownRight;
			bool isReadyLeft;
			bool enableHitDownLeft;

			ushort curUpLeft;
			ushort curUpRight;
			bool isReadyRight;
			bool enableHitDownRight;

		};
	}
}
