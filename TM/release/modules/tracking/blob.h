#pragma once

#include "core.h"

#include "structures.h"

#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"

#include <vector>
#include <numeric>


#define MC_AUXILIARY_FUNCTIONS


namespace mc
{

	namespace blob
	{

		// ==========================================================================================================================
		//
		// structure to store disparity thresholds
		//
		// ==========================================================================================================================


		struct DisparityThresholds
		{
			short minDisparity;
			short maxDisparity;
		};


		// ==========================================================================================================================
		//
		// blob finder parameter
		//
		// ==========================================================================================================================
		

		struct BlobFinderParameter
		{

			int floorLvl;
			int sideOffset;
			int erosionSizeX;
			int erosionSizeY;

			float minDistance;
			float maxDistance;			

			// this is maybe not needed anymore
			// at least should be simplified
			float maxDistanceOffsetBack;
			float maxDistanceOffsetFront;

			float minTrackSize;
			float maxSpeckleSize;

			float maxPointError;
			float maxHandDistance;

			// here we need a re-init threshold
			int maxNoMeasurement;
			int maxTrackerCount;

			// this should also be more flexible
			float varianceQ;
			float varianceR;

			BlobFinderParameter() : floorLvl(0),
				sideOffset(30),
				erosionSizeX(7),
				erosionSizeY(7),
				minDistance(0.f),
				maxDistance(10000.f),
				maxDistanceOffsetBack(500.f),
				maxDistanceOffsetFront(800.f),
				minTrackSize(10000.f),
				maxSpeckleSize(150.f),
				maxPointError(100.f),
				maxHandDistance(900.f),
				maxNoMeasurement(5),
				maxTrackerCount(32),
				varianceQ(1.f),
				varianceR(1e-1f) {}

			void read(const cv::FileNode& fn);

			void write(cv::FileStorage& fs) const;

		};


		void read(const cv::FileNode& fn, BlobFinderParameter& blobFinderPara, const BlobFinderParameter& default = BlobFinderParameter());

		void write(cv::FileStorage& fs, const std::string&, const BlobFinderParameter& blobFinderPara);

		bool saveBlobFinderParameter(const BlobFinderParameter& blobFinderPara,
			const std::string& filename, const std::string& key);

		bool readBlobFinderParameter(BlobFinderParameter& blobFinderPara,
			const std::string& filename, const std::string& key);


		// ==========================================================================================================================
		//
		// struct stores some auxiliary values for a blob
		//
		// ==========================================================================================================================


		struct BlobResult
		{
			cv::Point2f tl;
			cv::Point2f br;
			cv::Point2f cp;

			float area;
			float distance;
		};


		cv::Vec3f metricBlobResultTL(const fs::core::BasicStereoValues& values, const BlobResult& result);

		cv::Vec3f metricBlobResultBR(const fs::core::BasicStereoValues& values, const BlobResult& result);

		cv::Vec3f metricBlobResultCP(const fs::core::BasicStereoValues& values, const BlobResult& result);

		cv::Size2f metricBlobResultSize(const fs::core::BasicStereoValues& values, const BlobResult& result);

		float metricDistance(const fs::core::BasicStereoValues& values, const BlobResult& result0, const BlobResult& result1);

		// returns the distance projected on the XY plane
		float metricDistanceXY(const fs::core::BasicStereoValues& values, const BlobResult& result0, const BlobResult& result1);

		// returns the distance projected on the YZ plane
		float metricDistanceYZ(const fs::core::BasicStereoValues& values, const BlobResult& result0, const BlobResult& result1);

		// returns the distance projected on the XZ plane
		float metricDistanceXZ(const fs::core::BasicStereoValues& values, const BlobResult& result0, const BlobResult& result1);


		// ==========================================================================================================================
		//
		// blob tracker object
		//
		// ==========================================================================================================================


		/*
		*
		* WHAT THE STATE-VECTOR LOOKS LIKE:
		*
		*  [0] : position x top left corner
		*  [1] : position y top left corner
		*  [2] : position x bottom right corner
		*  [3] : position y bottom right corner
		*  [4] : position x cendroid
		*  [5] : position y centroid
		*  [6] : position z
		*  [7] : velocity x top left corner
		*  [8] : velocity y top left corner
		*  [9] : velocity x bottom right corner
		* [10] : velocity y bottom right corner
		* [11] : velocity x centroid
		* [12] : velocity y centroid
		* [13] : velocity z
		* [14] : acceleration x top left corner
		* [15] : acceleration y top left corner
		* [16] : acceleration x bottom right corner
		* [17] : acceleration y bottom right corner
		* [18] : acceleration x centroid
		* [19] : acceleration y centroid
		* [20] : acceleration z
		*
		* WHAT THE MEASUREMENT-VECTOR LOOKS LIKE:
		*
		*  [0] : position x top left corner
		*  [1] : position y top left corner
		*  [2] : position x bottom right corner
		*  [3] : position y bottom right corner
		*  [4] : position z
		*  [5] : position x centroid
		*  [6] : position y centroid
		*
		*/

		class BlobTracker
		{
		public:

			BlobTracker(const BlobResult& result, int index, float varianceQ = 1e-1f, float varianceR = 1.f, float varianceP = 0.1f) : measuredResult(result),
				kalman(21, 7, 0), init(true), meas(true)
			{

				kalman.A << 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.50000, 0, 0, 0, 0, 0, 0,
					0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.50000, 0, 0, 0, 0, 0,
					0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.50000, 0, 0, 0, 0,
					0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.50000, 0, 0, 0,
					0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.50000, 0, 0,
					0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.50000, 0,
					0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.50000,
					0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1;


				kalman.H.setIdentity();
				kalman.Q.setIdentity() *= varianceQ;
				kalman.R.setIdentity() *= varianceR;

				kalman.P.setIdentity() *= varianceP;

				associatedBlobs.push_back(index);

				blobTrackerID = instanceCounter++;
			}



			float match(const BlobResult& result, const BlobFinderParameter& parameter, const fs::core::BasicStereoValues& values) const;

			void collect(const BlobResult& result, int index);

			void predict();

			void correct();


			const BlobResult& getPredictedResult() const;
			
			const BlobResult& getMeasuredResult() const;

			const BlobResult& getCorrectedResult() const;

			cv::Rect getPredictedROI() const;

			cv::Rect getMeasuredROI() const;

			cv::Rect getCorrectedROI() const;

			const std::vector<uint32_t>& getAssociatedBlobs() const;

			bool hasMeasurement() const;

			int getBlobTrackerID() const;
			
			int getNoMeasurementCounter() const;

			void setVarianceQ(float varianceQ);

			void setVarianceR(float varianceR);


		private:

			static uint32_t instanceCounter;

			bool init;
			bool meas;

			std::vector<uint32_t> associatedBlobs;

			int blobTrackerID;
			int noMeasurementCounter;

			BlobResult predictedResult;
			BlobResult measuredResult;

			BlobResult correctedResult;

			fs::core::KalmanFilterFloat kalman;
		};


		// ==========================================================================================================================
		//
		// blob finder object
		//
		// ==========================================================================================================================

		
		class BlobFinder
		{
		public:

			// public interface:

			BlobFinder(const BlobFinderParameter& parameter, const fs::core::BasicStereoValues& values) : parameter(parameter), values(values)
			{

				if (BlobFinder::parameter.minDistance > BlobFinder::parameter.maxDistance)
					std::swap(BlobFinder::parameter.minDistance, BlobFinder::parameter.maxDistance);

				thresholds.minDisparity = static_cast<short>(16 * BlobFinder::values.focalLength * BlobFinder::values.baseLineLength / BlobFinder::parameter.maxDistance);
				thresholds.maxDisparity = 16 * BlobFinder::values.focalLength * BlobFinder::values.baseLineLength / BlobFinder::parameter.minDistance;

			}

			bool apply(const cv::Mat& disp);

			bool draw(cv::Mat& image, const cv::Scalar& color_corrected = cv::Scalar(255, 255, 255), const cv::Scalar& color_measured = cv::Scalar::all(-1),
				const cv::Scalar& color_predicted = cv::Scalar::all(-1)) const;

			void showBlobMask(cv::Mat& show) const;

			void showTrackerMask(cv::Mat& show) const;

			void plotTrackerID(cv::Mat& show) const;

			bool printBlobs(cv::Mat& show, const std::vector<cv::Scalar>& colors) const;

			bool printTrackers(cv::Mat& show, const std::vector<cv::Scalar>& colors) const;


			const BlobFinderParameter& getParameter() const;

			const DisparityThresholds& getThresholds() const;

			size_t getBlobCount() const;

			size_t getTrackerCount() const;

			void adjustDistanceRange(float minDistance, float maxDistance);

			void adjustFloorLevel(int floorLvl);



			////

			bool draw(cv::Mat& image, const mc::structures::PlayerSelection& selection,
				const cv::Scalar& color_one = { 0, 0, 255 }, const cv::Scalar& color_two = { 0, 255, 0 }, const cv::Scalar& color_inactive = { 156, 156, 156 }) const;

			bool printPlayers(cv::Mat& show, const mc::structures::PlayerSelection& selection,
				const cv::Scalar& color_one = { 0, 0, 255 }, const cv::Scalar& color_two = { 0, 255, 0 }, const cv::Scalar& color_inactive = { 156, 156, 156 }) const;

			void updateSharedData(mc::structures::PlayerSelection& selection, mc::structures::SharedData& data) const;

			////



			// public auxiliary functions:

#ifdef MC_AUXILIARY_FUNCTIONS

			const cv::Mat& getMask() const;

			const std::vector<int>& getOrdering() const;

			const std::vector<std::vector<cv::Point>>& getContours() const;

			const std::vector<cv::Vec4i>& getHierarchy() const;

			const std::vector<cv::Rect>& getRois() const;

			const BlobResult& getBlobResult() const;

			const std::vector<BlobTracker>& getTrackers() const;

#endif

		private:

			cv::Mat mask;

			std::vector<int> ordering;
			std::vector<std::vector<cv::Point>> contours;
			std::vector<cv::Vec4i> hierarchy;
			std::vector<cv::Rect> rois;

			BlobResult result;

			std::vector<BlobTracker> trackers;
			std::vector<BlobTracker> shadow;

			fs::core::BasicStereoValues values;
			BlobFinderParameter parameter;
			DisparityThresholds thresholds;

			// internal functions:

			void findBlobs(const cv::Mat& disp);

			void sortBlobs();

			void cleanUpTrackers();

			void predictTrackers();

			void updateResult(const cv::Mat& disp, const fs::core::BasicStereoValues& values, int index);

			void correctTrackers();

		};


	}
}
