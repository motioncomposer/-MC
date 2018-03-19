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

			int32_t halfWindowSize;

			
			int32_t halfRectSize;


			float headSize;

			// filter parameter for the 

			// and also for schmitt-trigger

			// here we store all the parameter used for the estimator

			// filter parameter for the result scaling at the end

			// kalman filter stuff? --> do we need a kalman filter? because we already have a kalman filter result from the blobfinder ... but here it was used only pixel based
			// we actually need somehow to find the "track" the hands and feet ... (if we want to use something like clap ...)

			// time

			// for heightlvl we need may be a kalman filter ... the size is basically constant ...

			// it is still not clear yet want kind of parameter we need here ...


			PositionEstimatorParameter() : halfWindowSize(5), halfRectSize(21), headSize(220.f)
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
		// helper structures and functions for position estimator
		//
		// ==========================================================================================================================


		// this get re designed ...
		struct BodyPartData
		{
			cv::Point handLeft;
			cv::Point handRight;
			cv::Point footLeft;
			cv::Point footRight;

			BodyPartData() : handLeft(-1, -1), handRight(-1, -1), footLeft(-1, -1), footRight(-1, -1)
			{}
		};


		struct ContourIndices
		{
			int32_t indexTopDivider;

			int32_t indexLeftFoot;
			int32_t indexRightFoot;

			int32_t indexLeftHip;
			int32_t indexRightHip;

			int32_t indexLeftHand;
			int32_t indexRightHand;

			ContourIndices() = default;
		};


		struct ExtremalPoints
		{
			cv::Point mostLeft;
			cv::Point mostRight;
			cv::Point mostUpper;
			cv::Point mostLower;

			ExtremalPoints() = default;
		};


		// this structure is kind of stupid ... we will deprecate it
		struct HandRefinementPoints
		{
			cv::Point handLeft;
			cv::Point handRight;

			HandRefinementPoints() = default;
		};


		struct MinimaPoints
		{
			cv::Point handLeft;
			cv::Point handRight;
			cv::Point footLeft;
			cv::Point footRight;

			float dispHandLeft;
			float dispHandRight;
			float dispFootLeft;
			float dispFootRight;

			MinimaPoints() = default;
		};


		// ==========================================================================================================================
		//
		// kernel object for position estimator
		//
		// ==========================================================================================================================


		// if we make this more complex ... then we should make it a class ... we can move the definitition of the helper structures in side of the kernel ...
		// we keep it as a structure ...
		struct PositionEstimatorKernel
		{


			PositionEstimatorKernel();
		};


		// ==========================================================================================================================
		//
		// position estimator object
		//
		// ==========================================================================================================================


		class PositionEstimator
		{

		public:

			PositionEstimator(const PositionEstimatorParameter& parameter, const fs::core::BasicStereoValues& values) : parameter(parameter), trackerID{ -1, -1 }, values(values),
				metricHeightFilter{ fs::core::SimpleFilterFloat(1200.f, 0.5f), fs::core::SimpleFilterFloat(1200.f, 0.5f) },
				metricCenterFilter{ fs::core::SimpleFilterFloat(600.f, 0.5f), fs::core::SimpleFilterFloat(600.f, 0.5f) }
			{}


			bool apply(const cv::Mat& disp, const mc::structures::PlayerActivation activation, const mc::structures::PlayerSelection& selection, const std::vector<std::vector<cv::Point>>& contours,
				mc::structures::SharedData& shared, mc::structures::Result& result);

			bool draw(cv::Mat& show, const mc::structures::SharedData& shared, const cv::Scalar& color_one = { 0, 255, 0 }, const cv::Scalar& color_two = { 0, 0, 255 });

			const BodyPartData& getBodyPartDataIdx0() const;

			const BodyPartData& getBodyPartDataIdx1() const;

		private:

			fs::core::BasicStereoValues values;
			PositionEstimatorParameter parameter;
			mc::structures::PlayerSelection trackerID;

			// here we should also put in the shoulderLvl and the head level (half way between top point and shoulder point ...) ...

			std::array<float, 2> metricHeight;
			std::array<fs::core::SimpleFilterFloat, 2> metricHeightFilter;

			std::array<float, 2> pixelHeight;
			std::array<uint32_t, 2> topLvl;
			
			std::array<float, 2> metricShoulder;

			std::array<float, 2> pixelShoulder;

			std::array<uint32_t, 2> shoulderLvl;


			std::array<float, 2> metricCenter;
			std::array<fs::core::SimpleFilterFloat, 2> metricCenterFilter;

			std::array<float, 2> pixelCenter;
			std::array<uint32_t, 2> midLvl;


			std::vector<cv::Point> smoothedContour;

			std::array<ContourIndices, 2> contourIndices;
			std::array<ExtremalPoints, 2> extremalPoints;
			std::array<HandRefinementPoints, 2> handRefinementPoints;
			std::array<MinimaPoints, 2> minimaPoints;


			// this this needed? ... no ...
			std::array<BodyPartData, 2> curBodyPartData;
			std::array<BodyPartData, 2> oldBodyPartData;


			// this we have to change ...
			void smoothContour(const std::vector<cv::Point>& contour);

			// we need some kind of tracking structure ...


			void scanForFeet(const mc::structures::SharedData& shared, uint32_t idx);

			void scanForLeftHip(const mc::structures::SharedData& shared, uint32_t idx);

			void scanForRightHip(const mc::structures::SharedData& shared, uint32_t idx);

			void scanForLeftHand(const mc::structures::SharedData& shared, uint32_t idx);

			void scanForRightHand(const mc::structures::SharedData& shared, uint32_t idx);

			void findInitialBodyPartData(const mc::structures::SharedData& shared, uint32_t idx);



			void findExtremalPoints(uint32_t idx);

			void findHandRefinementPoints(const mc::structures::SharedData& shared, uint32_t idx);



			void findMinimaPoints(const cv::Mat& disp, uint32_t idx);

			
			void combineResults(const mc::structures::SharedData& shared, uint32_t idx);


			void calculateState(const cv::Mat& disp, const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared, const std::vector<std::vector<cv::Point>>& contours, uint32_t idx);




			/////

			void learnInitialValues(const mc::structures::SharedData& shared, uint32_t idx);


			// this is maybe obsolete ...
			void calculateProvisionalState(const cv::Mat& disp, const mc::structures::SharedData& shared,
				const std::vector<std::vector<cv::Point>>& contours, uint32_t idx);

			void calculateCompleteState(const cv::Mat& disp, const mc::structures::SharedData& shared,
				const std::vector<std::vector<cv::Point>>& contours, uint32_t idx);

			//



			void updatePlayerState(const cv::Mat& disp, const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared, const std::vector<std::vector<cv::Point>>& contours, uint32_t idx);



			// actually we can directly hand in the image
			// in this function we do the scaling and the writing into the result data structure ...
			void calculatePositionResult(/*const cv::Size& imageSize, const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared,
				mc::structures::Result& result, uint32_t idx*/);


			void writeSharedData(mc::structures::SharedData& shared, uint32_t idx);

		};
	}
}