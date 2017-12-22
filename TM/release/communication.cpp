
#include "communication.h"

namespace mc
{
	namespace communication
	{

		// ==========================================================================================================================

		CommunicationCore::CommunicationCore(const CommunicationCoreFileAccess& access)
		{

			bool ok(true);

			//
			// read in the network configuration
			if (!mc::utils::readNetworkConfiguration(netConfig, access.filenameNetworkConfiguration, access.keyNetworkConfiguration))
			{
				ok = false;
				CommunicationCoreError::READING_NETCONFIG_FAIL;
			}



			//
			// read in the message patter for receiving from CM and initialize receiver
			mc::utils::MessagePattern dmy;

			if (!ok || !mc::utils::readMessagePattern(dmy, access.filenameCM2TM, access.keyCM2TM))
			{
				ok = false;
				CommunicationCoreError::READING_CM2TM_FAIL;
			}
			else
				ptr_receiverFromCM.reset(new mc::receiver::ControlModuleReceiver(netConfig, dmy, &cmCommandBundle));



			//
			// read in the message pattern for sending to CM and initialize sender
			if (!ok || !mc::utils::readMessagePattern(dmy, access.filenameTM2CM, access.keyTM2CM))
			{
				ok = false;
				CommunicationCoreError::READING_TM2CM_FAIL;
			}
			else
				ptr_senderToCM.reset(new mc::sender::ControlModuleSender(netConfig, dmy));



			//
			// read in the message patter for receiving from ME and initialize receiver
			if (!ok || !mc::utils::readMessagePattern(dmy, access.filenameME2TM, access.keyME2TM))
			{
				ok = false;
				CommunicationCoreError::READING_ME2TM_FAIL;
			}
			else
				ptr_receiverFromME.reset(new mc::receiver::MusicEnvironmentReceiver(netConfig, dmy, &meCommandBundle));



			//
			// read in the message pattern for sending to ME and initialize sender
			if (!ok || !mc::utils::readMessagePattern(dmy, access.filenameTM2ME, access.keyTM2ME))
			{
				ok = false;
				CommunicationCoreError::READING_TM2ME_FAIL;
			}
			else
				ptr_senderToME.reset(new mc::sender::MusicEnvironmentSender(netConfig, dmy));



			//
			// if no error, set state to loaded
			if (ok)
				state = CommunicationCoreError::NO_ERROR;
		}


		bool CommunicationCore::isReady() const
		{
			return state == CommunicationCoreError::NO_ERROR;
		}


		CommunicationCoreError CommunicationCore::getStateCode() const
		{
			return state;
		}


		void CommunicationCore::notifyControlModule()
		{
			ptr_senderToCM->sendLoaded();
		}


		//void readCommands();

		// this needs to be in "structures.h"
		void CommunicationCore::sendTrackingModuleState(const mc::test::TrackingModuleState& state)
		{
			ptr_senderToCM->sendReady();
		}

		void CommunicationCore::sendResultBundle(const mc::result::ResultBundle& result)
		{
			ptr_senderToME->send(meCommandBundle, result);
		}


		// ==========================================================================================================================


		std::string getCommunicationCoreErrorString(const CommunicationCoreError& state)
		{
			switch (state)
			{
			case CommunicationCoreError::NO_ERROR:
				return{ "loading completed" };
				break;
			case CommunicationCoreError::READING_NETCONFIG_FAIL:
				return{ "loading network configuration failed" };
				break;
			case CommunicationCoreError::READING_CM2TM_FAIL:
				return{ "loading osc protocol (CM to TM) failed" };
				break;
			case CommunicationCoreError::READING_TM2CM_FAIL:
				return{ "loading osc protocol (TM to CM) failed" };
				break;
			case CommunicationCoreError::READING_ME2TM_FAIL:
				return{ "loading osc protocol (ME to TM) failed" };
				break;
			case CommunicationCoreError::READING_TM2ME_FAIL:
				return{ "loading osc protocol (TM to ME) failed" };
				break;
			default:
				return{ "undefined error code" };
			}
		}
	}
}