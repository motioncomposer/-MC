
#include "blob.h"

namespace mc
{

	namespace blob
	{

		// ==========================================================================================================================
		//
		// blob finder parameter
		//
		// ==========================================================================================================================


		void BlobFinderParameter::read(const cv::FileNode& fn)
		{
			fn["floorLvl"] >> floorLvl;
			fn["sideOffset"] >> sideOffset;
			fn["erosionSizeX"] >> erosionSizeX;
			fn["erosionSizeY"] >> erosionSizeY;
			fn["minDistance"] >> minDistance;
			fn["maxDistance"] >> maxDistance;
			fn["maxDistanceOffsetBack"] >> maxDistanceOffsetBack;
			fn["maxDistanceOffsetFront"] >> maxDistanceOffsetFront;
			fn["minTrackSize"] >> minTrackSize;
			fn["maxSpeckleSize"] >> maxSpeckleSize;
			fn["maxPointError"] >> maxPointError;
			fn["maxHandDistance"] >> maxHandDistance;
			fn["maxNoMeasurement"] >> maxNoMeasurement;
			fn["maxTrackerCount"] >> maxTrackerCount;
			fn["varianceQ"] >> varianceQ;
			fn["varianceR"] >> varianceR;
		}


		void BlobFinderParameter::write(cv::FileStorage& fs) const
		{
			fs << "{"
				<< "floorLvl" << floorLvl
				<< "sideOffset" << sideOffset
				<< "erosionSizeX" << erosionSizeX
				<< "erosionSizeY" << erosionSizeY
				<< "minDistance" << minDistance
				<< "maxDistance" << maxDistance
				<< "maxDistanceOffsetBack" << maxDistanceOffsetBack
				<< "maxDistanceOffsetFront" << maxDistanceOffsetFront
				<< "minTrackSize" << minTrackSize
				<< "maxSpeckleSize" << maxSpeckleSize
				<< "maxPointError" << maxPointError
				<< "maxHandDistance" << maxHandDistance
				<< "maxNoMeasurement" << maxNoMeasurement
				<< "maxTrackerCount" << maxTrackerCount
				<< "varianceQ" << varianceQ
				<< "varianceR" << varianceR
				<< "}";
		}


		void read(const cv::FileNode& fn, BlobFinderParameter& blobFinderPara, const BlobFinderParameter& default)
		{
			if (fn.empty())
				blobFinderPara = default;
			else
				blobFinderPara.read(fn);
		}


		void write(cv::FileStorage& fs, const std::string&, const BlobFinderParameter& blobFinderPara)
		{
			blobFinderPara.write(fs);
		}


		bool saveBlobFinderParameter(const BlobFinderParameter& blobFinderPara,
			const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::WRITE);

			if (fs.isOpened())
			{
				fs << key << blobFinderPara;
				return true;
			}

			return false;
		}


		bool readBlobFinderParameter(BlobFinderParameter& blobFinderPara,
			const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::READ);

			if (fs.isOpened())
			{
				fs[key] >> blobFinderPara;
				return true;
			}
			
			return false;
		}


		// ==========================================================================================================================
		//
		// struct stores some auxiliary values for a blob
		//
		// ==========================================================================================================================


		cv::Vec3f metricBlobResultTL(const fs::core::BasicStereoValues& values, const BlobResult& result)
		{
			return backProjectionFromDepth(values, result.tl, result.distance);
		}


		cv::Vec3f metricBlobResultBR(const fs::core::BasicStereoValues& values, const BlobResult& result)
		{
			return backProjectionFromDepth(values, result.br, result.distance);
		}


		cv::Vec3f metricBlobResultCP(const fs::core::BasicStereoValues& values, const BlobResult& result)
		{
			return backProjectionFromDepth(values, result.cp, result.distance);
		}


		cv::Size2f metricBlobResultSize(const fs::core::BasicStereoValues& values, const BlobResult& result)
		{
			auto vec0(backProjectionFromDepth(values, result.tl, result.distance));
			auto vec1(backProjectionFromDepth(values, result.br, result.distance));

			return{ vec1[0] - vec0[0], vec1[1] - vec0[1] };
		}


		float metricDistance(const fs::core::BasicStereoValues& values, const BlobResult& result0, const BlobResult& result1)
		{
			auto vec0(metricBlobResultCP(values, result0));
			auto vec1(metricBlobResultCP(values, result1));

			return fs::core::metricDistance(vec0, vec1);
		}


		float metricDistanceXY(const fs::core::BasicStereoValues& values, const BlobResult& result0, const BlobResult& result1)
		{
			auto vec0(metricBlobResultCP(values, result0));
			auto vec1(metricBlobResultCP(values, result1));

			vec0[2] = 0.f;
			vec1[2] = 0.f;

			return fs::core::metricDistance(vec0, vec1);
		}


		float metricDistanceYZ(const fs::core::BasicStereoValues& values, const BlobResult& result0, const BlobResult& result1)
		{
			auto vec0(metricBlobResultCP(values, result0));
			auto vec1(metricBlobResultCP(values, result1));

			vec0[0] = 0.f;
			vec1[0] = 0.f;

			return fs::core::metricDistance(vec0, vec1);
		}


		float metricDistanceXZ(const fs::core::BasicStereoValues& values, const BlobResult& result0, const BlobResult& result1)
		{
			auto vec0(metricBlobResultCP(values, result0));
			auto vec1(metricBlobResultCP(values, result1));

			vec0[1] = 0.f;
			vec1[1] = 0.f;

			return fs::core::metricDistance(vec0, vec1);
		}



		// ==========================================================================================================================
		//
		// blob tracker object
		//
		// ==========================================================================================================================


		uint32_t BlobTracker::instanceCounter = 0;


		float BlobTracker::match(const BlobResult& result, const BlobFinderParameter& parameter, const fs::core::BasicStereoValues& values) const
		{

			auto candidate(true);
			auto score(std::numeric_limits<float>::max());

			if (init)
			{

				/*
				*
				* When init is set the master blob is already assigned to the tracker (this is done by the constructor),
				* so we only need to check here for useful candidates that could be a hand.
				*
				*/

				//
				candidate = candidate && (result.area < parameter.minTrackSize);
				candidate = candidate && (result.area > parameter.maxSpeckleSize);
				candidate = candidate && (result.br.y < measuredResult.br.y - (0.3333 * (measuredResult.br.y - measuredResult.tl.y)));
				candidate = candidate && (result.tl.y > measuredResult.tl.y);
				
				//
				auto metricXZ(metricDistanceXZ(values, result, measuredResult));
				candidate = candidate && (metricXZ < parameter.maxHandDistance);
				
				// this here is only useful if we want to add some big blobs
				//candidate = candidate && (result.distance < measuredResult.distance + parameter.maxDistanceOffsetBack);
				//candidate = candidate && (result.distance > measuredResult.distance - parameter.maxDistanceOffsetFront);

				score = candidate ? metricXZ : score;

			}
			else if (meas)
			{

				/*
				*
				* When meas is set we already have a master blob assigned to this already existing tracker (that means,
				* we have a prediction). In this case we have to check if there are useful candidates that could
				* be a hand with respect to the predicted blob size
				*
				*/

				// here we need to check if the measured is still smaller than the predicted
				// we can add here the case is we match another big blob


				// special case with blob split and blob merge is not yet handled

				// here we need a branch if it is grater than tracksize or greater than blobsize
				// --> so we can add more big blobs to the tracker in case the blobs scatters only possible if we have a prediction


				// we should specify a maximum size jump which is allowed (threshold for each direction of tl, br)

				//
				candidate = candidate && (result.area < parameter.minTrackSize);
				candidate = candidate && (result.area > parameter.maxSpeckleSize);
				candidate = candidate && (result.br.y < measuredResult.br.y - (0.3333 * (measuredResult.br.y - measuredResult.tl.y)));
				candidate = candidate && (result.tl.y > measuredResult.tl.y);

				//
				auto metricXZ(metricDistanceXZ(values, result, measuredResult));
				candidate = candidate && (metricXZ < parameter.maxHandDistance);

				//
				auto errorTLX = predictedResult.tl.x - result.tl.x;
				auto errorTLY = predictedResult.tl.y - result.tl.y;
				auto errorBRX = result.br.x - predictedResult.br.x;
				auto errorBRY = result.br.y - predictedResult.br.y;
				candidate = candidate && (errorTLX < 0.f || errorTLX < parameter.maxPointError);
				candidate = candidate && (errorTLY < 0.f || errorTLY < parameter.maxPointError);
				candidate = candidate && (errorBRX < 0.f || errorBRX < parameter.maxPointError);
				candidate = candidate && (errorBRY < 0.f || errorBRY < parameter.maxPointError);


				score = candidate ? ((errorTLX < 0.f ? -errorTLX : 2.f * errorTLX) +
					(errorTLY < 0.f ? -errorTLY : 2.f * errorTLY) +
					(errorBRX < 0.f ? -errorBRX : 2.f * errorBRX) +
					(errorBRY < 0.f ? -errorBRY : 2.f * errorBRY) +
					metricXZ) : score;

			}
			else
			{
				/*
				*
				* When meas was not set we have not yet find the master blob for this already exiting tracker (that means,
				* we have a prediction). So we are checking the similarity of both prediction and candidate
				*
				*/

				candidate = candidate && !(result.area < parameter.minTrackSize);

				auto errorTLX = predictedResult.tl.x - result.tl.x;
				auto errorTLY = predictedResult.tl.y - result.tl.y;
				auto errorBRX = result.br.x - predictedResult.br.x;
				auto errorBRY = result.br.y - predictedResult.br.y;

				score = candidate ? ((errorTLX < 0.f ? -errorTLX : 2.f * errorTLX) +
					(errorTLY < 0.f ? -errorTLY : 2.f * errorTLY) +
					(errorBRX < 0.f ? -errorBRX : 2.f * errorBRX) +
					(errorBRY < 0.f ? -errorBRY : 2.f * errorBRY)) : score;

			}

			return score;

		}


		void BlobTracker::collect(const BlobResult& result, int index)
		{
			if (meas)
			{
				cv::Rect dmy(measuredResult.tl, measuredResult.br);
				dmy |= cv::Rect(result.tl, result.br);

				measuredResult.tl = dmy.tl();
				measuredResult.br = dmy.br();

				measuredResult.area = (measuredResult.br.x - measuredResult.tl.x) * (measuredResult.br.y - measuredResult.tl.y);

				/*
				*
				* We don't update distance and centroid at this position as we assume,
				* that the first blob that we insert is the master blob and every following blob is
				* just the adding of some cut of extremities.
				*
				* That's why:
				*	- the centroid is therefor always the centroid of the master blob
				*	- the distance is therefor always the distance of the master blob
				*
				*/
			}
			else
			{
				measuredResult = result;
				meas = true;
			}

			associatedBlobs.push_back(index);
		}


		void BlobTracker::predict()
		{
			(void)kalman.predict();

			predictedResult.tl = cv::Point2f(kalman.x_(0), kalman.x_(1));
			predictedResult.br = cv::Point2f(kalman.x_(2), kalman.x_(3));
			predictedResult.cp = cv::Point2f(kalman.x_(4), kalman.x_(5));
			predictedResult.distance = kalman.x_(6);

			meas = false;
			associatedBlobs.clear();
		}


		void BlobTracker::correct()
		{
			if (init)
			{
				kalman.x.setZero();

				kalman.x(0) = measuredResult.tl.x;
				kalman.x(1) = measuredResult.tl.y;
				kalman.x(2) = measuredResult.br.x;
				kalman.x(3) = measuredResult.br.y;
				kalman.x(4) = measuredResult.cp.x;
				kalman.x(5) = measuredResult.cp.y;
				kalman.x(6) = measuredResult.distance;

				init = false;
			}
			else if (meas)
			{
				Eigen::VectorXf measurement(7);
				measurement(0) = measuredResult.tl.x;
				measurement(1) = measuredResult.tl.y;
				measurement(2) = measuredResult.br.x;
				measurement(3) = measuredResult.br.y;
				measurement(4) = measuredResult.cp.x;
				measurement(5) = measuredResult.cp.y;
				measurement(6) = measuredResult.distance;

				noMeasurementCounter = 0;
				(void)kalman.correct(measurement);
			}
			else
			{
				++noMeasurementCounter;
				(void)kalman.correct();
			}


			correctedResult.tl = cv::Point2f(kalman.x(0), kalman.x(1));
			correctedResult.br = cv::Point2f(kalman.x(2), kalman.x(3));
			correctedResult.cp = cv::Point2f(kalman.x(4), kalman.x(5));
			correctedResult.distance = kalman.x(6);

		}


		const BlobResult& BlobTracker::getPredictedResult() const
		{
			return predictedResult;
		}


		const BlobResult& BlobTracker::getMeasuredResult() const
		{
			return measuredResult;
		}


		const BlobResult& BlobTracker::getCorrectedResult() const
		{
			return correctedResult;
		}


		cv::Rect BlobTracker::getPredictedROI() const
		{
			return{ predictedResult.tl, predictedResult.br };
		}


		cv::Rect BlobTracker::getMeasuredROI() const
		{
			return{ measuredResult.tl, measuredResult.br };
		}


		cv::Rect BlobTracker::getCorrectedROI() const
		{
			return{ correctedResult.tl, correctedResult.br };
		}


		const std::vector<uint32_t>& BlobTracker::getAssociatedBlobs() const
		{
			return associatedBlobs;
		}


		bool BlobTracker::hasMeasurement() const
		{
			return meas;
		}


		int BlobTracker::getBlobTrackerID() const
		{
			return blobTrackerID;
		}


		int BlobTracker::getNoMeasurementCounter() const
		{
			return noMeasurementCounter;
		}


		void BlobTracker::setVarianceQ(float varianceQ)
		{
			kalman.Q.setIdentity() *= varianceQ;
		}


		void BlobTracker::setVarianceR(float varianceR)
		{
			kalman.R.setIdentity() *= varianceR;
		}
		

		// ==========================================================================================================================
		//
		// blob finder object
		//
		// ==========================================================================================================================


		bool BlobFinder::apply(const cv::Mat& disp)
		{
			if (disp.empty() || disp.type() != CV_16SC1)
				return false;

			findBlobs(disp);

			sortBlobs();

			cleanUpTrackers();


			predictTrackers();

			// may be we can write this without indices c and d
			for (auto&& c = 0; c < ordering.size(); ++c)
			{
				updateResult(disp, values, ordering[c]);

				auto curScore(0.f);
				auto minScore(std::numeric_limits<float>::max());
				auto trackerID(-1);

				for (auto&& d = 0; d < trackers.size(); ++d)
				{
					// we may be can store the BasicStereoValues also as member
					curScore = trackers[d].match(result, parameter, values);

					if (curScore < minScore)
					{
						minScore = curScore;
						trackerID = d;
					}
				}

				if (minScore != std::numeric_limits<float>::max())
					trackers[trackerID].collect(result, ordering[c]);
				else if (result.area > parameter.minTrackSize)
					trackers.push_back(BlobTracker(result, ordering[c], parameter.varianceQ, parameter.varianceR));

			}

			correctTrackers();
			
			return true;
		}


		bool BlobFinder::draw(cv::Mat& image, const cv::Scalar& color_corrected, const cv::Scalar& color_measured,
			const cv::Scalar& color_predicted) const
		{
			if (image.empty() || image.type() != CV_8UC3)
				return false;

			for (auto& it : trackers)
			{
				if (color_predicted != cv::Scalar::all(-1))
					cv::rectangle(image, it.getPredictedROI(), color_predicted, 6);

				if (color_measured != cv::Scalar::all(-1))
					cv::rectangle(image, it.getMeasuredROI(), color_measured, 4);
				
				cv::rectangle(image, it.getCorrectedROI(), color_corrected, 2);
				cv::circle(image, it.getCorrectedResult().cp, 5, color_corrected, -1);
			}

			return true;
		}


		void BlobFinder::showBlobMask(cv::Mat& show) const
		{
			show = cv::Mat::zeros(mask.size(), CV_8UC1);
			for (auto&& c = 0; c < contours.size(); c++)
				drawContours(show, contours, ordering[c], 255 - (c * ((255) / contours.size())), CV_FILLED);
		}


		void BlobFinder::showTrackerMask(cv::Mat& show) const
		{
			show = cv::Mat::zeros(mask.size(), CV_8UC1);
			for (auto&& c = 0; c < trackers.size(); c++)
			{
				cv::Rect area = trackers[c].getCorrectedROI();
				for(auto& it : trackers[c].getAssociatedBlobs())
					drawContours(show(area), contours, it, 255 - (c * ((255) / contours.size())), CV_FILLED, 8, hierarchy, 2147483647, -area.tl());
			}
		}


		void BlobFinder::plotTrackerID(cv::Mat& show) const
		{
			show = -1 * cv::Mat::ones(mask.size(), CV_32SC1);
			for (auto&& c = 0; c < trackers.size(); c++)
			{
				cv::Rect area = trackers[c].getCorrectedROI();
				for (auto& it : trackers[c].getAssociatedBlobs())
					drawContours(show(area), contours, it, trackers[c].getBlobTrackerID(), CV_FILLED, 8, hierarchy, 2147483647, -area.tl());
			}
		}


		bool BlobFinder::printBlobs(cv::Mat& show, const std::vector<cv::Scalar>& colors) const
		{
			if (show.empty() || show.type() != CV_8UC3)
				return false;

			auto max = std::min(colors.size(), contours.size());

			for (auto&& c = 0; c < max; c++)
				drawContours(show, contours, ordering[c], colors[c], CV_FILLED);

			return true;
		}


		bool BlobFinder::printTrackers(cv::Mat& show, const std::vector<cv::Scalar>& colors) const
		{
			if (show.empty() || show.type() != CV_8UC3)
				return false;

			auto max = std::min(colors.size(), trackers.size());

			for (auto&& c = 0; c < max; c++)
			{
				cv::Rect area = trackers[c].getCorrectedROI();
				for (auto& it : trackers[c].getAssociatedBlobs())
					drawContours(show(area), contours, it, colors[c], CV_FILLED, 8, hierarchy, 2147483647, -area.tl());
			}

			return true;
		}


		const BlobFinderParameter& BlobFinder::getParameter() const
		{
			return parameter;
		}


		const DisparityThresholds& BlobFinder::getThresholds() const
		{
			return thresholds;
		}


		size_t BlobFinder::getBlobCount() const
		{
			return rois.size();
		}


		size_t BlobFinder::getTrackerCount() const
		{
			return trackers.size();
		}


		void BlobFinder::adjustDistanceRange(float minDistance, float maxDistance)
		{
			if (minDistance > maxDistance)
				std::swap(minDistance, maxDistance);

			parameter.minDistance = minDistance;
			parameter.maxDistance = maxDistance;

			thresholds.minDisparity = static_cast<short>(16 * values.focalLength * values.baseLineLength / parameter.maxDistance);
			thresholds.maxDisparity = 16 * values.focalLength * values.baseLineLength / parameter.minDistance;
		}


		void BlobFinder::adjustFloorLevel(int floorLvl)
		{
			parameter.floorLvl = floorLvl;
		}


		bool BlobFinder::draw(cv::Mat& image, const mc::structures::PlayerSelection& selection,
			const cv::Scalar& color_one, const cv::Scalar& color_two, const cv::Scalar& color_inactive) const
		{
			if (image.empty() || image.type() != CV_8UC3)
				return false;

			for (auto& it : trackers)
			{
				if (it.getBlobTrackerID() == selection[0])
					cv::rectangle(image, it.getCorrectedROI(), color_one, 2);
				else if (it.getBlobTrackerID() == selection[1])
					cv::rectangle(image, it.getCorrectedROI(), color_two, 2);
				else
					cv::rectangle(image, it.getCorrectedROI(), color_inactive, 2);
			}

			return true;
		}


		bool BlobFinder::printPlayers(cv::Mat& show, const mc::structures::PlayerSelection& selection,
			const cv::Scalar& color_one, const cv::Scalar& color_two, const cv::Scalar& color_inactive) const
		{
			if (show.empty() || show.type() != CV_8UC3)
				return false;

			for (auto&& c = 0; c < trackers.size(); c++)
			{
				if (trackers[c].getBlobTrackerID() == selection[0])
				{
					cv::Rect area = trackers[c].getCorrectedROI();
					for (auto& it : trackers[c].getAssociatedBlobs())
						drawContours(show(area), contours, it, color_one, CV_FILLED, 8, hierarchy, 2147483647, -area.tl());
				}
				else if (trackers[c].getBlobTrackerID() == selection[1])
				{
					cv::Rect area = trackers[c].getCorrectedROI();
					for (auto& it : trackers[c].getAssociatedBlobs())
						drawContours(show(area), contours, it, color_two, CV_FILLED, 8, hierarchy, 2147483647, -area.tl());
				}
				else
				{
					cv::Rect area = trackers[c].getCorrectedROI();
					for (auto& it : trackers[c].getAssociatedBlobs())
						drawContours(show(area), contours, it, color_inactive, CV_FILLED, 8, hierarchy, 2147483647, -area.tl());
				}

				
			}

			return true;
		}


		void BlobFinder::updateSharedData(mc::structures::PlayerSelection& selection, mc::structures::SharedData& data) const
		{

			data.environment.maxDistance = parameter.maxDistance;
			data.environment.minDistance = parameter.minDistance;
			data.environment.floorLvl = parameter.floorLvl;

			data.trackerData[0] = mc::structures::TrackerData();
			data.trackerData[1] = mc::structures::TrackerData();


			if (selection[0] >= 0)
			{
				bool found(false);
				for (auto& it : trackers)
				{
					if (it.getBlobTrackerID() == selection[0])
					{
						data.trackerData[0].tl = it.getCorrectedResult().tl;
						data.trackerData[0].br = it.getCorrectedResult().br;
						data.trackerData[0].cp = it.getCorrectedResult().cp;
						data.trackerData[0].distance = it.getCorrectedResult().distance;
						data.trackerData[0].contourIdx = it.getAssociatedBlobs();
						data.trackerData[0].metricTL = fs::core::backProjectionFromDepth(values, it.getCorrectedResult().tl, it.getCorrectedResult().distance);
						data.trackerData[0].metricBR = fs::core::backProjectionFromDepth(values, it.getCorrectedResult().br, it.getCorrectedResult().distance);
						data.trackerData[0].metricCP = fs::core::backProjectionFromDepth(values, it.getCorrectedResult().cp, it.getCorrectedResult().distance);

						found = true;
						break;
					}
				}

				if (!found)
					selection[0] = -1;
			}
			

			if (selection[1] >= 0)
			{
				bool found(false);
				for (auto& it : trackers)
				{
					if (it.getBlobTrackerID() == selection[1])
					{
						data.trackerData[1].tl = it.getCorrectedResult().tl;
						data.trackerData[1].br = it.getCorrectedResult().br;
						data.trackerData[1].cp = it.getCorrectedResult().cp;
						data.trackerData[1].distance = it.getCorrectedResult().distance;
						data.trackerData[1].contourIdx = it.getAssociatedBlobs();
						data.trackerData[1].metricTL = fs::core::backProjectionFromDepth(values, it.getCorrectedResult().tl, it.getCorrectedResult().distance);
						data.trackerData[1].metricBR = fs::core::backProjectionFromDepth(values, it.getCorrectedResult().br, it.getCorrectedResult().distance);
						data.trackerData[1].metricCP = fs::core::backProjectionFromDepth(values, it.getCorrectedResult().cp, it.getCorrectedResult().distance);

						found = true;
						break;
					}
				}

				if (!found)
					selection[1] = -1;
			}

		}


#ifdef MC_AUXILIARY_FUNCTIONS

		const cv::Mat& BlobFinder::getMask() const
		{
			return mask;
		}


		const std::vector<int>& BlobFinder::getOrdering() const
		{
			return ordering;
		}


		const std::vector<std::vector<cv::Point>>& BlobFinder::getContours() const
		{
			return contours;
		}


		const std::vector<cv::Vec4i>& BlobFinder::getHierarchy() const
		{
			return hierarchy;
		}


		const std::vector<cv::Rect>& BlobFinder::getRois() const
		{
			return rois;
		}


		const BlobResult& BlobFinder::getBlobResult() const
		{
			return result;
		}


		const std::vector<BlobTracker>& BlobFinder::getTrackers() const
		{
			return trackers;
		}

#endif

		void BlobFinder::findBlobs(const cv::Mat& disp)
		{
			cv::inRange(disp, thresholds.minDisparity, thresholds.maxDisparity, mask);
			cv::morphologyEx(mask, mask, cv::MORPH_ERODE,
				cv::getStructuringElement(cv::MorphShapes::MORPH_ELLIPSE,
				{ parameter.erosionSizeX, parameter.erosionSizeY }));

			const uchar* ptr = mask.ptr<uchar>(parameter.floorLvl);
			std::vector<uchar> floor(ptr, ptr + mask.cols);
			int start(0), end(mask.cols - 1);
			for (auto&& c = 1; c < floor.size(); ++c)
			{
				if (floor[c] != 0 && floor[c - 1] == 0 || c == floor.size() - 1)
				{
					end = c - parameter.sideOffset;
					if (end - start > 0)
						mask(cv::Rect(cv::Point(start, parameter.floorLvl), cv::Point(end, mask.rows - 1))).setTo(0);
				}
				else if (floor[c] == 0 && floor[c - 1] != 0)
					start = c + parameter.sideOffset;
			}

			contours.clear();
			hierarchy.clear();
			cv::findContours(mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

			rois.clear();
			rois.reserve(contours.size());
			for (auto& it : contours)
				rois.push_back(cv::boundingRect(it));

		}


		void BlobFinder::sortBlobs()
		{
			ordering.clear();
			ordering.resize(rois.size());

			std::iota(ordering.begin(), ordering.end(), 0);
			std::sort(ordering.begin(), ordering.end(), [&](int a, int b) {return rois[a].area() > rois[b].area(); });
		}


		void BlobFinder::cleanUpTrackers()
		{
			shadow.clear();
			shadow.reserve(trackers.size());
			for (auto& it : trackers)
			{
				if (it.hasMeasurement() || it.getNoMeasurementCounter() < parameter.maxNoMeasurement)
					shadow.push_back(it);
			}
			std::swap(trackers, shadow);
		}


		void BlobFinder::predictTrackers()
		{
			for (auto& it : trackers)
				it.predict();
		}


		void BlobFinder::updateResult(const cv::Mat& disp, const fs::core::BasicStereoValues& values, int index)
		{
			result.tl = rois[index].tl();
			result.br = rois[index].br();
			result.area = rois[index].area();

			result.distance = 0.f;

			short dmy;
			for (auto& it : contours[index])
			{
				dmy = disp.at<short>(it);
				result.distance += (dmy > 0) ? dmy : 0;
			}


			fs::core::SpatialMoments moments = fs::core::getSpatialMoments(contours[index]);
			result.cp = { static_cast<float>(moments.m10 / moments.m00), static_cast<float>(moments.m01 / moments.m00) };


			result.distance /= static_cast<float>(contours[index].size());
			result.distance = (values.focalLength * values.baseLineLength * 16.f) / result.distance;
		}


		void BlobFinder::correctTrackers()
		{
			for (auto& it : trackers)
				it.correct();
		}

	}
}