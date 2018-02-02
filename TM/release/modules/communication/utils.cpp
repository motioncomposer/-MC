
#include "utils.h"

// rename this to network

namespace mc
{
	namespace utils
	{

		// ==========================================================================================================================
		//
		// network configuration object
		//
		// ==========================================================================================================================


		void NetworkConfiguration::read(const cv::FileNode & fn)
		{
			fn["ipCM"] >> ipCM;
			fn["ipME"] >> ipME;
			fn["ipTM"] >> ipTM;
			fn["portTM2CM"] >> portTM2CM;
			fn["portTM2ME"] >> portTM2ME;
			fn["portCM2TM"] >> portCM2TM;
			fn["portME2TM"] >> portME2TM;
		}


		void NetworkConfiguration::write(cv::FileStorage & fs) const
		{
			fs << "{"
				<< "ipCM" << ipCM
				<< "ipME" << ipME
				<< "ipTM" << ipTM
				<< "portTM2CM" << portTM2CM
				<< "portTM2ME" << portTM2ME
				<< "portCM2TM" << portCM2TM
				<< "portME2TM" << portME2TM
				<< "}";
		}


		void read(const cv::FileNode & fn, NetworkConfiguration & netConfig, const NetworkConfiguration & default)
		{
			if (fn.empty())
				netConfig = default;
			else
				netConfig.read(fn);
		}


		void write(cv::FileStorage & fs, const std::string &, const NetworkConfiguration & netConfig)
		{
			netConfig.write(fs);
		}


		bool saveNetworkConfiguration(const NetworkConfiguration& netConfig, const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::WRITE);

			if (fs.isOpened())
			{
				fs << key << netConfig;
				return true;
			}

			return false;
		}


		bool readNetworkConfiguration(NetworkConfiguration& netConfig, const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::READ);

			if (fs.isOpened())
			{
				fs[key] >> netConfig;
				return true;
			}

			return false;
		}





		// ==========================================================================================================================
		//
		// message pattern object
		//
		// ==========================================================================================================================


		void MessagePattern::read(const cv::FileNode & fn)
		{
			pattern.clear();

			size_t counter(0);
			std::string dmy;

			while (true)
			{
				fn[("item" + fs::core::withLeadingZeros(counter, mc::defines::enumeration))] >> dmy;
				if (dmy == std::string())
					break;
				pattern.push_back(dmy);
				++counter;
			}
		}


		void MessagePattern::write(cv::FileStorage & fs) const
		{
			fs << "{";

			size_t counter(0);
			for (auto& it : pattern)
			{
				fs << ("item" + fs::core::withLeadingZeros(counter, mc::defines::enumeration)) << it;
				++counter;
			}

			fs << "}";
		}
		

		void read(const cv::FileNode & fn, MessagePattern & messagePat, const MessagePattern & default)
		{
			if (fn.empty())
				messagePat = default;
			else
				messagePat.read(fn);
		}


		void write(cv::FileStorage & fs, const std::string &, const MessagePattern & messagePat)
		{
			messagePat.write(fs);
		}


		bool readMessagePattern(MessagePattern& messagePat, const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs;
			fs.open(filename, cv::FileStorage::READ);

			if (!fs.isOpened())
			{
				std::cout << "-- could not open file: " << filename << std::endl;
				return false;
			}

			fs[key] >> messagePat;
			fs.release();

			return true;
		}


		bool saveMessagePattern(const MessagePattern& messagePat, const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs;
			fs.open(filename, cv::FileStorage::WRITE);

			if (!fs.isOpened())
			{
				std::cout << "-- could not open file: " << filename << std::endl;
				return false;
			}

			fs << key << messagePat;
			fs.release();

			return true;
		}

	}
}