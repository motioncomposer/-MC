
#include "gesture.h"

namespace mc
{
	namespace gesture
	{

		// ==========================================================================================================================
		//
		// gesture estimator parameter
		//
		// ==========================================================================================================================


		void GestureEstimatorParameter::read(const cv::FileNode& fn)
		{
			/*
			fn["offsetFront"] >> offsetFront;
			fn["offsetBack"] >> offsetBack;
			fn["moveThreshold"] >> moveThreshold;
			fn["readyTimeMilli"] >> readyTimeMilli;
			*/
		}


		void GestureEstimatorParameter::write(cv::FileStorage& fs) const
		{
			/*
			fs << "{"
			<< "offsetFront" << offsetFront
			<< "offsetBack" << offsetBack
			<< "moveThreshold" << moveThreshold
			<< "readyTimeMilli" << readyTimeMilli
			<< "}";
			*/
		}


		void read(const cv::FileNode& fn, GestureEstimatorParameter& gesturePara, const GestureEstimatorParameter& default)
		{
			if (fn.empty())
				gesturePara = default;
			else
				gesturePara.read(fn);
		}


		void write(cv::FileStorage& fs, const std::string&, const GestureEstimatorParameter& gesturePara)
		{
			gesturePara.write(fs);
		}


		bool saveGestureEstimatorParameter(const GestureEstimatorParameter& gesturePara,
			const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::WRITE);

			if (fs.isOpened())
			{
				fs << key << gesturePara;
				return true;
			}

			return false;
		}


		bool readGestureEstimatorParameter(GestureEstimatorParameter& gesturePara,
			const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::READ);

			if (fs.isOpened())
			{
				fs[key] >> gesturePara;
				return true;
			}

			return false;
		}


		// ==========================================================================================================================
		//
		// gesture estimator object
		//
		// ==========================================================================================================================

		
		void GestureEstimator::apply(const mc::structures::FlowResult& flowResult, mc::structures::GestureResult& gestureResult)
		{
			gestureResult.hitDownLeft = isHitDownLeft(flowResult);
			gestureResult.hitDownRight = isHitDownRight(flowResult);
		}

		// this will become internal functions afterwards
		bool GestureEstimator::isHitDownLeft(const mc::structures::FlowResult &flowResult)
		{

			if (isReadyLeft)
			{
				if (flowResult.downwardsLeft > parameter.minFlow)
					++curDownLeft;

				if (curDownLeft > parameter.minDown)
				{
					isReadyLeft = false;
					curDownLeft = 0;
					return true;
				}
			}
			else
			{
				if (flowResult.upwardsLeft > parameter.minFlow)
					++curUpLeft;

				if (curUpLeft > parameter.minUp)
				{
					isReadyLeft = true;
					curUpLeft = 0;
				}
			}

			return false;
		}

		bool GestureEstimator::isHitDownRight(const mc::structures::FlowResult &flowResult)
		{

			if (isReadyRight)
			{
				if (flowResult.downwardsRight > parameter.minFlow)
					++curDownRight;

				if (curDownRight > parameter.minDown)
				{
					isReadyRight = false;
					curDownRight = 0;
					return true;
				}
			}
			else
			{
				if (flowResult.upwardsRight > parameter.minFlow)
					++curUpRight;

				if (curUpRight > parameter.minUp)
				{
					isReadyRight = true;
					curUpRight = 0;
				}
			}

			return false;
		}

		bool GestureEstimator::isHitSideLeft()
		{

		}

		bool GestureEstimator::isHitSideRight()
		{

		}

		bool GestureEstimator::isHitFrontLeft()
		{

		}

		bool GestureEstimator::isHitFrontRight()
		{

		}

		bool GestureEstimator::isKickLeft()
		{

		}

		bool GestureEstimator::isKickRight()
		{

		}

	}
}