#pragma once

#include <string>
#include <memory>
#include <mutex>

#include "communication/utils.h"
#include "communication/sender.h"
#include "communication/receiver.h"

#include "defines.h"
#include "structures.h"


namespace mc
{


	namespace communication
	{



		// ==========================================================================================================================


		enum class CommunicationCoreError
		{
			CC_NO_ERROR,
			CC_READING_NETCONFIG_FAIL,
			CC_READING_CM2TM_FAIL,
			CC_READING_TM2CM_FAIL,
			CC_READING_ME2TM_FAIL,
			CC_READING_TM2ME_FAIL
		};


		// ==========================================================================================================================


		std::string getCommunicationCoreErrorString(const CommunicationCoreError& state);


		// ==========================================================================================================================


		struct CommunicationCoreFileAccess
		{
			std::string filenameNetworkConfiguration;
			std::string keyNetworkConfiguration;

			std::string filenameCM2TM;
			std::string keyCM2TM;
			std::string filenameTM2CM;
			std::string keyTM2CM;

			std::string filenameME2TM;
			std::string keyME2TM;
			std::string filenameTM2ME;
			std::string keyTM2ME;

			CommunicationCoreFileAccess(const std::string& filenameNetworkConfiguration,
				const std::string& keyNetworkConfiguration,
				const std::string& filenameCM2TM,
				const std::string& keyCM2TM,
				const std::string& filenameTM2CM,
				const std::string& keyTM2CM,
				const std::string& filenameME2TM,
				const std::string& keyME2TM,
				const std::string& filenameTM2ME,
				const std::string& keyTM2ME) : filenameNetworkConfiguration(filenameNetworkConfiguration),
				keyNetworkConfiguration(keyNetworkConfiguration),
				filenameCM2TM(filenameCM2TM),
				keyCM2TM(keyCM2TM),
				filenameTM2CM(filenameTM2CM),
				keyTM2CM(keyTM2CM),
				filenameME2TM(filenameME2TM),
				keyME2TM(keyME2TM),
				filenameTM2ME(filenameTM2ME),
				keyTM2ME(keyTM2ME)
			{}

		};


		// ==========================================================================================================================


		class CommunicationCore
		{
		public:


			CommunicationCore(const CommunicationCoreFileAccess& access);

			bool isReady() const;

			CommunicationCoreError getStateCode() const;


			void notifyLoaded();

			void notifyReady();

			void notifyStopped();			


			void waitForStateUpdate();


			void sendError(int error);

			void sendResult(const mc::structures::Result& result);


			void updateActivation(mc::structures::Activation& activation);

			void updateSelection(mc::structures::Selection& selection);


			bool keepRunning();

			bool initRequested();

			bool stopRequested();


			void resetState();


		private:

			CommunicationCoreError state;

			mc::utils::NetworkConfiguration netConfig;


			mc::structures::ControlBundle controlBundle;
			std::shared_ptr<mc::receiver::ControlModuleReceiver> receiverFromCM;
			std::shared_ptr<mc::sender::ControlModuleSender> senderToCM;

			mc::structures::MusicBundle musicBundle;
			std::shared_ptr<mc::receiver::MusicEnvironmentReceiver> receiverFromME;
			std::shared_ptr<mc::sender::MusicEnvironmentSender> senderToME;

		};


		// ==========================================================================================================================


	}
}
