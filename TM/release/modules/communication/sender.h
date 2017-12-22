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

			// here we don't pass somehow the CMCommand ... but we can create a state result containing the current state of the tracking module
			// starting, loaded, initializing, ready, error + error code ...
			// we don't need to pass a CMCommandBundle as the CM can't request a specific sending behaviour
			// we can make this a send state function --> states: loaded, ready
			
			//
			void sendLoaded();

			void sendReady();


			//void sendState(int state_code);

			// may be we can hand in back the MCCommandBundle ... we may be should rename this in order to better represent the
			// communication structure ...

			// this is not used yet ...
			// we need to define a error code list ...
			void sendError(int err_code);

			// actually we can directly merge th two abouve functions ...
			// we can create the interface similar to MESender

			//void send();


			// normally TrackingModule does not communicate to controll module ... only in case of error


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

		// we are not having here a sending thread in here
		// do we need one?

		class MusicEnvironmentSender
		{
		public:

			MusicEnvironmentSender(const mc::utils::NetworkConfiguration& config, const mc::utils::MessagePattern& list) : list(list),
				transmitSocket(IpEndpointName(config.ipME.c_str(), std::stoi(config.portTM2ME))), packet(buffer, bufferSize)
			{
				findPlaceHolderPosition(list.pattern, position, mc::defines::placeholder);
			}

			void send(const mc::command::MECommandBundle& command, const mc::result::ResultBundle& result);

		private:

			std::vector<size_t> position;

			mc::utils::MessagePattern list;

			mc::result::ResultBundle oldResult;

			//
			UdpTransmitSocket transmitSocket;
			char buffer[bufferSize];
			osc::OutboundPacketStream packet;
			//


			// ==========================================================================================================================
			//
			// internal functions used for sending the result data
			//
			// ==========================================================================================================================


			bool sendOnChange(float cur, float old, float thresh);

			bool sendOnDifferentFromZero(float cur, float old);



			void sendPlayerResult(const mc::command::MECommandBundle& command, const mc::result::ResultBundle& cur, const mc::result::ResultBundle& old, size_t id);

			void sendZoneResult(const mc::command::MECommandBundle& command, const mc::result::ResultBundle& cur, const mc::result::ResultBundle& old, size_t id);



			void sendPlayerActivityResult(const mc::command::MECommandBundle& command, const mc::result::ResultBundle& cur, const mc::result::ResultBundle& old, size_t id);

			void sendPlayerLocationResult(const mc::command::MECommandBundle& command, const mc::result::ResultBundle& cur, const mc::result::ResultBundle& old, size_t id);

			void sendPlayerPositionResult(const mc::command::MECommandBundle& command, const mc::result::ResultBundle& cur, const mc::result::ResultBundle& old, size_t id);

			void sendPlayerGestureResult(const mc::command::MECommandBundle& command, const mc::result::ResultBundle& cur, const mc::result::ResultBundle& old, size_t id);

		};
	}
}
