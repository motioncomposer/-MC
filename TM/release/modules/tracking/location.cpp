
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
		// location estimator object
		//
		// ==========================================================================================================================
		

		void LocationEstimator::apply(const cv::Size& imageSize, const mc::structures::PlayerActivation activation, const mc::structures::PlayerSelection& selection,
			mc::structures::SharedData& shared, mc::structures::Result& result)
		{


			if (selection[0] == trackerID[1] || selection[1] == trackerID[0])
			{
				std::swap(trackerID[0], trackerID[1]);
				std::swap(kernel[0], kernel[1]);
			}


			if (selection[0] >= 0 && trackerID[0] == selection[0])
			{
				updatePlayerState(selection, shared, 0);
				if (activation[0])
					calculateLocationResult(imageSize, shared, result, 0);
				else
					result.player[0].location = mc::structures::LocationResult();
			}
			else
			{
				kernel[0] = LocationEstimatorKernel();
				result.player[0].location = mc::structures::LocationResult();
			}

			trackerID[0] = selection[0];


			if (selection[1] >= 0 && trackerID[1] == selection[1])
			{
				updatePlayerState(selection, shared, 1);
				if (activation[1])
					calculateLocationResult(imageSize, shared, result, 1);
				else
					result.player[1].location = mc::structures::LocationResult();
			}
			else
			{
				kernel[1] = LocationEstimatorKernel();
				result.player[1].location = mc::structures::LocationResult();
			}

			trackerID[1] = selection[1];


			writeSharedData(shared, 0);
			writeSharedData(shared, 1);
		}


		bool LocationEstimator::draw(cv::Mat& show, const cv::Scalar& color_present, const cv::Scalar& color_ready)
		{
			if (show.empty() || show.type() != CV_8UC3)
				return false;

			if (kernel[0].present && !kernel[0].outOfRange)
			{
				if (kernel[0].ready)
					cv::putText(show, "P0: ready", { 25, 15 }, CV_FONT_HERSHEY_PLAIN, 1, color_ready);
				else
					cv::putText(show, "P0: present", { 25, 15 }, CV_FONT_HERSHEY_PLAIN, 1, color_present);

			}			


			if (kernel[1].present && !kernel[1].outOfRange)
			{
				if (kernel[1].ready)
					cv::putText(show, "P1: ready", { 25, 35 }, CV_FONT_HERSHEY_PLAIN, 1, color_ready);
				else
					cv::putText(show, "P1: present", { 25, 35 }, CV_FONT_HERSHEY_PLAIN, 1, color_present);
			}

			return true;
		}


		void LocationEstimator::updatePlayerState(const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared, uint32_t idx)
		{

			if (std::abs(kernel[idx].lastCentroidX - shared.trackerData[idx].metricCP[0]) > parameter.moveThreshold/* || std::abs(lastCentroidZ[idx] - shared.trackerData[idx].metricCP[2]) > parameter.moveThreshold*/)
				kernel[idx].lastMove = std::chrono::high_resolution_clock::now();

			kernel[idx].lastCentroidX = shared.trackerData[idx].metricCP[0];
			kernel[idx].lastCentroidZ = shared.trackerData[idx].metricCP[2];


			if (shared.trackerData[idx].distance < (shared.environment.minDistance + parameter.offsetFront) ||
				shared.trackerData[idx].distance > (shared.environment.maxDistance - parameter.offsetBack))
				kernel[idx].outOfRange = true;
			else
				kernel[idx].outOfRange = false;

			/*
			if (selection[idx] != trackerID[idx])
			{
				trackerID[idx] = selection[idx];
				kernel[idx].present = !shared.trackerData[idx].contourIdx.empty();
				kernel[idx].ready = false;
			}
			else
			{
				kernel[idx].present = !shared.trackerData[idx].contourIdx.empty();
				kernel[idx].ready = kernel[idx].present && kernel[idx].ready ? true :
					((std::chrono::high_resolution_clock::now() - kernel[idx].lastMove) > std::chrono::duration<float, std::milli>(parameter.readyTimeMilli));
			}
			*/

			kernel[idx].present = !shared.trackerData[idx].contourIdx.empty();
			kernel[idx].ready = kernel[idx].present && kernel[idx].ready ? true :
				((std::chrono::high_resolution_clock::now() - kernel[idx].lastMove) > std::chrono::duration<float, std::milli>(parameter.readyTimeMilli));

		}


		void LocationEstimator::calculateLocationResult(const cv::Size& imageSize, const mc::structures::SharedData& shared,
			mc::structures::Result& result, uint32_t idx)
		{
			// we can also kind of filter these ...
			result.player[idx].location.centerX = shared.trackerData[idx].cp.x / imageSize.width;
			result.player[idx].location.centerZ = (shared.trackerData[idx].distance - shared.environment.minDistance) / (shared.environment.maxDistance - shared.environment.minDistance);

			result.player[idx].location.present = kernel[idx].present;
			result.player[idx].location.ready = kernel[idx].ready;
			result.player[idx].location.outOfRange = kernel[idx].outOfRange;
		}


		void LocationEstimator::writeSharedData(mc::structures::SharedData& shared, uint32_t idx)
		{
			shared.locationData[idx].present = kernel[idx].present;
			shared.locationData[idx].ready = kernel[idx].ready;
			shared.locationData[idx].outOfRange = kernel[idx].outOfRange;
		}


		void LocationEstimator::writeResult(mc::structures::Result& result, uint32_t idx)
		{
			// is may be not used ...
			// but we can think about something like this for mc::position::PositionEstimator, mc::activity::ActivityEstimator ...
		}

	}

}