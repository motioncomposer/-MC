#pragma once

#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp"

#include "core.h"

#include "structures.h"

namespace mc
{
	namespace position
	{

		// ==========================================================================================================================
		//
		// position estimator parameter
		//
		// ==========================================================================================================================

		struct PositionEstimatorParameter
		{
			// filter parameter for the 

			// and also for schmitt-trigger

			// here we store all the parameter used for the estimator

			// filter parameter for the result scaling at the end

			// kalman filter stuff? --> do we need a kalman filter? because we already have a kalman filter result from the blobfinder ... but here it was used only pixel based
			// we actually need somehow to find the "track" the hands and feet ... (if we want to use something like clap ...)

			// time

			// for heightlvl we need may be a kalman filter ... the size is basically constant ...


			PositionEstimatorParameter()
			{}


			void read(const cv::FileNode& fn);


			void write(cv::FileStorage& fs) const;
		};


		void read(const cv::FileNode& fn, PositionEstimatorParameter& positionPara, const PositionEstimatorParameter& default = PositionEstimatorParameter());

		void write(cv::FileStorage& fs, const std::string&, const PositionEstimatorParameter& positionPara);

		bool savePositionEstimatorParameter(const PositionEstimatorParameter& positionPara,
			const std::string& filename, const std::string& key);

		bool readPositionEstimatorParameter(PositionEstimatorParameter& positionPara,
			const std::string& filename, const std::string& key);


		// ==========================================================================================================================
		//
		// position estimator object
		//
		// ==========================================================================================================================


		class PositionEstimator
		{

		public:

			PositionEstimator(const PositionEstimatorParameter& parameter, const fs::core::BasicStereoValues& values) : parameter(parameter), values(values),
				metricHeightFilter{ fs::core::SimpleFilterFloat(1200.f, 0.5f), fs::core::SimpleFilterFloat(1200.f, 0.5f) },
				metricCenterFilter{ fs::core::SimpleFilterFloat(600.f, 0.5f), fs::core::SimpleFilterFloat(600.f, 0.5f) }
			{}


			bool apply(const cv::Mat& disp, const mc::structures::PlayerActivation activation, const mc::structures::PlayerSelection& selection, const std::vector<std::vector<cv::Point>>& contours,
				mc::structures::SharedData& shared, mc::structures::Result& result);

			bool draw(cv::Mat& show, const mc::structures::SharedData& shared, const cv::Scalar& color_one = { 0, 255, 0 }, const cv::Scalar& color_two = { 0, 0, 255 });


		private:

			fs::core::BasicStereoValues values;
			PositionEstimatorParameter parameter;
			mc::structures::PlayerSelection trackerID;


			std::array<float, 2> metricHeight;
			std::array<fs::core::SimpleFilterFloat, 2> metricHeightFilter;

			std::array<float, 2> pixelHeight;
			std::array<float, 2> topLvl;
			
			std::array<float, 2> metricCenter;
			std::array<fs::core::SimpleFilterFloat, 2> metricCenterFilter;

			std::array<float, 2> pixelCenter;
			std::array<float, 2> midLvl;


			void calculatePositionResult(const cv::Mat& disp, const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared,
				const std::vector<std::vector<cv::Point>>& contours, mc::structures::Result& result, uint32_t idx);

			// may be we should make the shared here mutable in order to directly write the result into it ... --> no!
			void learnInitialValues(const mc::structures::SharedData& shared, uint32_t idx);

			void calculateBasicResult(const cv::Mat& disp, const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared,
				const std::vector<std::vector<cv::Point>>& contours, mc::structures::Result& result, uint32_t idx);

			void calculateFullResult(const cv::Mat& disp, const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared,
				const std::vector<std::vector<cv::Point>>& contours, mc::structures::Result& result, uint32_t idx);

		};
	}
}