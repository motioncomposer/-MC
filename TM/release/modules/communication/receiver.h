#pragma once

#include "structures.h"

#include "communication/utils.h"

#include <iostream>
#include <string>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>
#include <functional>

#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"


namespace mc
{
	namespace receiver
	{

		// ==========================================================================================================================
		//
		// helper function to interpret the incoming messages
		//
		// ==========================================================================================================================


		size_t getOffset(const std::vector<std::string>& pattern, char delimiter);

		size_t checkSample(const std::string& sample, const std::string& pattern, int32_t prelude, char delimiter, char placeholder);


		// ==========================================================================================================================
		//
		// CM handler functions
		//
		// ==========================================================================================================================


		void handleInitRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t);


		void handleStopRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t);


		void handlePlayerBlobRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);


		void handlePlayerTrackingRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);


		void handlePlayerResetRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);


		void handleZoneBlobRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone);


		void handleZoneTrackingRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone);


		void handlePlayerSwapRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);


		void handleTrackingRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t idx);


		// here is still the sensitivity and the environment missing


		// ==========================================================================================================================
		//
		// listener object that maps the received osc messages to CM handler functions
		//
		// ==========================================================================================================================


		class ControlModuleListener : public osc::OscPacketListener
		{

		public:

			ControlModuleListener(const mc::utils::MessagePattern& list, mc::structures::ControlBundle* ref) : list(list),
				controllerRef(ref), error(0)
			{

				offset = getOffset(list.pattern, mc::defines::delimiter);

				handlers.clear();
				handlers.reserve(list.pattern.size());

				handlers.push_back(std::bind(handleInitRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handleStopRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerBlobRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerTrackingRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerResetRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handleZoneBlobRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handleZoneTrackingRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerSwapRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handleTrackingRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));

			}

		protected:

			virtual void ProcessMessage(const osc::ReceivedMessage& message,
				const IpEndpointName& remoteEndpoint);

		private:

			size_t error;

			size_t offset;

			mc::utils::MessagePattern list;

			std::vector<std::function<void(osc::ReceivedMessageArgumentStream, size_t)>> handlers;

			mc::structures::ControlBundle* controllerRef;
		};


		// ==========================================================================================================================
		//
		// receiver object that handles the incoming osc messages from CM
		//
		// ==========================================================================================================================


		class ControlModuleReceiver
		{
		public:

			ControlModuleReceiver(const mc::utils::NetworkConfiguration& config, const mc::utils::MessagePattern& list, mc::structures::ControlBundle* ref) : listener(list, ref),
				port(std::stoi(config.portCM2TM))
			{
				workerThread = std::thread(&ControlModuleReceiver::receiveFromNetwork, this);
			}

			~ControlModuleReceiver()
			{
				socket->AsynchronousBreak();
				workerThread.join();
			}

		private:

			int port;
			ControlModuleListener listener;
			std::unique_ptr<UdpListeningReceiveSocket> socket;
			std::thread workerThread;

			void receiveFromNetwork();
		};


		// ==========================================================================================================================
		//
		// ME handler functions
		//
		// ==========================================================================================================================


		void handlePlayerDiscreteHandLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);


		void handlePlayerDiscreteHandRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);


		void handlePlayerDiscreteHeadRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);


		void handlePlayerDiscreteLegLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);


		void handlePlayerDiscreteLegRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerDiscreteBodyUpperRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerDiscreteBodyLowerRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerDiscreteBodyLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerDiscreteBodyRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);


		void handlePlayerNormalHandLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerNormalHandRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerNormalHeadRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerNormalLegLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerNormalLegRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerNormalBodyUpperRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerNormalBodyLowerRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerNormalBodyLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerNormalBodyRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);


		void handlePlayerPeakRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);


		void handlePlayerFlowLeftwardsLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerFlowLeftwardsRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerFlowRightwardsLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerFlowRightwardsRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerFlowUpwardsLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerFlowUpwardsRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerFlowDownwardsLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerFlowDownwardsRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);


		void handlePlayerLocationReadyRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerLocationPresentRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerLocationCenterXRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerLocationCenterZRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerLocationOutOfRangeRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);


		void handlePlayerPositionHeightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerPositionHeightLevelRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerPositionVerticalHandLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerPositionVerticalHandRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerPositionSideHandLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerPositionSideHandRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerPositionSideFootLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerPositionSideFootRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerPositionFrontHandLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerPositionFrontHandRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerPositionFrontFootLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerPositionFrontFootRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerPositionWidthRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);


		void handlePlayerGestureHitOverheadRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerGestureHitSideLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerGestureHitSideRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerGestureHitDownLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerGestureHitDownRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerGestureHitForwardLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerGestureHitForwardRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerGestureKickSideLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerGestureKickSideRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerGestureKickForwardLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerGestureKickForwardRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerGestureDoubleArmSideRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerGestureDoubleArmSideCloseRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerGestureJumpRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);
		

		void handlePlayerGestureClapRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player);


		void handleZoneDiscreteRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone);


		void handleZoneNormalRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone);
		
		
		void handleZoneFlowLeftwardsRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone);
		
		
		void handleZoneFlowRightwardsRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone);
		
		
		void handleZoneFlowUpwardsRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone);
		
		
		void handleZoneFlowDownwardsRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone);

		
		// ==========================================================================================================================
		//
		// listener object that maps the received osc messages to ME handler functions
		//
		// ==========================================================================================================================


		class MusicEnvironmentListener : public osc::OscPacketListener
		{

		public:

			MusicEnvironmentListener(const mc::utils::MessagePattern& list, mc::structures::MusicBundle* ref) : list(list),
				controllerRef(ref),
				error(0)
			{

				offset = getOffset(list.pattern, mc::defines::delimiter);
				
				handlers.clear();
				handlers.reserve(list.pattern.size());

				handlers.push_back(std::bind(handlePlayerDiscreteHandLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerDiscreteHandRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerDiscreteHeadRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerDiscreteLegLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerDiscreteLegRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerDiscreteBodyUpperRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerDiscreteBodyLowerRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerDiscreteBodyLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerDiscreteBodyRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerNormalHandLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerNormalHandRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerNormalHeadRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerNormalLegLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerNormalLegRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerNormalBodyUpperRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerNormalBodyLowerRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerNormalBodyLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerNormalBodyRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerPeakRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerFlowLeftwardsLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerFlowLeftwardsRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerFlowRightwardsLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerFlowRightwardsRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerFlowUpwardsLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerFlowUpwardsRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerFlowDownwardsLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerFlowDownwardsRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerLocationReadyRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerLocationPresentRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerLocationCenterXRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerLocationCenterZRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerLocationOutOfRangeRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerPositionHeightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerPositionHeightLevelRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerPositionVerticalHandLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerPositionVerticalHandRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerPositionSideHandLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerPositionSideHandRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerPositionSideFootLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerPositionSideFootRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerPositionFrontHandLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerPositionFrontHandRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerPositionFrontFootLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerPositionFrontFootRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerPositionWidthRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerGestureHitOverheadRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerGestureHitSideLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerGestureHitSideRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerGestureHitDownLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerGestureHitDownRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerGestureHitForwardLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerGestureHitForwardRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerGestureKickSideLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerGestureKickSideRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerGestureKickForwardLeftRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerGestureKickForwardRightRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerGestureDoubleArmSideRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerGestureDoubleArmSideCloseRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerGestureJumpRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handlePlayerGestureClapRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handleZoneDiscreteRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handleZoneNormalRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handleZoneFlowLeftwardsRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handleZoneFlowRightwardsRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handleZoneFlowUpwardsRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));
				handlers.push_back(std::bind(handleZoneFlowDownwardsRequest, controllerRef, std::placeholders::_1, std::placeholders::_2));

			}

		protected:

			virtual void ProcessMessage(const osc::ReceivedMessage& message,
				const IpEndpointName& remoteEndpoint);

		private:

			size_t error;

			size_t offset;

			mc::utils::MessagePattern list;

			std::vector<std::function<void(osc::ReceivedMessageArgumentStream, size_t)>> handlers;

			mc::structures::MusicBundle* controllerRef;
		};


		// ==========================================================================================================================
		//
		// receiver object that handles the incoming osc messages from ME
		//
		// ==========================================================================================================================


		class MusicEnvironmentReceiver
		{
		public:

			MusicEnvironmentReceiver(const mc::utils::NetworkConfiguration& config, const mc::utils::MessagePattern& list, mc::structures::MusicBundle* ref) : listener(list, ref),
				port(std::stoi(config.portME2TM))
			{
				workerThread = std::thread(&MusicEnvironmentReceiver::receiveFromNetwork, this);
			}

			~MusicEnvironmentReceiver()
			{
				socket->AsynchronousBreak();
				workerThread.join();
			}

		private:

			int port;
			MusicEnvironmentListener listener;
			std::unique_ptr<UdpListeningReceiveSocket> socket;
			std::thread workerThread;

			void receiveFromNetwork();

		};
	}
}