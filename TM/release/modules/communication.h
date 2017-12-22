#pragma once

#include <string>

#include "defines.h"
#include "structures.h"

#include "communication/utils.h"
#include "communication/sender.h"
#include "communication/receiver.h"


namespace mc
{


	namespace communication
	{


		// ==========================================================================================================================


		enum class CommunicationCoreError
		{
			NO_ERROR,
			READING_NETCONFIG_FAIL,
			READING_CM2TM_FAIL,
			READING_TM2CM_FAIL,
			READING_ME2TM_FAIL,
			READING_TM2ME_FAIL
		};


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

			CommunicationCoreFileAccess(const std::string& filenameNetworkConfiguration = mc::defines::filenameNetworkConfiguration,
				const std::string& keyNetworkConfiguration = mc::defines::keyNetworkConfiguration,
				const std::string& filenameCM2TM = mc::defines::filenameCM2TM,
				const std::string& keyCM2TM = mc::defines::keyCM2TM,
				const std::string& filenameTM2CM = mc::defines::filenameTM2CM,
				const std::string& keyTM2CM = mc::defines::keyTM2CM,
				const std::string& filenameME2TM = mc::defines::filenameME2TM,
				const std::string& keyME2TM = mc::defines::keyME2TM,
				const std::string& filenameTM2ME = mc::defines::filenameTM2ME,
				const std::string& keyTM2ME = mc::defines::keyTM2ME) : filenameNetworkConfiguration(filenameNetworkConfiguration),
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



			void notifyControlModule();

			//void readCommands();

			// this needs to be in "structures.h"
			void sendTrackingModuleState(const mc::test::TrackingModuleState& state);

			void sendResultBundle(const mc::result::ResultBundle& result);

		private:

			CommunicationCoreError state;

			mc::utils::NetworkConfiguration netConfig;

			mc::command::CMCommandBundle cmCommandBundle;
			std::shared_ptr<mc::receiver::ControlModuleReceiver> ptr_receiverFromCM;
			std::shared_ptr<mc::sender::ControlModuleSender> ptr_senderToCM;

			mc::command::MECommandBundle meCommandBundle;
			std::shared_ptr<mc::receiver::MusicEnvironmentReceiver> ptr_receiverFromME;
			std::shared_ptr<mc::sender::MusicEnvironmentSender> ptr_senderToME;

		};


		std::string getCommunicationCoreErrorString(const CommunicationCoreError& state);

	}
}
