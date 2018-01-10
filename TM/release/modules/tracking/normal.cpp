
#include "normal.h"

namespace mc
{
	namespace normal
	{

		bool NormalEstimation::apply(const cv::Mat& last, const cv::Mat& cur, const cv::Point& center, mc::result::NormalResult& normalResult,
			const cv::Mat& mask)
		{
			if (last.empty() || last.type() != CV_8UC1 || cur.empty() || cur.type() != CV_8UC1
				|| last.size() != imageSize || cur.size() != imageSize)
				return false;


			cv::absdiff(last, cur, diff);
			cv::threshold(diff, thresh, minDifference, 255, cv::ThresholdTypes::THRESH_BINARY);

			if (mask.empty() || mask.type() != CV_8UC1 || mask.size() != imageSize)
				_getNormalWithoutMask(center, normalResult.normalBodyLeft, normalResult.normalBodyRight);
			else
				_getNormalWithMask(center, normalResult.normalBodyLeft, normalResult.normalBodyRight, mask);

			// in this function we can put the std::exp scaling
			_filterNormal(normalResult.normalBodyLeft, normalResult.normalBodyRight);

			return true;
		}


		bool NormalEstimation::draw(cv::Mat& image, const cv::Scalar& color, const cv::Mat& mask)
		{
			if (image.empty() || image.type() != CV_8UC3)
				return false;

			if (mask.empty() || mask.type() != CV_8UC1 || mask.size() != image.size())
				_drawNormalWithoutMask(image, color);
			else
				_drawNormalWithMask(image, color, mask);

			return true;
		}


		void NormalEstimation::setMinDifference(int minDifference)
		{
			NormalEstimation::minDifference = minDifference;
		}


		void NormalEstimation::setExpScale(float expScale)
		{
			NormalEstimation::expScale = expScale;
		}


		void NormalEstimation::setFilterConst(float filterConst)
		{
			filterLeft.setAlpha(filterConst);
			filterRight.setAlpha(filterConst);
		}


		void NormalEstimation::_getNormalWithMask(const cv::Point& center, float& normalLeft, float& normalRight, const cv::Mat& mask)
		{
			normalLeftAbsolute = 0;
			normalRightAbsolute = 0;
			thresh.forEach<uchar>([&](const uchar& value, const int* position)
			{
				if (value != 0 && mask.at<uchar>(position[0], position[1]) != 0)
				{
					if (position[1] < center.x)
					{
						std::lock_guard<std::mutex> guard(mtxLeft);
						++normalLeftAbsolute;
					}
					else
					{
						std::lock_guard<std::mutex> guard(mtxRight);
						++normalRightAbsolute;
					}

				}
			});

			// applying a limit to normal --> actually we need a depth compensation here
			// may be we can simplify the sum computation by ready-to-use OpenCV functions
			normalLeft = 1.f - std::expf(-1.f * expScale * normalLeftAbsolute);
			normalRight = 1.f - std::expf(-1.f * expScale * normalRightAbsolute);
		}


		void NormalEstimation::_getNormalWithoutMask(const cv::Point& center, float& normalLeft, float& normalRight)
		{
			normalLeftAbsolute = 0;
			normalRightAbsolute = 0;
			thresh.forEach<uchar>([&](const uchar& value, const int* position)
			{
				if (value != 0)
				{
					if (position[1] < center.x)
					{
						std::lock_guard<std::mutex> guard(mtxLeft);
						++normalLeftAbsolute;
					}
					else
					{
						std::lock_guard<std::mutex> guard(mtxRight);
						++normalRightAbsolute;
					}
				}
			});

			// may be we can put this also to the filter function
			normalLeft = 1.f - std::expf(-1.f * expScale * normalLeftAbsolute);
			normalRight = 1.f - std::expf(-1.f * expScale * normalRightAbsolute);
		}


		void NormalEstimation::_filterNormal(float& normalLeft, float& normalRight)
		{
			normalLeft = filterLeft.apply(normalLeft);
			normalRight = filterRight.apply(normalRight);
		}


		void NormalEstimation::_drawNormalWithMask(cv::Mat& image, const cv::Scalar& color, const cv::Mat& mask)
		{
			image.setTo(color, (mask + thresh - (mask != thresh)));
		}


		void NormalEstimation::_drawNormalWithoutMask(cv::Mat& image, const cv::Scalar& color)
		{
			image.setTo(color, thresh);
		}

	}
}