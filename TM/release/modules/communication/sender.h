#pragma once

#include "structures.h"
#include "communication/utils.h"

#include <iostream>
#include <string>
#include <mutex>
#include <thread>
#include <vector>

#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"

namespace mc
{
	namespace sender
	{

		const size_t bufferSize(1024);


		// ==========================================================================================================================
		//
		// helper function to prepare the to be sent messages
		//
		// ==========================================================================================================================


		void findPlaceHolderPosition(const std::vector<std::string>& pattern, std::vector<size_t>& position, char placeholder);


		// ==========================================================================================================================
		//
		// sender object to send osc messages to CM
		//
		// ==========================================================================================================================


		class ControlModuleSender
		{
		public:

			ControlModuleSender(const mc::utils::NetworkConfiguration& config, const mc::utils::MessagePattern& list) : list(list),
				transmitSocket(IpEndpointName(config.ipCM.c_str(), std::stoi(config.portTM2CM))), packet(buffer, bufferSize)
			{
				findPlaceHolderPosition(list.pattern, position, mc::defines::placeholder);
			}

			void sendLoaded();

			void sendReady();

			void sendError(int error);

			void sendStopped();

		private:

			std::vector<size_t> position;

			mc::utils::MessagePattern list;

			//
			UdpTransmitSocket transmitSocket;
			char buffer[bufferSize];
			osc::OutboundPacketStream packet;
			//

		};


		// ==========================================================================================================================
		//
		// sender object to send osc messages to ME
		//
		// ==========================================================================================================================


		class MusicEnvironmentSender
		{
		public:

			MusicEnvironmentSender(const mc::utils::NetworkConfiguration& config, const mc::utils::MessagePattern& list) : list(list),
				transmitSocket(IpEndpointName(config.ipME.c_str(), std::stoi(config.portTM2ME))), packet(buffer, bufferSize)
			{
				findPlaceHolderPosition(list.pattern, position, mc::defines::placeholder);
			}

			void send(const mc::structures::MusicBundle& command, const mc::structures::Selection& selection, const mc::structures::Result& result);

		private:

			std::vector<size_t> position;

			mc::utils::MessagePattern list;

			mc::structures::Result oldResult;

			//
			UdpTransmitSocket transmitSocket;
			char buffer[bufferSize];
			osc::OutboundPacketStream packet;
			//


			bool sendOnChange(float cur, float old, float thresh);

			bool sendOnDifferentFromZero(float cur, float old);


			void sendPlayerResult(const mc::structures::MusicBundle& command, const mc::structures::Result& cur, const mc::structures::Result& old, size_t id, char cs);

			void sendZoneResult(const mc::structures::MusicBundle& command, const mc::structures::Result& cur, const mc::structures::Result& old, size_t id, char cs);



			void sendPlayerActivityResult(const mc::structures::MusicBundle& command, const mc::structures::Result& cur, const mc::structures::Result& old, size_t id, char cs);

			void sendPlayerLocationResult(const mc::structures::MusicBundle& command, const mc::structures::Result& cur, const mc::structures::Result& old, size_t id, char cs);

			void sendPlayerPositionResult(const mc::structures::MusicBundle& command, const mc::structures::Result& cur, const mc::structures::Result& old, size_t id, char cs);

			void sendPlayerGestureResult(const mc::structures::MusicBundle& command, const mc::structures::Result& cur, const mc::structures::Result& old, size_t id, char cs);

		};
	}
}
