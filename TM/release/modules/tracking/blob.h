#pragma once

#include "core.h"
#include "misc.h"

#include "structures.h"

#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"

#include <vector>
#include <numeric>


namespace mc
{
	namespace blob
	{

		struct BlobMaster
		{
			int64_t size;
			int64_t index;
			short blobDisp;

			cv::Rect searchRegion;
			cv::Rect operatingRoi;
		};


		struct BlobFinderParameter
		{
			int floorLvl;
			int sideOffset;
			int erosionSizeX;
			int erosionSizeY;

			short minDisparity;
			short maxDisparity;
			short maxDispOffsetBack;
			short maxDispOffsetFront;

			int64_t minMasterSize;
			int64_t minSlaveSize;

			BlobFinderParameter() : floorLvl(0), sideOffset(30), erosionSizeX(7), erosionSizeY(7), minDisparity(0), maxDisparity(10000),
				maxDispOffsetBack(72), maxDispOffsetFront(72), minMasterSize(10000), minSlaveSize(150) {}
		};


		class BlobFinder
		{
		public:

			BlobFinder(const BlobFinderParameter& parameter = BlobFinderParameter()) : parameter(parameter) {}

			bool apply(const cv::Mat& disp);

			bool draw(cv::Mat& image, const cv::Scalar& color_one = cv::Scalar(0, 0, 255), const cv::Scalar& color_two = cv::Scalar(255, 0, 0));

			const BlobFinderParameter& getParameter() const;

			BlobFinderParameter& getParameter();

			const cv::Mat& getMask() const;

		private:

			cv::Mat mask;

			std::vector<std::vector<cv::Point>> contours;
			std::vector<cv::Vec4i> hierarchy;
			std::vector<cv::Rect> rois;

			bool one_detected;
			bool two_detected;

			BlobFinderParameter parameter;
			BlobMaster master[2];

			short dmyDisp;
			short dmyDistanceX[2];

			bool candidate_for_blob[2];
			cv::Scalar blob_color[2];

		};
	}
}


// this #define should be removed
#define MC_AUXILIARY_FUNCTIONS
#define MC_PUBLIC_INTERNALS

#define MC_COMPILE_WITH_NEW_DRAFT
#ifdef MC_COMPILE_WITH_NEW_DRAFT

namespace mc_test
{

	/*
	* the current plan:
	*
	* - we distict between some person tracking (where we use a special tracking sheme); for blobs, we make it more simple
	* - tracking the main blob (a.k.a. body blob), and the resulting blob (which is the puzzled-togehter version)
	* - improving the blob matching by removing the hand search region (which is bad designed because of wrong scaling)
	* - we should chec the influence of a histogram equalization
	* - if person is close to camera the main blob may is separatet into multiple main blobs that are abouve each other
	* - they need to be merged
	* - we have position result but we need also something like velocity result and acceleration result
	* - centerX as mean value of the contour --> but this is not part of the mc::blob::BlobFinder
	* - discrimination between blob tracking (blob stuff) and player tracking (special tracking scheme needed to obtain location and position results)
	* - we need a special handtracker bundled to the person tracking; may be we can re-evaluate the hill climber approach from the ZED camera
	* - we are measuring the height from the bottum so the center line should be nearly constant
	*/


	namespace blob
	{


		// ==========================================================================================================================
		//
		// blob finder parameter
		//
		// ==========================================================================================================================
		

		struct BlobFinderParameter
		{
			uchar blobPrintBase;
			uchar trackerPrintBase;

			int floorLvl;
			int sideOffset;
			int erosionSizeX;
			int erosionSizeY;

			short minDisparity;
			short maxDisparity;

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

			// this should also be more flexible --> later
			float varianceQ;
			float varianceR;

			BlobFinderParameter() : blobPrintBase(200),
				trackerPrintBase(255),
				floorLvl(0),
				sideOffset(30),
				erosionSizeX(7),
				erosionSizeY(7),
				minDisparity(0),
				maxDisparity(10000),
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
		};


		// ==========================================================================================================================
		//
		// struct that holds the player-to-blob mapping
		//
		// ==========================================================================================================================

		// here we store the tracker id value ...
		// -1 means not assigned (ore better every thing smaller than 0)
		// we still need some managing of disapearing players
		// don't know how this should be done
		// we can rename this to MapResult ... or put this in some other header
		// or we put this to controll? we need a function that fits a tapping to a blob
		// this is somthink we have to work on ...
		struct PlayerMapping
		{
			int playerOneID;
			int playerTwoID;

			PlayerMapping() : playerOneID(-1), playerTwoID(-1) {}


			// don't know how these functions are useful, but we definitly need somthing like this
			bool noPlayerActive()
			{
				return (playerOneID < 0) && (playerTwoID < 0);
			}


			bool onePlayerActive()
			{
				return ((playerOneID < 0) && !(playerTwoID < 0)) || (!(playerOneID < 0) && (playerTwoID < 0));
			}


			bool twoPlayerActive()
			{
				return !((playerOneID < 0) || (playerTwoID < 0));
			}
		};


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

			const std::vector<int>& getAssociatedBlobs() const;

			bool hasMeasurement() const;

			int getBlobTrackerID() const;
			
			int getNoMeasurementCounter() const;

			void setVarianceQ(float varianceQ);

			void setVarianceR(float varianceR);


		private:

			static int32_t instanceCounter;

			bool init;
			bool meas;

			std::vector<int> associatedBlobs;

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

			BlobFinder(const BlobFinderParameter& parameter = BlobFinderParameter()) : parameter(parameter) {}

			bool apply(const cv::Mat& disp, const fs::core::BasicStereoValues& values);

			bool draw(cv::Mat& image, const cv::Scalar& color_corrected = cv::Scalar(255, 255, 255), const cv::Scalar& color_measured = cv::Scalar::all(-1),
				const cv::Scalar& color_predicted = cv::Scalar::all(-1)) const;

			void showBlobMask(cv::Mat& show) const;

			void showTrackerMask(cv::Mat& show) const;

			bool printBlobs(cv::Mat& show, const std::vector<cv::Scalar>& colors) const;

			bool printTrackers(cv::Mat& show, const std::vector<cv::Scalar>& colors) const;

			// here we need to provide the player information
			//void showTrackerBlobs(cv::Mat& show) const;

			const BlobFinderParameter& getParameter() const;

			BlobFinderParameter& getParameter();

			size_t getBlobCount() const;

			size_t getTrackerCount() const;


			// this would be more generic if we want to use more players
			//void getPlayer(int id);

			// don't use this functions ...
			//void getFirstPlayer();

			//void getSecondPlayer();


			// public auxiliary functions:

#ifdef MC_AUXILIARY_FUNCTIONS

			const cv::Mat& getMask() const;

			// these are just here to get a reference to the basic blob stuff in order to test it ...
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

			BlobFinderParameter parameter;


#ifdef MC_PUBLIC_INTERNALS
		public:
#endif

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

#endif // MC_COMPILE_WITH_NEW_DRAFT
