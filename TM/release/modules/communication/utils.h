#pragma once

// this is only because of the verbose read and write operations ...
#include <iostream>

#include "opencv2/core/core.hpp"

#include <string>
#include <vector>

#include "core.h"

#include "defines.h"

namespace mc
{
	namespace utils
	{

		// ==========================================================================================================================
		//
		// network configuration object
		//
		// ==========================================================================================================================


		class NetworkConfiguration
		{
		public:
			NetworkConfiguration(const std::string ipCM = mc::defines::ip_localhost,
				const std::string ipME = mc::defines::ip_localhost,
				const std::string ipTM = mc::defines::ip_localhost,
				const std::string portTM2CM = mc::defines::port_TM_to_CM,
				const std::string portTM2ME = mc::defines::port_TM_to_ME,
				const std::string portCM2TM = mc::defines::port_TM_from_CM,
				const std::string portME2TM = mc::defines::port_TM_from_ME) : ipCM(ipCM),
				ipME(ipME),
				ipTM(ipTM),
				portTM2CM(portTM2CM),
				portTM2ME(portTM2ME),
				portCM2TM(portCM2TM),
				portME2TM(portME2TM)
			{}

			void read(const cv::FileNode& fn);

			void write(cv::FileStorage& fs) const;


			std::string ipCM;
			std::string ipME;
			std::string ipTM;

			std::string portTM2CM;
			std::string portTM2ME;

			std::string portCM2TM;
			std::string portME2TM;

		};


		void read(const cv::FileNode& fn, NetworkConfiguration& netConfig, const NetworkConfiguration& default = NetworkConfiguration());

		void write(cv::FileStorage& fs, const std::string&, const NetworkConfiguration& netConfig);

		bool readNetworkConfiguration(NetworkConfiguration& netConfig,
			const std::string& filename, const std::string& key);

		bool saveNetworkConfiguration(const NetworkConfiguration& netConfig,
			const std::string& filename, const std::string& key);


		// ==========================================================================================================================
		//
		// message pattern object
		//
		// ==========================================================================================================================


		class MessagePattern
		{
		public:

			MessagePattern(const std::string& prelude, const std::vector<std::string>& pattern)
			{
				MessagePattern::pattern.clear();
				MessagePattern::pattern.reserve(pattern.size());

				for (auto& it : pattern)
					MessagePattern::pattern.push_back(prelude + it);
			}


			MessagePattern(const std::vector<std::string>& pattern = {}) : pattern(pattern)
			{}

			void read(const cv::FileNode& fn);

			void write(cv::FileStorage& fs) const;


			std::vector<std::string> pattern;
		};


		void read(const cv::FileNode& fn, MessagePattern& messagePat, const MessagePattern& default = MessagePattern());

		void write(cv::FileStorage& fs, const std::string&, const MessagePattern& messagePat);

		bool readMessagePattern(MessagePattern& messagePat,
			const std::string& filename, const std::string& key);

		bool saveMessagePattern(const MessagePattern& messagePat,
			const std::string& filename, const std::string& key);

	}
}
