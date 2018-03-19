#pragma once

#include "core.h"

#include "structures.h"

#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"

#include <array>

namespace mc
{
	namespace gesture
	{

		// ==========================================================================================================================
		//
		// gesture estimator parameter
		//
		// ==========================================================================================================================


		struct GestureEstimatorParameter
		{

			// we can assume that these two are the same
			// they are cutted out from the mc::gesture::GestureEstimator ... don't know if we need them ...
			// but  thikn we first have to deal with the position estimator ...
			// we can say, jump is only possible if the blob is in side the legal region of interest ...

			// when we have learned the initial height and the initial centroid ... they should stay on the same level ...
			ushort minDown;
			ushort minUp;
			float minFlow;



			GestureEstimatorParameter() = default;

			void read(const cv::FileNode& fn);

			void write(cv::FileStorage& fs) const;

		};


		void read(const cv::FileNode& fn, GestureEstimatorParameter& gesturePara, const GestureEstimatorParameter& default = GestureEstimatorParameter());

		void write(cv::FileStorage& fs, const std::string&, const GestureEstimatorParameter& gesturePara);

		bool saveGestureEstimatorParameter(const GestureEstimatorParameter& gesturePara,
			const std::string& filename, const std::string& key);

		bool readGestureEstimatorParameter(GestureEstimatorParameter& gesturePara,
			const std::string& filename, const std::string& key);


		// ==========================================================================================================================
		//
		// helper structures and functions for gesture estimator
		//
		// ==========================================================================================================================

		// nothing here yet ...

		// ==========================================================================================================================
		//
		// kernel object for gesture estimator
		//
		// ==========================================================================================================================


		struct GestureEstimatorKernel
		{
			// let's check if we need this ...
			ushort curDownLeft;
			ushort curDownRight;
			bool isReadyLeft;
			bool enableHitDownLeft;

			ushort curUpLeft;
			ushort curUpRight;
			bool isReadyRight;
			bool enableHitDownRight;

			// we need to define some defalt parameter ...
			GestureEstimatorKernel() = default;
		};


		// ==========================================================================================================================
		//
		// gesture estimator object
		//
		// ==========================================================================================================================


		class GestureEstimator
		{
		public:


			GestureEstimator(const GestureEstimatorParameter& parameter) : parameter(parameter), trackerID{ -1, -1 }
			{}

			// here we can take the default parameter ...
			// we haven't prvided some default parameter here for min Up minDown and minFlow ... minFlow refers to the velocity of the movement ...
			GestureEstimator(ushort minDown, ushort minUp, float minFlow) : curDownLeft(0),
				curDownRight(0), curUpLeft(0), curUpRight(0), isReadyLeft(false), enableHitDownLeft(false),
				isReadyRight(false), enableHitDownRight(false) {}

			// this should not depend on flow as flow is just of priority 2
			// here we have to hand in shared ... or directly the position
			// we need some timers ...
			// we have to use some kind of state machin in order to detect the hits and kicks (side and frnot is possible ... for overhead we need to im prove the arm height ...)
			void apply(const mc::structures::FlowResult& flowResult, mc::structures::GestureResult& gestureResult);

			// this will become internal functions afterwards
			// switch order so that they match the ordering in the spec
			// this is may be only possible when the hand are not too close to the body ...
			// no need for flowResult ...
			bool isHitDownLeft(const mc::structures::FlowResult &flowResult);

			bool isHitDownRight(const mc::structures::FlowResult &flowResult);


			//
			bool isHitSideLeft();
			
			bool isHitSideRight();

			bool isHitFrontLeft();

			bool isHitFrontRight();

			bool isKickLeft();
			
			bool isKickRight();
			//


		private:


			// how should the gesture detector deal with suddenly appearing swaps?

			GestureEstimatorParameter parameter;

			// do we need the tracker ids here?
			mc::structures::PlayerSelection trackerID;
			

			// to kernel ... jupp ... but we first have to deal with position ...
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
