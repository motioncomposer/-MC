
#include "location.h"


namespace mc
{
	namespace location
	{

		// ==========================================================================================================================
		//
		// location estimator parameter
		//
		// ==========================================================================================================================


		void LocationEstimatorParameter::read(const cv::FileNode& fn)
		{
			fn["offsetFront"] >> offsetFront;
			fn["offsetBack"] >> offsetBack;
			fn["moveThreshold"] >> moveThreshold;
			fn["readyTimeMilli"] >> readyTimeMilli;
		}


		void LocationEstimatorParameter::write(cv::FileStorage& fs) const
		{
			fs << "{"
				<< "offsetFront" << offsetFront
				<< "offsetBack" << offsetBack
				<< "moveThreshold" << moveThreshold
				<< "readyTimeMilli" << readyTimeMilli
				<< "}";
		}


		void read(const cv::FileNode& fn, LocationEstimatorParameter& locationPara, const LocationEstimatorParameter& default)
		{
			if (fn.empty())
				locationPara = default;
			else
				locationPara.read(fn);
		}


		void write(cv::FileStorage& fs, const std::string&, const LocationEstimatorParameter& locationPara)
		{
			locationPara.write(fs);
		}


		bool saveLocationEstimatorParameter(const LocationEstimatorParameter& locationPara,
			const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::WRITE);

			if (fs.isOpened())
			{
				fs << key << locationPara;
				return true;
			}

			return false;
		}


		bool readLocationEstimatorParameter(LocationEstimatorParameter& locationPara,
			const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::READ);

			if (fs.isOpened())
			{
				fs[key] >> locationPara;
				return true;
			}

			return false;
		}


		// ==========================================================================================================================
		//
		// position estimator object
		//
		// ==========================================================================================================================
		

		void LocationEstimator::apply(const cv::Size& imageSize, const mc::structures::PlayerActivation activation, const mc::structures::PlayerSelection& selection,
			mc::structures::SharedData& shared, mc::structures::Result& result)
		{

			// check if players are swapped
			if (selection[0] == trackerID[1] && selection[1] == trackerID[0])
			{
				std::swap(trackerID[0], trackerID[1]);
				std::swap(lastCentroidX[0], lastCentroidX[1]);
				std::swap(lastMove[0], lastMove[1]);
			}



			if (selection[0] >= 0)
			{
				updatePlayerState(selection, shared, 0);
				if (activation[0])
					calculateLocationResult(imageSize, selection, shared, result, 0);
				else
					result.player[0].location = mc::structures::LocationResult();
			}
			else
				result.player[0].location = mc::structures::LocationResult();


			if (selection[1] >= 0)
			{
				updatePlayerState(selection, shared, 1);
				if (activation[1])
					calculateLocationResult(imageSize, selection, shared, result, 1);
				else
					result.player[1].location = mc::structures::LocationResult();
			}
			else
				result.player[1].location = mc::structures::LocationResult();



			shared.locationData[0].present = present[0];
			shared.locationData[0].ready = ready[0];
			shared.locationData[0].outOfRange = outOfRange[0];

			shared.locationData[1].present = present[1];
			shared.locationData[1].ready = ready[1];
			shared.locationData[1].outOfRange = outOfRange[1];

		}


		bool LocationEstimator::draw(cv::Mat& show, const cv::Scalar& color_present, const cv::Scalar& color_ready)
		{
			if (show.empty() || show.type() != CV_8UC3)
				return false;

			if (present[0] && !outOfRange[0])
			{
				if (ready[0])
					cv::putText(show, "P0: ready", { 25, 15 }, CV_FONT_HERSHEY_PLAIN, 1, color_ready);
				else
					cv::putText(show, "P0: present", { 25, 15 }, CV_FONT_HERSHEY_PLAIN, 1, color_present);

			}			


			if (present[1] && !outOfRange[1])
			{
				if (ready[1])
					cv::putText(show, "P1: ready", { 25, 35 }, CV_FONT_HERSHEY_PLAIN, 1, color_ready);
				else
					cv::putText(show, "P1: present", { 25, 35 }, CV_FONT_HERSHEY_PLAIN, 1, color_present);
			}

			return true;
		}


		void LocationEstimator::updatePlayerState(const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared, uint32_t idx)
		{

			if (std::abs(lastCentroidX[idx] - shared.trackerData[idx].metricCP[0]) > parameter.moveThreshold)
				lastMove[idx] = std::chrono::high_resolution_clock::now();

			lastCentroidX[idx] = shared.trackerData[idx].metricCP[0];


			if (shared.trackerData[idx].distance < (shared.environment.minDistance + parameter.offsetFront) ||
				shared.trackerData[idx].distance > (shared.environment.maxDistance - parameter.offsetBack))
				outOfRange[idx] = true;
			else
				outOfRange[idx] = false;


			if (selection[idx] != LocationEstimator::trackerID[idx])
			{
				LocationEstimator::trackerID[idx] = selection[idx];
				present[idx] = !shared.trackerData[idx].contourIdx.empty();
				ready[idx] = false;
			}
			else
			{
				present[idx] = !shared.trackerData[idx].contourIdx.empty();
				ready[idx] = present[idx] && ready[idx] ? true :
					((std::chrono::high_resolution_clock::now() - lastMove[idx]) > std::chrono::duration<float, std::milli>(parameter.readyTimeMilli));
			}

		}


		void LocationEstimator::calculateLocationResult(const cv::Size& imageSize, const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared,
			mc::structures::Result& result, uint32_t idx)
		{

			result.player[idx].location.centerX = shared.trackerData[idx].cp.x / imageSize.width;
			result.player[idx].location.centerZ = (shared.trackerData[idx].distance - shared.environment.minDistance) / (shared.environment.maxDistance - shared.environment.minDistance);

			result.player[idx].location.present = present[idx];
			result.player[idx].location.ready = ready[idx];
			result.player[idx].location.outOfRange = outOfRange[idx];
		}


	}

}