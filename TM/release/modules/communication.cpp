
#include "communication.h"

namespace mc
{
	namespace communication
	{



		// ==========================================================================================================================


		std::string getCommunicationCoreErrorString(const CommunicationCoreError& state)
		{
			switch (state)
			{
			case CommunicationCoreError::CC_NO_ERROR:
				return{ "loading completed" };
				break;
			case CommunicationCoreError::CC_READING_NETCONFIG_FAIL:
				return{ "loading network configuration failed" };
				break;
			case CommunicationCoreError::CC_READING_CM2TM_FAIL:
				return{ "loading osc protocol (CM to TM) failed" };
				break;
			case CommunicationCoreError::CC_READING_TM2CM_FAIL:
				return{ "loading osc protocol (TM to CM) failed" };
				break;
			case CommunicationCoreError::CC_READING_ME2TM_FAIL:
				return{ "loading osc protocol (ME to TM) failed" };
				break;
			case CommunicationCoreError::CC_READING_TM2ME_FAIL:
				return{ "loading osc protocol (TM to ME) failed" };
				break;
			default:
				return{ "undefined error code" };
			}
		}


		// ==========================================================================================================================


		CommunicationCore::CommunicationCore(const CommunicationCoreFileAccess& access)
		{

			bool ok(true);

			//
			// read in the network configuration
			if (!mc::utils::readNetworkConfiguration(netConfig, access.filenameNetworkConfiguration, access.keyNetworkConfiguration))
			{
				state = CommunicationCoreError::CC_READING_NETCONFIG_FAIL;
				ok = false;
			}



			//
			// read in the message patter for receiving from CM and initialize receiver
			mc::utils::MessagePattern dmy;

			if (!ok || !mc::utils::readMessagePattern(dmy, access.filenameCM2TM, access.keyCM2TM))
			{
				state = ok ? CommunicationCoreError::CC_READING_CM2TM_FAIL : state;
				ok = false;
			}
			else
				receiverFromCM.reset(new mc::receiver::ControlModuleReceiver(netConfig, dmy, &controlBundle));



			//
			// read in the message pattern for sending to CM and initialize sender
			if (!ok || !mc::utils::readMessagePattern(dmy, access.filenameTM2CM, access.keyTM2CM))
			{
				state = ok ? CommunicationCoreError::CC_READING_TM2CM_FAIL : state;
				ok = false;
			}
			else
				senderToCM.reset(new mc::sender::ControlModuleSender(netConfig, dmy));



			//
			// read in the message patter for receiving from ME and initialize receiver
			if (!ok || !mc::utils::readMessagePattern(dmy, access.filenameME2TM, access.keyME2TM))
			{
				state = ok ? CommunicationCoreError::CC_READING_ME2TM_FAIL : state;
				ok = false;
			}
			else
				receiverFromME.reset(new mc::receiver::MusicEnvironmentReceiver(netConfig, dmy, &musicBundle));



			//
			// read in the message pattern for sending to ME and initialize sender
			if (!ok || !mc::utils::readMessagePattern(dmy, access.filenameTM2ME, access.keyTM2ME))
			{
				state = ok ? CommunicationCoreError::CC_READING_TM2ME_FAIL : state;
				ok = false;
			}
			else
				senderToME.reset(new mc::sender::MusicEnvironmentSender(netConfig, dmy));



			//
			// if no error, set state to loaded
			if (ok)
				state = CommunicationCoreError::CC_NO_ERROR;


			/*
			*
			* at this position, we can setup the default values of the CMCommandBundle
			* in order to have a specific init player
			*
			*/
			
		}


		bool CommunicationCore::isReady() const
		{
			return state == CommunicationCoreError::CC_NO_ERROR;
		}


		CommunicationCoreError CommunicationCore::getStateCode() const
		{
			return state;
		}


		void CommunicationCore::notifyLoaded()
		{
			senderToCM->sendLoaded();
		}


		void CommunicationCore::notifyReady()
		{
			senderToCM->sendReady();
		}


		void CommunicationCore::notifyStopped()
		{
			senderToCM->sendStopped();
		}


		void CommunicationCore::waitForStateUpdate()
		{
			std::unique_lock<std::mutex> lck(controlBundle.mtx);
			while (controlBundle.state == 0)
				controlBundle.cond.wait(lck);
		}


		void CommunicationCore::sendError(int error)
		{
			senderToCM->sendError(error);
		}


		void CommunicationCore::sendResult(const mc::structures::Selection& selection, const mc::structures::Result& result)
		{
			senderToME->send(musicBundle, selection, result);
		}


		void CommunicationCore::updateActivation(mc::structures::Activation& activation)
		{
			std::lock_guard<std::mutex> guard(controlBundle.mtx);

			for (auto&& c = 0; c < 2; ++c)
				activation.player[c] = controlBundle.player[c].tracking;

			for (auto&& c = 0; c < 6; ++c)
				activation.zone[c] = controlBundle.zone[c].tracking;
		}


		void CommunicationCore::updateSelection(mc::structures::Selection& selection)
		{
			std::lock_guard<std::mutex> guard(controlBundle.mtx);

			if (controlBundle.player[0].blob == std::numeric_limits<int32_t>::max() && controlBundle.player[1].blob == std::numeric_limits<int32_t>::max())
			{
				selection.time[0] = std::chrono::high_resolution_clock::now();
				selection.time[1] = selection.time[0];

				std::swap(selection.player[0], selection.player[1]);
			}


			if (controlBundle.player[0].time > selection.time[0])
			{
				selection.time[0] = std::chrono::high_resolution_clock::now();
				selection.player[0] = controlBundle.player[0].blob;
			}


			if (controlBundle.player[1].time > selection.time[1])
			{
				selection.time[1] = std::chrono::high_resolution_clock::now();
				selection.player[1] = controlBundle.player[1].blob;
			}


			if (selection.player[0] < 0 && selection.player[1] >= 0)
			{
				std::swap(selection.time[0], selection.time[1]);
				std::swap(selection.player[0], selection.player[1]);
			}


			for (auto&& c = 0; c < 6; ++c)
				selection.zone[c] = controlBundle.zone[c].blob;
		}


		bool CommunicationCore::keepRunning()
		{
			std::lock_guard<std::mutex> guard(controlBundle.mtx);
			return controlBundle.state == 0;
		}


		bool CommunicationCore::initRequested()
		{
			std::lock_guard<std::mutex> guard(controlBundle.mtx);
			return controlBundle.state > 0;
		}


		bool CommunicationCore::stopRequested()
		{
			std::lock_guard<std::mutex> guard(controlBundle.mtx);
			return controlBundle.state < 0;
		}


		void CommunicationCore::resetState()
		{
			std::lock_guard<std::mutex> guard(controlBundle.mtx);
			controlBundle.state = 0;
		}


		// ==========================================================================================================================


	}
}