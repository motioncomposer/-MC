
#include "position.h"


namespace mc
{
	namespace position
	{

		// ==========================================================================================================================
		//
		// position estimator parameter
		//
		// ==========================================================================================================================


		void PositionEstimatorParameter::read(const cv::FileNode& fn)
		{
			/*
			fn["offsetFront"] >> offsetFront;
			fn["offsetBack"] >> offsetBack;
			fn["moveThreshold"] >> moveThreshold;
			fn["readyTimeMilli"] >> readyTimeMilli;
			*/
		}


		void PositionEstimatorParameter::write(cv::FileStorage& fs) const
		{
			/*
			fs << "{"
			<< "offsetFront" << offsetFront
			<< "offsetBack" << offsetBack
			<< "moveThreshold" << moveThreshold
			<< "readyTimeMilli" << readyTimeMilli
			<< "}";
			*/
		}


		void read(const cv::FileNode& fn, PositionEstimatorParameter& positionPara, const PositionEstimatorParameter& default)
		{
			if (fn.empty())
				positionPara = default;
			else
				positionPara.read(fn);
		}


		void write(cv::FileStorage& fs, const std::string&, const PositionEstimatorParameter& positionPara)
		{
			positionPara.write(fs);
		}


		bool savePositionEstimatorParameter(const PositionEstimatorParameter& positionPara,
			const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::WRITE);

			if (fs.isOpened())
			{
				fs << key << positionPara;
				return true;
			}

			return false;
		}


		bool readPositionEstimatorParameter(PositionEstimatorParameter& positionPara,
			const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::READ);

			if (fs.isOpened())
			{
				fs[key] >> positionPara;
				return true;
			}

			return false;
		}


		// ==========================================================================================================================
		//
		// position estimator object
		//
		// ==========================================================================================================================


		bool PositionEstimator::apply(const cv::Mat& disp, const mc::structures::PlayerActivation activation, const mc::structures::PlayerSelection& selection, const std::vector<std::vector<cv::Point>>& contours,
			mc::structures::SharedData& shared, mc::structures::Result& result)
		{
			if (disp.empty() || disp.type() != CV_16SC1 || contours.empty())
				return false;


			// check if players are swapped
			if (selection[0] == trackerID[1] && selection[1] == trackerID[0])
			{
				std::swap(trackerID[0], trackerID[1]);
				std::swap(metricHeight[0], metricHeight[1]);
				std::swap(metricHeightFilter[0], metricHeightFilter[1]);
			}


			// ACTIVATION is not used yet ...
			// this is implemented when we finish the position result ...

			// here we somhow have to take the active into account in order to retrieve the result
			// therfor we have to specify a new trackPosition(...) subfunction (if we need a tracker)

			if (selection[0] >= 0 && shared.locationData[0].present)
				calculatePositionResult(disp, selection, shared, contours, result, 0);
			else
				result.player[0].position = mc::structures::PositionResult();

			if (selection[1] >= 0 && shared.locationData[1].present)
				calculatePositionResult(disp, selection, shared, contours, result, 1);
			else
				result.player[1].position = mc::structures::PositionResult();



			shared.positionData[0].metricHeight = metricHeight[0];
			shared.positionData[0].pixelHeight = pixelHeight[0];
			shared.positionData[0].topLvl = topLvl[0];
			shared.positionData[0].metricCenter = metricCenter[0];
			shared.positionData[0].pixelCenter = pixelCenter[0];
			shared.positionData[0].midLvl = midLvl[0];

			shared.positionData[1].metricHeight = metricHeight[1];
			shared.positionData[1].pixelHeight = pixelHeight[1];
			shared.positionData[1].topLvl = topLvl[1];
			shared.positionData[1].metricCenter = metricCenter[1];
			shared.positionData[1].pixelCenter = pixelCenter[1];
			shared.positionData[1].midLvl = midLvl[1];

			return true;
		}


		bool PositionEstimator::draw(cv::Mat& show, const mc::structures::SharedData& shared, const cv::Scalar& color_one, const cv::Scalar& color_two)
		{
			if (show.empty() || show.type() != CV_8UC3)
				return false;


			// later we can remove the shared parameter if we finaly have stored all needed values as member


			cv::line(show, { static_cast<int>(shared.trackerData[0].tl.x), static_cast<int>(topLvl[0]) },
			{ static_cast<int>(shared.trackerData[0].br.x), static_cast<int>(topLvl[0]) }, color_one, 2);


			cv::line(show, { static_cast<int>(shared.trackerData[1].tl.x), static_cast<int>(topLvl[1]) },
			{ static_cast<int>(shared.trackerData[1].br.x), static_cast<int>(topLvl[1]) }, color_two, 2);

			return true;
		}


		void PositionEstimator::calculatePositionResult(const cv::Mat& disp, const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared,
			const std::vector<std::vector<cv::Point>>& contours, mc::structures::Result& result, uint32_t idx)
		{
			if (shared.locationData[idx].present && !shared.locationData[idx].ready)
			{
				learnInitialValues(shared, idx);
				calculateBasicResult(disp, selection, shared, contours, result, idx);
			}
			else if (shared.locationData[idx].present && shared.locationData[idx].ready)
				calculateFullResult(disp, selection, shared, contours, result, idx);
			else
				result.player[idx].position = mc::structures::PositionResult();
		}


		void PositionEstimator::learnInitialValues(const mc::structures::SharedData& shared, uint32_t idx)
		{

			metricHeight[idx] = metricHeightFilter[idx].apply(shared.trackerData[idx].metricBR[1] - shared.trackerData[idx].metricTL[1]);
			pixelHeight[idx] = values.focalLength * metricHeight[idx] / shared.trackerData[idx].distance;
			topLvl[idx] = shared.trackerData[idx].br.y - pixelHeight[idx];


			metricCenter[idx] = metricCenterFilter[idx].apply(shared.trackerData[idx].metricBR[1] - shared.trackerData[idx].metricCP[1]);
			pixelCenter[idx] = values.focalLength * metricCenter[idx] / shared.trackerData[idx].distance;
			midLvl[idx] = shared.trackerData[idx].br.y - pixelCenter[idx];

		}


		void PositionEstimator::calculateBasicResult(const cv::Mat& disp, const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared,
			const std::vector<std::vector<cv::Point>>& contours, mc::structures::Result& result, uint32_t idx)
		{
			// this is when we do not have initial values ...
			result.player[idx].position.heightLevel = 2;
		}


		void PositionEstimator::calculateFullResult(const cv::Mat& disp, const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared,
			const std::vector<std::vector<cv::Point>>& contours, mc::structures::Result& result, uint32_t idx)
		{

			// if we realy have a tracker, then we need to track even if the player is not active ...


			// don't now if these values are really needed ...
			auto metricTL = fs::core::backProjectionFromDepth(values, shared.trackerData[idx].tl, shared.trackerData[idx].distance);
			auto metricBR = fs::core::backProjectionFromDepth(values, shared.trackerData[idx].br, shared.trackerData[idx].distance);
			auto metricCP = fs::core::backProjectionFromDepth(values, shared.trackerData[idx].cp, shared.trackerData[idx].distance);

			cv::Size2f metricSize{ metricBR[0] - metricTL[0], metricBR[1] - metricTL[1] };


			// actually this stuff is realy noisy ... we said, that we want to find the arm width (left + right) first, and after this we add these two together as the total widhth (metric)
			// we need to provide some max value ... thex are learned ...
			result.player[idx].position.width = metricSize.width;
			result.player[idx].position.height = metricSize.height;

			// here we calculate heightLvl ... important ...

			// we have to think about other limits, we should also use floorLvl
			// and we have to put them into the position parameter
			// we wanted to use a schmitt trigger for this to prevent the system from continously switching
			auto heightRation = metricSize.height / metricHeight[idx];

			if (heightRation > 1.f)
				result.player[idx].position.heightLevel = 3;
			else if (heightRation > 0.66f)
				result.player[idx].position.heightLevel = 2;
			else if (heightRation > 0.33f)
				result.player[idx].position.heightLevel = 1;
			else
				result.player[idx].position.heightLevel = 0;


			// here we can make two sub functions
			if (shared.trackerData[idx].contourIdx.size() == 1)
			{

				auto pixelHeight = values.focalLength * metricHeight[0] / shared.trackerData[idx].distance;

				cv::Point top(contours[shared.trackerData[idx].contourIdx[0]][0]);

				for (auto& it : contours[shared.trackerData[idx].contourIdx[0]])
				{
					// and we also need to find the point with the biggest score ... accordingly to ... a line we can put into the centroid
				}




				// in this case we only have one contour we have to analize
			}
			else
			{
				// we have more than one contour, which actually means that one of the contours we have added are hand contours ... or some garbage ...
				// first idx is the master blob ...
			}
		}

	}
}