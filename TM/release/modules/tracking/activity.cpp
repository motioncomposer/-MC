
#include "activity.h"


namespace mc
{
	namespace activity
	{

		// ==========================================================================================================================
		//
		// activity estimator parameter
		//
		// ==========================================================================================================================


		void ActivityEstimatorParameter::read(const cv::FileNode& fn)
		{
			fn["minDifference"] >> minDifference;
			fn["normalScaleHead"] >> normalScaleHead;
			fn["normalScaleLimbs"] >> normalScaleLimbs;
			fn["normalScaleBody"] >> normalScaleBody;
			fn["discreteThreshold"] >> discreteThreshold;
			fn["discreteTimeoutMilli"] >> discreteTimeoutMilli;
		}


		void ActivityEstimatorParameter::write(cv::FileStorage& fs) const
		{
			fs << "{"
				<< "minDifference" << minDifference
				<< "normalScaleHead" << normalScaleHead
				<< "normalScaleLimbs" << normalScaleLimbs
				<< "normalScaleBody" << normalScaleBody
				<< "discreteThreshold" << discreteThreshold
				<< "discreteTimeoutMilli" << discreteTimeoutMilli
				<< "}";
		}


		void read(const cv::FileNode& fn, ActivityEstimatorParameter& activityPara, const ActivityEstimatorParameter& default)
		{
			if (fn.empty())
				activityPara = default;
			else
				activityPara.read(fn);
		}


		void write(cv::FileStorage& fs, const std::string&, const ActivityEstimatorParameter& activityPara)
		{
			activityPara.write(fs);
		}


		bool saveActivityEstimatorParameter(const ActivityEstimatorParameter& activityPara,
			const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::WRITE);

			if (fs.isOpened())
			{
				fs << key << activityPara;
				return true;
			}

			return false;
		}


		bool readActivityEstimatorParameter(ActivityEstimatorParameter& activityPara,
			const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::READ);

			if (fs.isOpened())
			{
				fs[key] >> activityPara;
				return true;
			}

			return false;
		}


		// ==========================================================================================================================
		//
		// activity estimator object
		//
		// ==========================================================================================================================


		bool ActivityEstimator::apply(const cv::Mat& image, const cv::Mat& mask, const mc::structures::PlayerActivation activation, const mc::structures::PlayerSelection& selection,
			const mc::structures::SharedData& shared, mc::structures::Result& result)
		{
			if (image.empty() || image.type() != CV_8UC1 || (!last.empty() && image.size() != last.size()))
				return false;


			if (selection[0] == trackerID[1] || selection[1] == trackerID[0])
			{
				std::swap(trackerID[0], trackerID[1]);
				std::swap(kernel[0], kernel[1]);
			}


			if (selection[0] >= 0 && trackerID[0] == selection[0])
			{
				updatePlayerState(image, mask, shared, 0);
				if (activation[0])
					calculateActivityResult(result, 0);
				else
					result.player[0].activity = mc::structures::ActivityResult();
			}
			else
			{
				kernel[0] = ActivityEstimatorKernel();
				result.player[0].activity = mc::structures::ActivityResult();
			}

			trackerID[0] = selection[0];



			if (selection[1] >= 0 && trackerID[1] == selection[1])
			{
				updatePlayerState(image, mask, shared, 1);
				if (activation[1])
					calculateActivityResult(result, 1);
				else
					result.player[1].activity = mc::structures::ActivityResult();
			}
			else
			{
				kernel[1] = ActivityEstimatorKernel();
				result.player[1].activity = mc::structures::ActivityResult();
			}

			trackerID[1] = selection[1];

			return true;
		}


		///////////////////////////////

		// do we really need a draw function ... ?
		bool ActivityEstimator::draw(cv::Mat& show, const cv::Mat& mask, const mc::structures::SharedData& shared, const cv::Scalar& color_one, const cv::Scalar& color_two)
		{
			if (show.empty() || show.type() != CV_8UC3)
				return false;

			// here we have again when we need need to draw a result
			const cv::Rect overallPlayerOne(shared.trackerData[0].tl, shared.trackerData[0].br);
			const cv::Rect overallPlayerTwo(shared.trackerData[0].tl, shared.trackerData[0].br);

			// here we may be have the problem when we internally use a bigger image size (no problem at the moment ...)

			show.setTo(color_one, (mask + (diff > parameter.minDifference) - (mask != (diff > parameter.minDifference))));

			return true;
		}


		/////////////////////////////


		const Normal& ActivityEstimator::getNormalIdx0() const
		{
			return kernel[0].normal;
		}


		const Normal& ActivityEstimator::getNormalIdx1() const
		{
			return kernel[1].normal;
		}


		const Discrete& ActivityEstimator::getDiscreteIdx0() const
		{
			return kernel[0].discrete;
		}


		const Discrete& ActivityEstimator::getDiscreteIdx1() const
		{
			return kernel[1].discrete;
		}


		void ActivityEstimator::updatePlayerState(const cv::Mat& image, const cv::Mat& mask, const mc::structures::SharedData& shared, uint32_t idx)
		{
			if (!last.empty())
			{
				cv::absdiff(last, image, diff);

				analyzeFrameDiff(mask, shared, idx);

				calculateNormalState(idx);
				calculateDiscreteState(idx);
			}

			image.copyTo(last);
		}


		void ActivityEstimator::analyzeFrameDiff(const cv::Mat& mask, const mc::structures::SharedData& shared, uint32_t idx)
		{
			kernel[idx].diffCounter = Counter();
			kernel[idx].maskCounter = Counter();

			// here we also have to check if the roi is completely within the image ... (?)

			const cv::Rect head/*(here we use some stuff from the postion data)*/;
			const cv::Rect overall(shared.trackerData[idx].tl, shared.trackerData[idx].br);

			for (auto c = overall.tl().y; c < overall.br().y; ++c)
			{
				const uchar* diff_ptr = diff.ptr<uchar>(c);
				const uchar* mask_ptr = mask.ptr<uchar>(c);

				for (auto d = overall.tl().x; d < overall.br().x; ++d)
				{
					/*
					// checking for the head
					if (head.contains({ d, c }))
					{
					if (mask_ptr[d] != 0)
					{
					if (diff_ptr[d] > parameter.minDifference)
					++diffCounter[idx].head;

					++maskCounter[idx].head;
					}
					}
					// we can may be connect this with else if
					*/

					if (d < shared.trackerData[idx].cp.x && c < shared.trackerData[idx].cp.y)
					{
						if (mask_ptr[d] != 0)
						{
							if (diff_ptr[d] > parameter.minDifference)
								++kernel[idx].diffCounter.upperLeft;

							++kernel[idx].maskCounter.upperLeft;
						}

					}
					else if (d >= shared.trackerData[idx].cp.x && c < shared.trackerData[idx].cp.y)
					{
						if (mask_ptr[d] != 0)
						{
							if (diff_ptr[d] > parameter.minDifference)
								++kernel[idx].diffCounter.upperRight;

							++kernel[idx].maskCounter.upperRight;
						}
					}
					else if (d < shared.trackerData[idx].cp.x && c >= shared.trackerData[idx].cp.y)
					{
						if (mask_ptr[d] != 0)
						{
							if (diff_ptr[d] > parameter.minDifference)
								++kernel[idx].diffCounter.lowerLeft;

							++kernel[idx].maskCounter.lowerLeft;
						}
					}
					else
					{
						if (mask_ptr[d] != 0)
						{
							if (diff_ptr[d] > parameter.minDifference)
								++kernel[idx].diffCounter.lowerRight;

							++kernel[idx].maskCounter.lowerRight;
						}
					}
				}
			}
		}


		void ActivityEstimator::calculateNormalState(uint32_t idx)
		{
			kernel[idx].normal.head = filter[idx].head.apply(parameter.normalScaleHead * static_cast<float>(kernel[idx].diffCounter.head) / static_cast<float>(kernel[idx].maskCounter.head));

			kernel[idx].normal.handLeft = filter[idx].handLeft.apply(parameter.normalScaleLimbs * static_cast<float>(kernel[idx].diffCounter.upperLeft) / static_cast<float>(kernel[idx].maskCounter.upperLeft));
			kernel[idx].normal.handRight = filter[idx].handRight.apply(parameter.normalScaleLimbs * static_cast<float>(kernel[idx].diffCounter.upperRight) / static_cast<float>(kernel[idx].maskCounter.upperRight));
			kernel[idx].normal.legLeft = filter[idx].legLeft.apply(parameter.normalScaleLimbs * static_cast<float>(kernel[idx].diffCounter.lowerLeft) / static_cast<float>(kernel[idx].maskCounter.lowerLeft));
			kernel[idx].normal.legRight = filter[idx].legRight.apply(parameter.normalScaleLimbs * static_cast<float>(kernel[idx].diffCounter.lowerRight) / static_cast<float>(kernel[idx].maskCounter.lowerRight));

			kernel[idx].normal.bodyLeft = filter[idx].bodyLeft.apply(parameter.normalScaleBody * std::sqrt(static_cast<float>(kernel[idx].diffCounter.upperLeft + kernel[idx].diffCounter.lowerLeft)) / std::sqrt(static_cast<float>(kernel[idx].maskCounter.upperLeft + kernel[idx].maskCounter.lowerLeft)));
			kernel[idx].normal.bodyRight = filter[idx].bodyRight.apply(parameter.normalScaleBody * std::sqrt(static_cast<float>(kernel[idx].diffCounter.upperRight + kernel[idx].diffCounter.lowerRight)) / std::sqrt(static_cast<float>(kernel[idx].maskCounter.upperRight + kernel[idx].maskCounter.lowerRight)));
			kernel[idx].normal.bodyUpper = filter[idx].bodyUpper.apply(parameter.normalScaleBody * std::sqrt(static_cast<float>(kernel[idx].diffCounter.upperLeft + kernel[idx].diffCounter.upperRight)) / std::sqrt(static_cast<float>(kernel[idx].maskCounter.upperLeft + kernel[idx].maskCounter.upperRight)));
			kernel[idx].normal.bodyLower = filter[idx].bodyLower.apply(parameter.normalScaleBody * std::sqrt(static_cast<float>(kernel[idx].diffCounter.lowerLeft + kernel[idx].diffCounter.lowerRight)) / std::sqrt(static_cast<float>(kernel[idx].maskCounter.lowerLeft + kernel[idx].maskCounter.lowerRight)));
		}


		void ActivityEstimator::calculateDiscreteState(uint32_t idx)
		{

			kernel[idx].curState.head = kernel[idx].normal.head > parameter.discreteThreshold;

			kernel[idx].curState.handLeft = kernel[idx].normal.handLeft > parameter.discreteThreshold;
			kernel[idx].curState.handRight = kernel[idx].normal.handRight > parameter.discreteThreshold;
			kernel[idx].curState.legLeft = kernel[idx].normal.legLeft > parameter.discreteThreshold;
			kernel[idx].curState.legRight = kernel[idx].normal.legRight > parameter.discreteThreshold;

			kernel[idx].curState.bodyLeft = kernel[idx].normal.bodyLeft > parameter.discreteThreshold;
			kernel[idx].curState.bodyRight = kernel[idx].normal.bodyRight > parameter.discreteThreshold;
			kernel[idx].curState.bodyUpper = kernel[idx].normal.bodyUpper > parameter.discreteThreshold;
			kernel[idx].curState.bodyLower = kernel[idx].normal.bodyLower > parameter.discreteThreshold;


			kernel[idx].discrete.head = (kernel[idx].curState.head && !kernel[idx].oldState.head && ((std::chrono::high_resolution_clock::now() - kernel[idx].timePoint.head) > std::chrono::duration<float, std::milli>(parameter.discreteTimeoutMilli)));

			kernel[idx].discrete.handLeft = (kernel[idx].curState.handLeft && !kernel[idx].oldState.handLeft && ((std::chrono::high_resolution_clock::now() - kernel[idx].timePoint.handLeft) > std::chrono::duration<float, std::milli>(parameter.discreteTimeoutMilli)));
			kernel[idx].discrete.handRight = (kernel[idx].curState.handRight && !kernel[idx].oldState.handRight && ((std::chrono::high_resolution_clock::now() - kernel[idx].timePoint.handRight) > std::chrono::duration<float, std::milli>(parameter.discreteTimeoutMilli)));
			kernel[idx].discrete.legLeft = (kernel[idx].curState.legLeft && !kernel[idx].oldState.legLeft && ((std::chrono::high_resolution_clock::now() - kernel[idx].timePoint.legLeft) > std::chrono::duration<float, std::milli>(parameter.discreteTimeoutMilli)));
			kernel[idx].discrete.legRight = (kernel[idx].curState.legRight && !kernel[idx].oldState.legRight && ((std::chrono::high_resolution_clock::now() - kernel[idx].timePoint.legRight) > std::chrono::duration<float, std::milli>(parameter.discreteTimeoutMilli)));

			kernel[idx].discrete.bodyLeft = (kernel[idx].curState.bodyLeft && !kernel[idx].oldState.bodyLeft && ((std::chrono::high_resolution_clock::now() - kernel[idx].timePoint.bodyLeft) > std::chrono::duration<float, std::milli>(parameter.discreteTimeoutMilli)));
			kernel[idx].discrete.bodyRight = (kernel[idx].curState.bodyRight && !kernel[idx].oldState.bodyRight && ((std::chrono::high_resolution_clock::now() - kernel[idx].timePoint.bodyRight) > std::chrono::duration<float, std::milli>(parameter.discreteTimeoutMilli)));
			kernel[idx].discrete.bodyUpper = (kernel[idx].curState.bodyUpper && !kernel[idx].oldState.bodyUpper && ((std::chrono::high_resolution_clock::now() - kernel[idx].timePoint.bodyUpper) > std::chrono::duration<float, std::milli>(parameter.discreteTimeoutMilli)));
			kernel[idx].discrete.bodyLower = (kernel[idx].curState.bodyLower && !kernel[idx].oldState.bodyLower && ((std::chrono::high_resolution_clock::now() - kernel[idx].timePoint.bodyLower) > std::chrono::duration<float, std::milli>(parameter.discreteTimeoutMilli)));


			if (kernel[idx].curState.head) kernel[idx].timePoint.head = std::chrono::high_resolution_clock::now();

			if (kernel[idx].curState.handLeft) kernel[idx].timePoint.handLeft = std::chrono::high_resolution_clock::now();
			if (kernel[idx].curState.handRight) kernel[idx].timePoint.handRight = std::chrono::high_resolution_clock::now();
			if (kernel[idx].curState.legLeft) kernel[idx].timePoint.legLeft = std::chrono::high_resolution_clock::now();
			if (kernel[idx].curState.legRight) kernel[idx].timePoint.legRight = std::chrono::high_resolution_clock::now();

			if (kernel[idx].curState.bodyLeft) kernel[idx].timePoint.bodyLeft = std::chrono::high_resolution_clock::now();
			if (kernel[idx].curState.bodyRight) kernel[idx].timePoint.bodyRight = std::chrono::high_resolution_clock::now();
			if (kernel[idx].curState.bodyUpper) kernel[idx].timePoint.bodyUpper = std::chrono::high_resolution_clock::now();
			if (kernel[idx].curState.bodyLower) kernel[idx].timePoint.bodyLower = std::chrono::high_resolution_clock::now();


			kernel[idx].oldState.head = kernel[idx].curState.head;

			kernel[idx].oldState.handLeft = kernel[idx].curState.handLeft;
			kernel[idx].oldState.handRight = kernel[idx].curState.handRight;
			kernel[idx].oldState.legLeft = kernel[idx].curState.legLeft;
			kernel[idx].oldState.legRight = kernel[idx].curState.legRight;

			kernel[idx].oldState.bodyLeft = kernel[idx].curState.bodyLeft;
			kernel[idx].oldState.bodyRight = kernel[idx].curState.bodyRight;
			kernel[idx].oldState.bodyUpper = kernel[idx].curState.bodyUpper;
			kernel[idx].oldState.bodyLower = kernel[idx].curState.bodyLower;

		}


		void ActivityEstimator::calculateActivityResult(mc::structures::Result& result, uint32_t idx)
		{
			result.player[idx].activity.normal.head = kernel[idx].normal.head;

			result.player[idx].activity.normal.handLeft = kernel[idx].normal.handLeft;
			result.player[idx].activity.normal.handRight = kernel[idx].normal.handRight;
			result.player[idx].activity.normal.legLeft = kernel[idx].normal.legLeft;
			result.player[idx].activity.normal.legRight = kernel[idx].normal.legRight;

			result.player[idx].activity.normal.bodyLeft = kernel[idx].normal.bodyLeft;
			result.player[idx].activity.normal.bodyRight = kernel[idx].normal.bodyRight;
			result.player[idx].activity.normal.bodyUpper = kernel[idx].normal.bodyUpper;
			result.player[idx].activity.normal.bodyLower = kernel[idx].normal.bodyLower;



			result.player[idx].activity.discrete.head = kernel[idx].discrete.head;

			result.player[idx].activity.discrete.handLeft = kernel[idx].discrete.handLeft;
			result.player[idx].activity.discrete.handRight = kernel[idx].discrete.handRight;
			result.player[idx].activity.discrete.legLeft = kernel[idx].discrete.legLeft;
			result.player[idx].activity.discrete.legRight = kernel[idx].discrete.legRight;

			result.player[idx].activity.discrete.bodyLeft = kernel[idx].discrete.bodyLeft;
			result.player[idx].activity.discrete.bodyRight = kernel[idx].discrete.bodyRight;
			result.player[idx].activity.discrete.bodyUpper = kernel[idx].discrete.bodyUpper;
			result.player[idx].activity.discrete.bodyLower = kernel[idx].discrete.bodyLower;

		}
	}
}