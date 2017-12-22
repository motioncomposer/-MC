#pragma once

#include "core.h"

#include "result.h"

#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp"
#include "opencv2\calib3d.hpp"
#include "opencv2\video.hpp"

#include <array>
#include <vector>
#include <iostream>
#include <mutex>
#include <string>
#include <chrono>
#include <random>

namespace mc
{
	namespace normal
	{

		// normal, discrete and flow should be merged together into one estimator

		// for this we also need an .xml I/O (for the parameters in general)
		struct NormalParameter;

		class NormalEstimation
		{
		public:

			NormalEstimation(const cv::Size& imageSize, int minDifference, float expScale, float filterConst) : imageSize(imageSize),
				minDifference(minDifference), expScale(expScale), filterLeft(0.f, filterConst), filterRight(0.f, filterConst) {}

			bool apply(const cv::Mat& last, const cv::Mat& cur, const cv::Point& center, mc::result::NormalResult& normalResult,
				const cv::Mat& mask = cv::Mat());

			bool draw(cv::Mat& image, const cv::Scalar& color = { 0, 255, 0 }, const cv::Mat& mask = cv::Mat());

			// this can be improved by using the mc::normal::NormalParameter
			void setMinDifference(int minDifference);

			void setExpScale(float expScale);

			void setFilterConst(float filterConst);


		private:

			cv::Size imageSize;
			int minDifference;
			float expScale;

			fs::core::SimpleBranchedFilterFloat filterLeft;
			fs::core::SimpleBranchedFilterFloat filterRight;

			cv::Mat diff;
			cv::Mat thresh;
			std::mutex mtxLeft, mtxRight;
			size_t normalLeftAbsolute, normalRightAbsolute;

			// internal function
			void _getNormalWithMask(const cv::Point& center, float& normalLeft, float& normalRight, const cv::Mat& mask);

			void _getNormalWithoutMask(const cv::Point& center, float& normalLeft, float& normalRight);

			void _filterNormal(float& normalLeft, float& normalRight);

			void _drawNormalWithMask(cv::Mat& image, const cv::Scalar& color, const cv::Mat& mask);

			void _drawNormalWithoutMask(cv::Mat& image, const cv::Scalar& color);
		};
	}
}
