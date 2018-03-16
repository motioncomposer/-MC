#pragma once

#include "core.h"

#include "structures.h"

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
	namespace activity
	{

		// ==========================================================================================================================
		//
		// activity estimator parameter
		//
		// ==========================================================================================================================


		struct ActivityEstimatorParameter
		{

			float minDifference;

			float normalScaleHead;
			float normalScaleLimbs;
			float normalScaleBody;

			float discreteThreshold;
			float discreteTimeoutMilli;

			// we still have to add here the filter parameter ...


			ActivityEstimatorParameter() : minDifference(20.f),
				normalScaleHead(1.7f),
				normalScaleLimbs(1.7f),
				normalScaleBody(1.7f),
				discreteThreshold(0.f),
				discreteTimeoutMilli(250.f)
			{}


			void read(const cv::FileNode& fn);


			void write(cv::FileStorage& fs) const;
		};


		void read(const cv::FileNode& fn, ActivityEstimatorParameter& activityPara, const ActivityEstimatorParameter& default = ActivityEstimatorParameter());

		void write(cv::FileStorage& fs, const std::string&, const ActivityEstimatorParameter& activityPara);

		bool saveActivityEstimatorParameter(const ActivityEstimatorParameter& activityPara,
			const std::string& filename, const std::string& key);

		bool readActivityEstimatorParameter(ActivityEstimatorParameter& activityPara,
			const std::string& filename, const std::string& key);


		// ==========================================================================================================================
		//
		// helper structures and functions for activity estimator
		//
		// ==========================================================================================================================


		struct Counter
		{
			size_t head;
			size_t upperLeft;
			size_t upperRight;
			size_t lowerLeft;
			size_t lowerRight;

			Counter() : head(0), upperLeft(0), upperRight(0), lowerLeft(0), lowerRight(0)
			{}
		};


		struct Normal
		{
			float head;
			float handLeft;
			float handRight;
			float legLeft;
			float legRight;
			float bodyLeft;
			float bodyRight;
			float bodyUpper;
			float bodyLower;

			Normal() : head(0.f), handLeft(0.f), handRight(0.f), legLeft(0.f), legRight(0.f),
				bodyLeft(0.f), bodyRight(0.f), bodyUpper(0.f), bodyLower(0.f)
			{}
		};


		struct Filter
		{
			fs::core::SimpleBranchedFilterFloat head;
			fs::core::SimpleBranchedFilterFloat handLeft;
			fs::core::SimpleBranchedFilterFloat handRight;
			fs::core::SimpleBranchedFilterFloat legLeft;
			fs::core::SimpleBranchedFilterFloat legRight;
			fs::core::SimpleBranchedFilterFloat bodyLeft;
			fs::core::SimpleBranchedFilterFloat bodyRight;
			fs::core::SimpleBranchedFilterFloat bodyUpper;
			fs::core::SimpleBranchedFilterFloat bodyLower;

			Filter(float last, float alpha) : head(last, alpha),
				handLeft(last, alpha),
				handRight(last, alpha),
				legLeft(last, alpha),
				legRight(last, alpha),
				bodyLeft(last, alpha),
				bodyRight(last, alpha),
				bodyUpper(last, alpha),
				bodyLower(last, alpha)
			{}
		};


		struct State
		{
			bool head;
			bool handLeft;
			bool handRight;
			bool legLeft;
			bool legRight;
			bool bodyLeft;
			bool bodyRight;
			bool bodyUpper;
			bool bodyLower;

			State() : head(false), handLeft(false), handRight(false), legLeft(false), legRight(false),
				bodyLeft(false), bodyRight(false), bodyUpper(false), bodyLower(false)
			{}
		};


		struct TimePoint
		{
			std::chrono::high_resolution_clock::time_point head;
			std::chrono::high_resolution_clock::time_point handLeft;
			std::chrono::high_resolution_clock::time_point handRight;
			std::chrono::high_resolution_clock::time_point legLeft;
			std::chrono::high_resolution_clock::time_point legRight;
			std::chrono::high_resolution_clock::time_point bodyLeft;
			std::chrono::high_resolution_clock::time_point bodyRight;
			std::chrono::high_resolution_clock::time_point bodyUpper;
			std::chrono::high_resolution_clock::time_point bodyLower;

			TimePoint() = default;
		};


		struct Discrete
		{
			bool head;
			bool handLeft;
			bool handRight;
			bool legLeft;
			bool legRight;
			bool bodyLeft;
			bool bodyRight;
			bool bodyUpper;
			bool bodyLower;

			Discrete() : head(false), handLeft(false), handRight(false), legLeft(false), legRight(false),
				bodyLeft(false), bodyRight(false), bodyUpper(false), bodyLower(false)
			{}
		};


		// ==========================================================================================================================
		//
		// kernel object for activity estimator
		//
		// ==========================================================================================================================


		struct ActivityEstimatorKernel
		{

			mc::activity::Counter diffCounter;
			mc::activity::Counter maskCounter;

			mc::activity::Normal normal;

			mc::activity::Discrete discrete;
			mc::activity::State curState;
			mc::activity::State oldState;
			mc::activity::TimePoint timePoint;

			ActivityEstimatorKernel() = default;
		};


		// ==========================================================================================================================
		//
		// activity estimator object
		//
		// ==========================================================================================================================


		class ActivityEstimator
		{
		public:


			// here we have to adjust the filter ...
			// we need also filter values ... INSIDE of the parameter ...
			// may be we put the filter alos inside of the kernel ... so the grouping is better ...
			ActivityEstimator(const ActivityEstimatorParameter& parameter) : parameter(parameter), filter{ Filter(0.f, 0.8f), Filter(0.f, 0.8f) }
			{}

			bool apply(const cv::Mat& image, const cv::Mat& mask, const mc::structures::PlayerActivation activation, const mc::structures::PlayerSelection& selection,
				const mc::structures::SharedData& shared, mc::structures::Result& result);

			bool draw(cv::Mat& show, const cv::Mat& mask, const mc::structures::SharedData& shared, const cv::Scalar& color_one = { 0, 255, 0 }, const cv::Scalar& color_two = { 0, 0, 255 });

			const Normal& getNormalIdx0() const;

			const Normal& getNormalIdx1() const;

			const Discrete& getDiscreteIdx0() const;

			const Discrete& getDiscreteIdx1() const;


		private:

			ActivityEstimatorParameter parameter;
			mc::structures::PlayerSelection trackerID;
			std::array<ActivityEstimatorKernel, 2> kernel;
			std::array<Filter, 2> filter;
		
			cv::Mat last;
			cv::Mat diff;


			void updatePlayerState(const cv::Mat& image, const cv::Mat& mask, const mc::structures::SharedData& shared, uint32_t idx);


			void analyzeFrameDiff(const cv::Mat& mask, const mc::structures::SharedData& shared, uint32_t idx);

			void calculateNormalState(uint32_t idx);

			void calculateDiscreteState(uint32_t idx);


			void calculateActivityResult(mc::structures::Result& result, uint32_t idx);

		};
	}
}