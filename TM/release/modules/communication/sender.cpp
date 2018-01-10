
#include "sender.h"

namespace mc
{
	namespace sender
	{

		// ==========================================================================================================================
		//
		// helper function to prepare the to be sent messages
		//
		// ==========================================================================================================================


		void findPlaceHolderPosition(const std::vector<std::string>& pattern, std::vector<size_t>& position, char placeholder)
		{
			position.clear();
			position.reserve(pattern.size());

			for (auto& it : pattern)
				position.push_back(it.find_first_of(placeholder));
		}


		// ==========================================================================================================================
		//
		// sender object to send osc messages to CM
		//
		// ==========================================================================================================================


		// actually ... these 3 functions are all we need ... they correspond to the internal function sof the ME sender

		// so there is actually no need for doing it more complicated

		// we can make this stuff private function ...

		// and have a public send function that takes an tracking state structure in order to decide what to send ...

		void ControlModuleSender::sendLoaded()
		{
			packet << osc::BeginMessage(list.pattern[0].c_str()) << osc::EndMessage;
			transmitSocket.Send(packet.Data(), packet.Size());
			packet.Clear();
		}


		void ControlModuleSender::sendReady()
		{
			packet << osc::BeginMessage(list.pattern[1].c_str()) << osc::EndMessage;
			transmitSocket.Send(packet.Data(), packet.Size());
			packet.Clear();
		}


		void ControlModuleSender::sendError(int err_code)
		{
			packet << osc::BeginMessage(list.pattern[2].c_str()) << err_code << osc::EndMessage;
			transmitSocket.Send(packet.Data(), packet.Size());
			packet.Clear();
		}


		// ==========================================================================================================================
		//
		// sender object to send osc messages to ME
		//
		// ==========================================================================================================================


		void MusicEnvironmentSender::send(const mc::command::MECommandBundle& command, const mc::result::ResultBundle& result)
		{

			for (auto&& c = 0; c < mc::defines::maxPlayer; ++c)
				sendPlayerResult(command, result, oldResult, c);


			for (auto&& c = 0; c < mc::defines::maxZones; ++c)
				sendZoneResult(command, result, oldResult, c);


			oldResult = result;
		}


		// ==========================================================================================================================
		//
		// internal functions used for sending the result data
		//
		// ==========================================================================================================================


		bool MusicEnvironmentSender::sendOnChange(float cur, float old, float thresh)
		{
			return (std::abs(cur - old) > thresh);
		}


		bool MusicEnvironmentSender::sendOnDifferentFromZero(float cur, float old)
		{
			return (cur != 0.f || old != 0.f);
		}



		void MusicEnvironmentSender::sendPlayerResult(const mc::command::MECommandBundle& command, const mc::result::ResultBundle& cur, const mc::result::ResultBundle& old, size_t id)
		{
			sendPlayerActivityResult(command, cur, old, id);
			sendPlayerLocationResult(command, cur, old, id);
			sendPlayerPositionResult(command, cur, old, id);
			sendPlayerGestureResult(command, cur, old, id);
		}


		void MusicEnvironmentSender::sendZoneResult(const mc::command::MECommandBundle& command, const mc::result::ResultBundle& cur, const mc::result::ResultBundle& old, size_t id)
		{

			if (command.zoneCommands[id].sendDiscrete && cur.zoneResults[id].discrete)
			{
				list.pattern[60][position[60]] = static_cast<char>(id) - 48 + 1;
				packet << osc::BeginMessage(list.pattern[60].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.zoneCommands[id].sendNormal &&
				sendOnDifferentFromZero(cur.zoneResults[id].normal, old.zoneResults[id].normal))
			{
				list.pattern[61][position[61]] = static_cast<char>(id) - 48 + 1;
				packet << osc::BeginMessage(list.pattern[61].c_str()) << cur.zoneResults[id].normal << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.zoneCommands[id].sendFlowLeftwards &&
				sendOnDifferentFromZero(cur.zoneResults[id].flowLeftwards, old.zoneResults[id].flowLeftwards))
			{
				list.pattern[62][position[62]] = static_cast<char>(id) - 48 + 1;
				packet << osc::BeginMessage(list.pattern[62].c_str()) << cur.zoneResults[id].flowLeftwards << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.zoneCommands[id].sendFlowRightwards &&
				sendOnDifferentFromZero(cur.zoneResults[id].flowRightwards, old.zoneResults[id].flowRightwards))
			{
				list.pattern[63][position[63]] = static_cast<char>(id) - 48 + 1;
				packet << osc::BeginMessage(list.pattern[63].c_str()) << cur.zoneResults[id].flowRightwards << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.zoneCommands[id].sendFlowUpwards &&
				sendOnDifferentFromZero(cur.zoneResults[id].flowUpwards, old.zoneResults[id].flowUpwards))
			{
				list.pattern[64][position[64]] = static_cast<char>(id) - 48 + 1;
				packet << osc::BeginMessage(list.pattern[64].c_str()) << cur.zoneResults[id].flowUpwards << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.zoneCommands[id].sendFlowDownwards &&
				sendOnDifferentFromZero(cur.zoneResults[id].flowDownwards, old.zoneResults[id].flowDownwards))
			{
				list.pattern[65][position[65]] = static_cast<char>(id) - 48 + 1;
				packet << osc::BeginMessage(list.pattern[65].c_str()) << cur.zoneResults[id].flowDownwards << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}
		}



		void MusicEnvironmentSender::sendPlayerActivityResult(const mc::command::MECommandBundle& command, const mc::result::ResultBundle& cur, const mc::result::ResultBundle& old, size_t id)
		{

			// ==========================================================================================================================
			// activity discrete
			// ==========================================================================================================================


			if (command.playerCommands[id].sendDiscreteHandLeft && cur.playerResults[id].activity.discrete.handLeft)
			{
				list.pattern[0][position[0]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[0].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendDiscreteHandRight && cur.playerResults[id].activity.discrete.handRight)
			{
				list.pattern[1][position[1]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[1].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendDiscreteHead && cur.playerResults[id].activity.discrete.head)
			{
				list.pattern[2][position[2]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[2].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendDiscreteLegLeft && cur.playerResults[id].activity.discrete.legLeft)
			{
				list.pattern[3][position[3]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[3].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendDiscreteLegRight && cur.playerResults[id].activity.discrete.legRight)
			{
				list.pattern[4][position[4]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[4].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendDiscreteBodyUpper && cur.playerResults[id].activity.discrete.bodyUpper)
			{
				list.pattern[5][position[5]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[5].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendDiscreteBodyLower && cur.playerResults[id].activity.discrete.bodyLower)
			{
				list.pattern[6][position[6]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[6].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendDiscreteBodyLeft && cur.playerResults[id].activity.discrete.bodyLeft)
			{
				list.pattern[7][position[7]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[7].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendDiscreteBodyRight && cur.playerResults[id].activity.discrete.bodyRight)
			{
				list.pattern[8][position[8]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[8].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}


			// ==========================================================================================================================
			// activity normal
			// ==========================================================================================================================


			if (command.playerCommands[id].sendNormalHandLeft &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.normal.handLeft, old.playerResults[id].activity.normal.handLeft))
			{
				list.pattern[9][position[9]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[9].c_str()) << cur.playerResults[id].activity.normal.handLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendNormalHandRight &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.normal.handRight, old.playerResults[id].activity.normal.handRight))
			{
				list.pattern[10][position[10]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[10].c_str()) << cur.playerResults[id].activity.normal.handRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendNormalHead &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.normal.head, old.playerResults[id].activity.normal.head))
			{
				list.pattern[11][position[11]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[11].c_str()) << cur.playerResults[id].activity.normal.head << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendNormalLegLeft &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.normal.legLeft, old.playerResults[id].activity.normal.legLeft))
			{
				list.pattern[12][position[12]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[12].c_str()) << cur.playerResults[id].activity.normal.legLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendNormalLegRight &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.normal.legRight, old.playerResults[id].activity.normal.legRight))
			{
				list.pattern[13][position[13]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[13].c_str()) << cur.playerResults[id].activity.normal.legRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendNormalBodyUpper &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.normal.bodyUpper, old.playerResults[id].activity.normal.bodyUpper))
			{
				list.pattern[14][position[14]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[14].c_str()) << cur.playerResults[id].activity.normal.bodyUpper << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendNormalBodyLower &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.normal.bodyLower, old.playerResults[id].activity.normal.bodyLower))
			{
				list.pattern[15][position[15]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[15].c_str()) << cur.playerResults[id].activity.normal.bodyLower << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendNormalBodyLeft &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.normal.bodyLeft, old.playerResults[id].activity.normal.bodyLeft))
			{
				list.pattern[16][position[16]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[16].c_str()) << cur.playerResults[id].activity.normal.bodyLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendNormalBodyRight &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.normal.bodyRight, old.playerResults[id].activity.normal.bodyRight))
			{
				list.pattern[17][position[17]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[17].c_str()) << cur.playerResults[id].activity.normal.bodyRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}


			////////////////////////////////////////////////////

			// request if peak should be moved to position because it fits better there ...
			// in tech spec it is still in activity ...
			// in this source code, it is already located in position
			// but not (yet) in the me command ...
			if (command.playerCommands[id].sendPeak &&
				sendOnDifferentFromZero(cur.playerResults[id].position.peak, old.playerResults[id].position.peak))
			{
				list.pattern[18][position[18]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[18].c_str()) << cur.playerResults[id].position.peak << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}
			//////////////////////////////////////////////////


			// ==========================================================================================================================
			// activity flow
			// ==========================================================================================================================


			if (command.playerCommands[id].sendFlowLeftwardsLeft &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.flow.leftwardsLeft, old.playerResults[id].activity.flow.leftwardsLeft))
			{
				list.pattern[19][position[19]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[19].c_str()) << cur.playerResults[id].activity.flow.leftwardsLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendFlowLeftwardsRight &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.flow.leftwardsRight, old.playerResults[id].activity.flow.leftwardsRight))
			{
				list.pattern[20][position[20]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[20].c_str()) << cur.playerResults[id].activity.flow.leftwardsRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendFlowRightwardsLeft &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.flow.rightwardsLeft, old.playerResults[id].activity.flow.rightwardsLeft))
			{
				list.pattern[21][position[21]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[21].c_str()) << cur.playerResults[id].activity.flow.rightwardsLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendFlowRightwardsRight &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.flow.rightwardsRight, old.playerResults[id].activity.flow.rightwardsRight))
			{
				list.pattern[22][position[22]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[22].c_str()) << cur.playerResults[id].activity.flow.rightwardsRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendFlowUpwardsLeft &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.flow.upwardsLeft, old.playerResults[id].activity.flow.upwardsLeft))
			{
				list.pattern[23][position[23]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[23].c_str()) << cur.playerResults[id].activity.flow.upwardsLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendFlowUpwardsRight &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.flow.upwardsRight, old.playerResults[id].activity.flow.upwardsRight))
			{
				list.pattern[24][position[24]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[24].c_str()) << cur.playerResults[id].activity.flow.upwardsRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendFlowDownwardsLeft &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.flow.downwardsLeft, old.playerResults[id].activity.flow.downwardsLeft))
			{
				list.pattern[25][position[25]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[25].c_str()) << cur.playerResults[id].activity.flow.downwardsLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendFlowDownwardsRight &&
				sendOnDifferentFromZero(cur.playerResults[id].activity.flow.downwardsRight, old.playerResults[id].activity.flow.downwardsRight))
			{
				list.pattern[26][position[26]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[26].c_str()) << cur.playerResults[id].activity.flow.downwardsRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}

		}


		void MusicEnvironmentSender::sendPlayerLocationResult(const mc::command::MECommandBundle& command, const mc::result::ResultBundle& cur, const mc::result::ResultBundle& old, size_t id)
		{
			if (command.playerCommands[id].sendLocationReady && cur.playerResults[id].location.ready)
			{
				list.pattern[27][position[27]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[27].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}			
			
			if (command.playerCommands[id].sendLocationPresent && cur.playerResults[id].location.present)
			{
				list.pattern[28][position[28]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[28].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}			
			
			if (command.playerCommands[id].sendLocationCenterX && sendOnChange(cur.playerResults[id].location.centerX, old.playerResults[id].location.centerX, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[29][position[29]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[29].c_str()) << cur.playerResults[id].location.centerX << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}
			
			if (command.playerCommands[id].sendLocationCenterZ && sendOnChange(cur.playerResults[id].location.centerZ, old.playerResults[id].location.centerZ, 100.f))
			{
				list.pattern[30][position[30]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[30].c_str()) << cur.playerResults[id].location.centerZ << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}


			if (command.playerCommands[id].sendLocationOutOfRange && cur.playerResults[id].location.outOfRange)
			{
				// this is not a bool, but we need to send a string in oder to nidicate if person is to close or to far away
				// we can internally have to bools: too close, too far, and this is only sended as outOfRange with a string ...
				// we need to provide a metric threshold
				//list.pattern[31][position[31]] = static_cast<char>(id) - 48;
				//packet << osc::BeginMessage(list.pattern[31].c_str()) << cur.playerResults[id].position.positionHeight << osc::EndMessage;
				//transmitSocket.Send(packet.Data(), packet.Size());
				//packet.Clear();
			}


		}


		void MusicEnvironmentSender::sendPlayerPositionResult(const mc::command::MECommandBundle& command, const mc::result::ResultBundle& cur, const mc::result::ResultBundle& old, size_t id)
		{


			if (command.playerCommands[id].sendPositionHeight && sendOnChange(cur.playerResults[id].position.height, old.playerResults[id].position.height, 2.f / mc::defines::operatingHeight))
			{
				list.pattern[32][position[32]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[32].c_str()) << cur.playerResults[id].position.height << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendPositionHeightLevel && sendOnChange(cur.playerResults[id].position.heightLevel, old.playerResults[id].position.heightLevel, 1))
			{
				list.pattern[33][position[33]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[33].c_str()) << cur.playerResults[id].position.heightLevel << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendPositionVerticalHandLeft && sendOnChange(cur.playerResults[id].position.verticalHandLeft, old.playerResults[id].position.verticalHandLeft, 2.f / mc::defines::operatingHeight))
			{
				list.pattern[34][position[34]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[34].c_str()) << cur.playerResults[id].position.verticalHandLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendPositionVerticalHandRight && sendOnChange(cur.playerResults[id].position.verticalHandRight, old.playerResults[id].position.verticalHandRight, 2.f / mc::defines::operatingHeight))
			{
				list.pattern[35][position[35]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[35].c_str()) << cur.playerResults[id].position.verticalHandRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendPositionSideHandLeft && sendOnChange(cur.playerResults[id].position.sideHandLeft, old.playerResults[id].position.sideHandLeft, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[36][position[36]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[36].c_str()) << cur.playerResults[id].position.sideHandLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendPositionSideHandRight && sendOnChange(cur.playerResults[id].position.sideHandRight, old.playerResults[id].position.sideHandRight, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[37][position[37]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[37].c_str()) << cur.playerResults[id].position.sideHandRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendPositionSideFootLeft && sendOnChange(cur.playerResults[id].position.sideFootLeft, old.playerResults[id].position.sideFootLeft, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[38][position[38]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[38].c_str()) << cur.playerResults[id].position.sideFootLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendPositionSideFootRight && sendOnChange(cur.playerResults[id].position.sideFootRight, old.playerResults[id].position.sideFootRight, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[39][position[39]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[39].c_str()) << cur.playerResults[id].position.sideFootRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendPositionFrontHandLeft && sendOnChange(cur.playerResults[id].position.frontHandLeft, old.playerResults[id].position.frontHandLeft, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[40][position[40]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[40].c_str()) << cur.playerResults[id].position.frontHandLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendPositionFrontHandRight && sendOnChange(cur.playerResults[id].position.frontHandRight, old.playerResults[id].position.frontHandRight, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[41][position[41]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[41].c_str()) << cur.playerResults[id].position.frontHandRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendPositionFrontFootLeft && sendOnChange(cur.playerResults[id].position.frontFootLeft, old.playerResults[id].position.frontFootLeft, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[42][position[42]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[42].c_str()) << cur.playerResults[id].position.frontFootLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendPositionFrontFootRight && sendOnChange(cur.playerResults[id].position.frontFootRight, old.playerResults[id].position.frontFootRight, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[43][position[43]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[43].c_str()) << cur.playerResults[id].position.frontFootRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendPositionWidth && sendOnChange(cur.playerResults[id].position.width, old.playerResults[id].position.width, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[44][position[44]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[44].c_str()) << cur.playerResults[id].position.width << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}


			// here we later have peak


		}


		void MusicEnvironmentSender::sendPlayerGestureResult(const mc::command::MECommandBundle& command, const mc::result::ResultBundle& cur, const mc::result::ResultBundle& old, size_t id)
		{


			if (command.playerCommands[id].sendGestureHitOverhead && cur.playerResults[id].gesture.hitOverhead)
			{
				list.pattern[45][position[45]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[45].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendGestureHitDownLeft && cur.playerResults[id].gesture.hitDownLeft)
			{
				list.pattern[46][position[46]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[46].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendGestureHitDownRight && cur.playerResults[id].gesture.hitDownRight)
			{
				list.pattern[47][position[47]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[47].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendGestureHitSideLeft && cur.playerResults[id].gesture.hitSideLeft)
			{
				list.pattern[48][position[48]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[48].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendGestureHitSideRight && cur.playerResults[id].gesture.hitSideRight)
			{
				list.pattern[49][position[49]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[49].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendGestureHitForwardLeft && cur.playerResults[id].gesture.hitForwardLeft)
			{
				list.pattern[50][position[50]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[50].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendGestureHitForwardRight && cur.playerResults[id].gesture.hitForwardRight)
			{
				list.pattern[51][position[51]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[51].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendGestureKickSideLeft && cur.playerResults[id].gesture.kickSideLeft)
			{
				list.pattern[52][position[52]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[52].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendGestureKickSideRight && cur.playerResults[id].gesture.kickSideRight)
			{
				list.pattern[53][position[53]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[53].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendGestureKickForwardLeft && cur.playerResults[id].gesture.kickForwardLeft)
			{
				list.pattern[54][position[54]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[54].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}


			if (command.playerCommands[id].sendGestureKickForwardRight && cur.playerResults[id].gesture.kickForwardRight)
			{
				list.pattern[55][position[55]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[55].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendGestureDoubleArmSide && cur.playerResults[id].gesture.doubleArmSide)
			{
				list.pattern[56][position[56]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[56].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendGestureDoubleArmSideClose && cur.playerResults[id].gesture.doubleArmSideClose)
			{
				list.pattern[57][position[57]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[57].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendGestureJump && cur.playerResults[id].gesture.jump)
			{
				list.pattern[58][position[58]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[58].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.playerCommands[id].sendGestureClap && cur.playerResults[id].gesture.clap)
			{
				list.pattern[59][position[59]] = static_cast<char>(id) - 48;
				packet << osc::BeginMessage(list.pattern[59].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}
		}

	}
}