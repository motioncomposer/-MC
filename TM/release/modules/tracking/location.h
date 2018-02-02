#pragma once

#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp"

#include "core.h"

#include "structures.h"

namespace mc
{

	namespace location
	{
		// ==========================================================================================================================
		//
		// location estimator parameter
		//
		// ==========================================================================================================================


		struct LocationEstimatorParameter
		{

			float offsetFront;
			float offsetBack;

			float moveThreshold;
			float readyTimeMilli;


			LocationEstimatorParameter() : offsetFront(300.f),
				offsetBack(300.f),
				moveThreshold(300.f),
				readyTimeMilli(1500.f)
			{}


			void read(const cv::FileNode& fn);

			void write(cv::FileStorage& fs) const;

		};


		void read(const cv::FileNode& fn, LocationEstimatorParameter& locationPara, const LocationEstimatorParameter& default = LocationEstimatorParameter());

		void write(cv::FileStorage& fs, const std::string&, const LocationEstimatorParameter& locationPara);

		bool saveLocationEstimatorParameter(const LocationEstimatorParameter& locationPara,
			const std::string& filename, const std::string& key);

		bool readLocationEstimatorParameter(LocationEstimatorParameter& locationPara,
			const std::string& filename, const std::string& key);


		// ==========================================================================================================================
		//
		// position estimator object
		//
		// ==========================================================================================================================


		class LocationEstimator
		{
		public:

			LocationEstimator(const LocationEstimatorParameter& parameter) : parameter(parameter), trackerID{ -1, -1 }, lastCentroidX{ 0.f, 0.f }
			{}

			void apply(const cv::Size& imageSize, const mc::structures::PlayerActivation activation, const mc::structures::PlayerSelection& selection,
				mc::structures::SharedData& shared, mc::structures::Result& result);

			bool draw(cv::Mat& show, const cv::Scalar& color_present = { 0, 0, 255 }, const cv::Scalar& color_ready = { 0, 255, 0 });


		private:

			LocationEstimatorParameter parameter;
			mc::structures::PlayerSelection trackerID;


			std::array<bool, 2> present;
			std::array<bool, 2> ready;
			std::array<bool, 2> outOfRange;

			std::array<float, 2> lastCentroidX;
			std::array<std::chrono::high_resolution_clock::time_point, 2> lastMove;


			void updatePlayerState(const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared, uint32_t idx);

			void calculateLocationResult(const cv::Size& imageSize, const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared,
				mc::structures::Result& result, uint32_t idx);

		};
	}

	




}
