#pragma once

#include "core.h"

#include "structures.h"

#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\video.hpp"

#include <array>
#include <vector>

namespace mc
{
	namespace flow
	{


		// normal, discrete and flow should be merged together into one estimator

		// for apply on zones --> no disparity map is needed --> grid mode like it is performed here, but only on some sub images
		// the roi is actually part of the zone specification


		struct FlowParameter;

		class FlowEstimation
		{
		public:

			FlowEstimation(const cv::Size& imageSize, size_t gridStep, double minThreshold, double maxThreshold, float expScale, float filterConst,
				const cv::Size& winSize = { 31, 31 }, int maxLevel = 5) : imageSize(imageSize), gridStep(gridStep), minThreshold(minThreshold),
				maxThreshold(maxThreshold), expScale(expScale), filterLeft({ fs::core::SimpleBranchedFilterFloat(0.f, filterConst), fs::core::SimpleBranchedFilterFloat(0.f, filterConst),
					fs::core::SimpleBranchedFilterFloat(0.f, filterConst), fs::core::SimpleBranchedFilterFloat(0.f, filterConst) }), filterRight({ fs::core::SimpleBranchedFilterFloat(0.f, filterConst),
						fs::core::SimpleBranchedFilterFloat(0.f, filterConst), fs::core::SimpleBranchedFilterFloat(0.f, filterConst), fs::core::SimpleBranchedFilterFloat(0.f, filterConst) })
			{
				sparsePyrLKOptivalFlow = cv::SparsePyrLKOpticalFlow::create(winSize, maxLevel);
				_initGrid();
			}

			bool apply(const cv::Mat& last, const cv::Mat& cur, const cv::Point& center, mc::structures::FlowResult& flowResult,
				const cv::Mat& mask = cv::Mat());

			bool draw(cv::Mat& image, const cv::Scalar& color_grid = { 0, 255, 0 }, const cv::Scalar& color_flow = { 0, 0, 255 }, double ofFactor = 16.0,
				const cv::Mat& mask = cv::Mat());

			// we some how need to get rid of this grid ;)
			void setOpticalFlowParameter(const cv::Size& winSize = { 31, 31 }, int maxLevel = 5);

			void setGridParameter(size_t gridStep);

			void setFilterParameter(double minThreshold, double maxThreshold);

			void setExpScale(float expScale);

			void setFilterConst(float filterConst);


		private:

			cv::Size imageSize;

			size_t gridStep;
			double minThreshold;
			double maxThreshold;

			cv::Ptr<cv::SparsePyrLKOpticalFlow> sparsePyrLKOptivalFlow;

			std::vector<cv::Point2f> gridPoints;
			std::vector<cv::Point2f> shiftedGridPoints;

			std::vector<uchar> status;
			std::vector<float> error;

			size_t counterLeft, counterRight;

			float expScale;

			std::array<fs::core::SimpleBranchedFilterFloat, 4> filterLeft;
			std::array<fs::core::SimpleBranchedFilterFloat, 4> filterRight;

			// internal function
			void _initGrid();

			void _getFlowWithMask(const cv::Point& center, mc::structures::FlowResult& flowResult, const cv::Mat& mask);

			void _getFlowWithoutMask(const cv::Point& center, mc::structures::FlowResult& flowResult);

			void _filterFlow(mc::structures::FlowResult& flowResult);

			void _drawFlowWithMask(cv::Mat& image, const cv::Scalar& color_grid, const cv::Scalar& color_flow, double ofFactor, const cv::Mat& mask);

			void _drawFlowWithoutMask(cv::Mat& image, const cv::Scalar& color_grid, const cv::Scalar& color_flow, double ofFactor);
		};
	}
}