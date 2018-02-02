
#include "flow.h"

namespace mc
{
	namespace flow
	{
		bool FlowEstimation::apply(const cv::Mat& last, const cv::Mat& cur, const cv::Point& center, mc::structures::FlowResult& flowResult,
			const cv::Mat& mask)
		{
			if (last.empty() || last.type() != CV_8UC1 || cur.empty() || cur.type() != CV_8UC1
				|| last.size() != imageSize || cur.size() != imageSize)
				return false;

			// actually it is crap to calculate the flow for the complete image --> may be we can find some interresting points with the hill climber
			// approach on disparity map (may be in combination with good_features to track)
			sparsePyrLKOptivalFlow->calc(last, cur, gridPoints, shiftedGridPoints, status, error);

			if (mask.empty() || mask.type() != CV_8UC1 || mask.size() != imageSize)
				_getFlowWithoutMask(center, flowResult);
			else
				_getFlowWithMask(center, flowResult, mask);

			_filterFlow(flowResult);

			return true;
		}


		bool FlowEstimation::draw(cv::Mat& image, const cv::Scalar& color_grid, const cv::Scalar& color_flow, double ofFactor, const cv::Mat& mask)
		{
			if (image.empty() || image.type() != CV_8UC3)
				return false;

			if (mask.empty() || mask.type() != CV_8UC1 || mask.size() != image.size())
				_drawFlowWithoutMask(image, color_grid, color_flow, ofFactor);
			else
				_drawFlowWithMask(image, color_grid, color_flow, ofFactor, mask);

			return true;
		}


		void FlowEstimation::setOpticalFlowParameter(const cv::Size& winSize, int maxLevel)
		{
			sparsePyrLKOptivalFlow = cv::SparsePyrLKOpticalFlow::create(winSize, maxLevel);
		}


		void FlowEstimation::setGridParameter(size_t gridStep)
		{
			FlowEstimation::gridStep = gridStep;

			_initGrid();
		}


		void FlowEstimation::setFilterParameter(double minThreshold, double maxThreshold)
		{
			FlowEstimation::minThreshold = minThreshold;
			FlowEstimation::maxThreshold = maxThreshold;
		}


		void FlowEstimation::setExpScale(float expScale)
		{
			FlowEstimation::expScale = expScale;
		}


		void FlowEstimation::setFilterConst(float filterConst)
		{
			filterLeft[0].setAlpha(filterConst);
			filterLeft[1].setAlpha(filterConst);
			filterLeft[2].setAlpha(filterConst);
			filterLeft[3].setAlpha(filterConst);
			filterRight[0].setAlpha(filterConst);
			filterRight[1].setAlpha(filterConst);
			filterRight[2].setAlpha(filterConst);
			filterRight[3].setAlpha(filterConst);
		}


		// internal function
		void FlowEstimation::_initGrid()
		{
			gridPoints.clear();
			for (auto y = gridStep; y < imageSize.height; y += gridStep)
			{
				for (auto x = gridStep; x < imageSize.width; x += gridStep)
				{
					gridPoints.push_back(cv::Point2f(x, y));
				}
			}

			shiftedGridPoints.clear();
			shiftedGridPoints.resize(gridPoints.size());

			status.clear();
			status.resize(gridPoints.size());
			error.clear();
			error.resize(gridPoints.size());
		}


		void FlowEstimation::_getFlowWithMask(const cv::Point& center, mc::structures::FlowResult& flowResult, const cv::Mat& mask)
		{
			// calculating the activity values;
			flowResult = mc::structures::FlowResult();

			counterLeft = 0;
			counterRight = 0;
			for (auto&& c = 0; c < gridPoints.size(); ++c)
			{
				cv::Point2f dmy = (shiftedGridPoints[c] - gridPoints[c]);
				if ((mask.at<uchar>(gridPoints[c]) != 0) && (cv::norm(dmy) > minThreshold) && (cv::norm(dmy) < maxThreshold))
				{
					if (gridPoints[c].x < center.x)
					{
						if (dmy.x < 0)
							flowResult.leftwardsLeft -= dmy.x;
						else
							flowResult.rightwardsLeft += dmy.x;

						if (dmy.y < 0)
							flowResult.upwardsLeft -= dmy.y;
						else
							flowResult.downwardsLeft += dmy.y;

						++counterLeft;
					}
					else
					{
						if (dmy.x < 0)
							flowResult.leftwardsRight -= dmy.x;
						else
							flowResult.rightwardsRight += dmy.x;

						if (dmy.y < 0)
							flowResult.upwardsRight -= dmy.y;
						else
							flowResult.downwardsRight += dmy.y;

						++counterRight;
					}
				}
			}

			if (counterLeft != 0)
			{
				flowResult.leftwardsLeft /= counterLeft;
				flowResult.rightwardsLeft /= counterLeft;
				flowResult.upwardsLeft /= counterLeft;
				flowResult.downwardsLeft /= counterLeft;
			}

			if (counterRight != 0)
			{
				flowResult.leftwardsRight /= counterRight;
				flowResult.rightwardsRight /= counterRight;
				flowResult.upwardsRight /= counterRight;
				flowResult.downwardsRight /= counterRight;
			}
		}


		void FlowEstimation::_getFlowWithoutMask(const cv::Point& center, mc::structures::FlowResult& flowResult)
		{
			// calculating the activity values;
			flowResult = mc::structures::FlowResult();

			counterLeft = 0;
			counterRight = 0;
			for (auto&& c = 0; c < gridPoints.size(); ++c)
			{
				cv::Point2f dmy = (shiftedGridPoints[c] - gridPoints[c]);
				if ((cv::norm(dmy) > minThreshold) && (cv::norm(dmy) < maxThreshold))
				{
					if (gridPoints[c].x < center.x)
					{
						if (dmy.x < 0)
							flowResult.leftwardsLeft -= dmy.x;
						else
							flowResult.rightwardsLeft += dmy.x;

						if (dmy.y < 0)
							flowResult.upwardsLeft -= dmy.y;
						else
							flowResult.downwardsLeft += dmy.y;

						++counterLeft;
					}
					else
					{
						if (dmy.x < 0)
							flowResult.leftwardsRight -= dmy.x;
						else
							flowResult.rightwardsRight += dmy.x;

						if (dmy.y < 0)
							flowResult.upwardsRight -= dmy.y;
						else
							flowResult.downwardsRight += dmy.y;

						++counterRight;
					}
				}
			}

			if (counterLeft != 0)
			{
				flowResult.leftwardsLeft /= counterLeft;
				flowResult.rightwardsLeft /= counterLeft;
				flowResult.upwardsLeft /= counterLeft;
				flowResult.downwardsLeft /= counterLeft;
			}

			if (counterRight != 0)
			{
				flowResult.leftwardsRight /= counterRight;
				flowResult.rightwardsRight /= counterRight;
				flowResult.upwardsRight /= counterRight;
				flowResult.downwardsRight /= counterRight;
			}

		}


		void FlowEstimation::_filterFlow(mc::structures::FlowResult& flowResult)
		{
			// mutual exclusion of left / right and up / down flow

			flowResult.leftwardsLeft = flowResult.leftwardsLeft > flowResult.rightwardsLeft ? flowResult.leftwardsLeft : 0.f;
			flowResult.rightwardsLeft = flowResult.rightwardsLeft > flowResult.leftwardsLeft ? flowResult.rightwardsLeft : 0.f;
			flowResult.upwardsLeft = flowResult.upwardsLeft > flowResult.downwardsLeft ? flowResult.upwardsLeft : 0.f;
			flowResult.downwardsLeft = flowResult.downwardsLeft > flowResult.upwardsLeft ? flowResult.downwardsLeft : 0.f;

			flowResult.leftwardsRight = flowResult.leftwardsRight > flowResult.leftwardsRight ? flowResult.leftwardsRight : 0.f;
			flowResult.rightwardsRight = flowResult.rightwardsRight > flowResult.rightwardsRight ? flowResult.rightwardsRight : 0.f;
			flowResult.upwardsRight = flowResult.upwardsRight > flowResult.downwardsRight ? flowResult.upwardsRight : 0.f;
			flowResult.downwardsRight = flowResult.downwardsRight > flowResult.upwardsRight ? flowResult.downwardsRight : 0.f;

			// scaling the values to the range (0, 1] --> problem is that we have jumps because of min Threshold
			// actually expScale is based on maxThreshold
			flowResult.leftwardsLeft = 1.f - std::expf(-1.f * expScale * flowResult.leftwardsLeft);
			flowResult.rightwardsLeft = 1.f - std::expf(-1.f * expScale * flowResult.rightwardsLeft);
			flowResult.upwardsLeft = 1.f - std::expf(-1.f * expScale * flowResult.upwardsLeft);
			flowResult.downwardsLeft = 1.f - std::expf(-1.f * expScale * flowResult.downwardsLeft);

			flowResult.leftwardsRight = 1.f - std::expf(-1.f * expScale * flowResult.leftwardsRight);
			flowResult.rightwardsRight = 1.f - std::expf(-1.f * expScale * flowResult.rightwardsRight);
			flowResult.upwardsRight = 1.f - std::expf(-1.f * expScale * flowResult.upwardsRight);
			flowResult.downwardsRight = 1.f - std::expf(-1.f * expScale * flowResult.downwardsRight);

			// this is commented out as this is making problems at the moment
			//flowLeft[0] = filterLeft[0].apply(flowLeft[0]);
			//flowLeft[1] = filterLeft[1].apply(flowLeft[1]);
			//flowLeft[2] = filterLeft[2].apply(flowLeft[2]);
			//flowLeft[3] = filterLeft[3].apply(flowLeft[3]);

			//flowRight[0] = filterRight[0].apply(flowRight[0]);
			//flowRight[1] = filterRight[1].apply(flowRight[1]);
			//flowRight[2] = filterRight[2].apply(flowRight[2]);
			//flowRight[3] = filterRight[3].apply(flowRight[3]);
		}


		void FlowEstimation::_drawFlowWithMask(cv::Mat& image, const cv::Scalar& color_grid, const cv::Scalar& color_flow, double ofFactor, const cv::Mat& mask)
		{
			for (auto& it : gridPoints)
			{
				cv::Point2f dmy = (shiftedGridPoints[&it - &gridPoints[0]] - it);
				if ((status[&it - &gridPoints[0]]) && (mask.at<uchar>(it) != 0) && (cv::norm(dmy) > minThreshold) && (cv::norm(dmy) < maxThreshold))
				{
					const cv::Point2f& fxy = shiftedGridPoints[&it - &gridPoints[0]] - it;
					cv::line(image, it, cv::Point(std::round(it.x + ofFactor * fxy.x / 32.0), std::round(it.y + ofFactor * fxy.y / 32.0)), color_flow, 2);
				}

				circle(image, it, 2, color_grid, -1);
			}
		}


		void FlowEstimation::_drawFlowWithoutMask(cv::Mat& image, const cv::Scalar& color_grid, const cv::Scalar& color_flow, double ofFactor)
		{
			for (auto& it : gridPoints)
			{
				cv::Point2f dmy = (shiftedGridPoints[&it - &gridPoints[0]] - it);
				if ((status[&it - &gridPoints[0]]) && (cv::norm(dmy) > minThreshold) && (cv::norm(dmy) < maxThreshold))
				{
					const cv::Point2f& fxy = shiftedGridPoints[&it - &gridPoints[0]] - it;
					cv::line(image, it, cv::Point(std::round(it.x + ofFactor * fxy.x / 32.0), std::round(it.y + ofFactor * fxy.y / 32.0)), color_flow, 2);
				}

				circle(image, it, 2, color_grid, -1);
			}
		}
	}
}