
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


		void ControlModuleSender::sendError(int error)
		{
			packet << osc::BeginMessage(list.pattern[2].c_str()) << error << osc::EndMessage;
			transmitSocket.Send(packet.Data(), packet.Size());
			packet.Clear();
		}


		void ControlModuleSender::sendStopped()
		{
			packet << osc::BeginMessage(list.pattern[3].c_str()) << osc::EndMessage;
			transmitSocket.Send(packet.Data(), packet.Size());
			packet.Clear();
		}


		// ==========================================================================================================================
		//
		// sender object to send osc messages to ME
		//
		// ==========================================================================================================================


		void MusicEnvironmentSender::send(const mc::structures::MusicBundle& command, const mc::structures::Selection& selection, const mc::structures::Result& result)
		{

			// may be we have to reset the oldResult to refresh the initialization ...
			// I think we can simplify the interface to the sub functions more ... I don't think we need to pass always the complete results in here ...
			if (selection.player[0] >= 0 && selection.player[1] < 0)
				sendPlayerResult(command, result, oldResult, 0, '0');
			else if (selection.player[0] >= 0 && selection.player[1] >= 0)
			{
				sendPlayerResult(command, result, oldResult, 0, '1');
				sendPlayerResult(command, result, oldResult, 1, '2');
			}

			// here we may be have to adjust the call signs ...
			sendZoneResult(command, result, oldResult, 0, '0');
			sendZoneResult(command, result, oldResult, 1, '1');
			sendZoneResult(command, result, oldResult, 2, '2');
			sendZoneResult(command, result, oldResult, 3, '3');
			sendZoneResult(command, result, oldResult, 4, '4');
			sendZoneResult(command, result, oldResult, 5, '5');

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



		void MusicEnvironmentSender::sendPlayerResult(const mc::structures::MusicBundle& command, const mc::structures::Result& cur, const mc::structures::Result& old, size_t id, char cs)
		{
			sendPlayerActivityResult(command, cur, old, id, cs);
			sendPlayerLocationResult(command, cur, old, id, cs);
			sendPlayerPositionResult(command, cur, old, id, cs);
			sendPlayerGestureResult(command, cur, old, id, cs);
		}


		void MusicEnvironmentSender::sendZoneResult(const mc::structures::MusicBundle& command, const mc::structures::Result& cur, const mc::structures::Result& old, size_t id, char cs)
		{

			if (command.zone[id].sendDiscrete && cur.zone[id].discrete)
			{
				list.pattern[60][position[60]] = cs;
				packet << osc::BeginMessage(list.pattern[60].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.zone[id].sendNormal &&
				sendOnDifferentFromZero(cur.zone[id].normal, old.zone[id].normal))
			{
				list.pattern[61][position[61]] = cs;
				packet << osc::BeginMessage(list.pattern[61].c_str()) << cur.zone[id].normal << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.zone[id].sendFlowLeftwards &&
				sendOnDifferentFromZero(cur.zone[id].flowLeftwards, old.zone[id].flowLeftwards))
			{
				list.pattern[62][position[62]] = cs;
				packet << osc::BeginMessage(list.pattern[62].c_str()) << cur.zone[id].flowLeftwards << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.zone[id].sendFlowRightwards &&
				sendOnDifferentFromZero(cur.zone[id].flowRightwards, old.zone[id].flowRightwards))
			{
				list.pattern[63][position[63]] = cs;
				packet << osc::BeginMessage(list.pattern[63].c_str()) << cur.zone[id].flowRightwards << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.zone[id].sendFlowUpwards &&
				sendOnDifferentFromZero(cur.zone[id].flowUpwards, old.zone[id].flowUpwards))
			{
				list.pattern[64][position[64]] = cs;
				packet << osc::BeginMessage(list.pattern[64].c_str()) << cur.zone[id].flowUpwards << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.zone[id].sendFlowDownwards &&
				sendOnDifferentFromZero(cur.zone[id].flowDownwards, old.zone[id].flowDownwards))
			{
				list.pattern[65][position[65]] = cs;
				packet << osc::BeginMessage(list.pattern[65].c_str()) << cur.zone[id].flowDownwards << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}
		}



		void MusicEnvironmentSender::sendPlayerActivityResult(const mc::structures::MusicBundle& command, const mc::structures::Result& cur, const mc::structures::Result& old, size_t id, char cs)
		{

			// ==========================================================================================================================
			// activity discrete
			// ==========================================================================================================================


			if (command.player[id].sendDiscreteHandLeft && cur.player[id].activity.discrete.handLeft)
			{
				list.pattern[0][position[0]] = cs;
				packet << osc::BeginMessage(list.pattern[0].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendDiscreteHandRight && cur.player[id].activity.discrete.handRight)
			{
				list.pattern[1][position[1]] = cs;
				packet << osc::BeginMessage(list.pattern[1].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendDiscreteHead && cur.player[id].activity.discrete.head)
			{
				list.pattern[2][position[2]] = cs;
				packet << osc::BeginMessage(list.pattern[2].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendDiscreteLegLeft && cur.player[id].activity.discrete.legLeft)
			{
				list.pattern[3][position[3]] = cs;
				packet << osc::BeginMessage(list.pattern[3].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendDiscreteLegRight && cur.player[id].activity.discrete.legRight)
			{
				list.pattern[4][position[4]] = cs;
				packet << osc::BeginMessage(list.pattern[4].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendDiscreteBodyUpper && cur.player[id].activity.discrete.bodyUpper)
			{
				list.pattern[5][position[5]] = cs;
				packet << osc::BeginMessage(list.pattern[5].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendDiscreteBodyLower && cur.player[id].activity.discrete.bodyLower)
			{
				list.pattern[6][position[6]] = cs;
				packet << osc::BeginMessage(list.pattern[6].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendDiscreteBodyLeft && cur.player[id].activity.discrete.bodyLeft)
			{
				list.pattern[7][position[7]] = cs;
				packet << osc::BeginMessage(list.pattern[7].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendDiscreteBodyRight && cur.player[id].activity.discrete.bodyRight)
			{
				list.pattern[8][position[8]] = cs;
				packet << osc::BeginMessage(list.pattern[8].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}


			// ==========================================================================================================================
			// activity normal
			// ==========================================================================================================================


			if (command.player[id].sendNormalHandLeft &&
				sendOnDifferentFromZero(cur.player[id].activity.normal.handLeft, old.player[id].activity.normal.handLeft))
			{
				list.pattern[9][position[9]] = cs;
				packet << osc::BeginMessage(list.pattern[9].c_str()) << cur.player[id].activity.normal.handLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendNormalHandRight &&
				sendOnDifferentFromZero(cur.player[id].activity.normal.handRight, old.player[id].activity.normal.handRight))
			{
				list.pattern[10][position[10]] = cs;
				packet << osc::BeginMessage(list.pattern[10].c_str()) << cur.player[id].activity.normal.handRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendNormalHead &&
				sendOnDifferentFromZero(cur.player[id].activity.normal.head, old.player[id].activity.normal.head))
			{
				list.pattern[11][position[11]] = cs;
				packet << osc::BeginMessage(list.pattern[11].c_str()) << cur.player[id].activity.normal.head << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendNormalLegLeft &&
				sendOnDifferentFromZero(cur.player[id].activity.normal.legLeft, old.player[id].activity.normal.legLeft))
			{
				list.pattern[12][position[12]] = cs;
				packet << osc::BeginMessage(list.pattern[12].c_str()) << cur.player[id].activity.normal.legLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendNormalLegRight &&
				sendOnDifferentFromZero(cur.player[id].activity.normal.legRight, old.player[id].activity.normal.legRight))
			{
				list.pattern[13][position[13]] = cs;
				packet << osc::BeginMessage(list.pattern[13].c_str()) << cur.player[id].activity.normal.legRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendNormalBodyUpper &&
				sendOnDifferentFromZero(cur.player[id].activity.normal.bodyUpper, old.player[id].activity.normal.bodyUpper))
			{
				list.pattern[14][position[14]] = cs;
				packet << osc::BeginMessage(list.pattern[14].c_str()) << cur.player[id].activity.normal.bodyUpper << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendNormalBodyLower &&
				sendOnDifferentFromZero(cur.player[id].activity.normal.bodyLower, old.player[id].activity.normal.bodyLower))
			{
				list.pattern[15][position[15]] = cs;
				packet << osc::BeginMessage(list.pattern[15].c_str()) << cur.player[id].activity.normal.bodyLower << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendNormalBodyLeft &&
				sendOnDifferentFromZero(cur.player[id].activity.normal.bodyLeft, old.player[id].activity.normal.bodyLeft))
			{
				list.pattern[16][position[16]] = cs;
				packet << osc::BeginMessage(list.pattern[16].c_str()) << cur.player[id].activity.normal.bodyLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendNormalBodyRight &&
				sendOnDifferentFromZero(cur.player[id].activity.normal.bodyRight, old.player[id].activity.normal.bodyRight))
			{
				list.pattern[17][position[17]] = cs;
				packet << osc::BeginMessage(list.pattern[17].c_str()) << cur.player[id].activity.normal.bodyRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}


			////////////////////////////////////////////////////

			// request if peak should be moved to position because it fits better there ...
			// in tech spec it is still in activity ...
			// in this source code, it is already located in position
			// but not (yet) in the me command ...
			if (command.player[id].sendPositionPeak &&
				sendOnDifferentFromZero(cur.player[id].position.peak, old.player[id].position.peak))
			{
				list.pattern[18][position[18]] = cs;
				packet << osc::BeginMessage(list.pattern[18].c_str()) << cur.player[id].position.peak << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}
			// ask this Robert !

			//////////////////////////////////////////////////


			// ==========================================================================================================================
			// activity flow
			// ==========================================================================================================================


			if (command.player[id].sendFlowLeftwardsLeft &&
				sendOnDifferentFromZero(cur.player[id].activity.flow.leftwardsLeft, old.player[id].activity.flow.leftwardsLeft))
			{
				list.pattern[19][position[19]] = cs;
				packet << osc::BeginMessage(list.pattern[19].c_str()) << cur.player[id].activity.flow.leftwardsLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendFlowLeftwardsRight &&
				sendOnDifferentFromZero(cur.player[id].activity.flow.leftwardsRight, old.player[id].activity.flow.leftwardsRight))
			{
				list.pattern[20][position[20]] = cs;
				packet << osc::BeginMessage(list.pattern[20].c_str()) << cur.player[id].activity.flow.leftwardsRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendFlowRightwardsLeft &&
				sendOnDifferentFromZero(cur.player[id].activity.flow.rightwardsLeft, old.player[id].activity.flow.rightwardsLeft))
			{
				list.pattern[21][position[21]] = cs;
				packet << osc::BeginMessage(list.pattern[21].c_str()) << cur.player[id].activity.flow.rightwardsLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendFlowRightwardsRight &&
				sendOnDifferentFromZero(cur.player[id].activity.flow.rightwardsRight, old.player[id].activity.flow.rightwardsRight))
			{
				list.pattern[22][position[22]] = cs;
				packet << osc::BeginMessage(list.pattern[22].c_str()) << cur.player[id].activity.flow.rightwardsRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendFlowUpwardsLeft &&
				sendOnDifferentFromZero(cur.player[id].activity.flow.upwardsLeft, old.player[id].activity.flow.upwardsLeft))
			{
				list.pattern[23][position[23]] = cs;
				packet << osc::BeginMessage(list.pattern[23].c_str()) << cur.player[id].activity.flow.upwardsLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendFlowUpwardsRight &&
				sendOnDifferentFromZero(cur.player[id].activity.flow.upwardsRight, old.player[id].activity.flow.upwardsRight))
			{
				list.pattern[24][position[24]] = cs;
				packet << osc::BeginMessage(list.pattern[24].c_str()) << cur.player[id].activity.flow.upwardsRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendFlowDownwardsLeft &&
				sendOnDifferentFromZero(cur.player[id].activity.flow.downwardsLeft, old.player[id].activity.flow.downwardsLeft))
			{
				list.pattern[25][position[25]] = cs;
				packet << osc::BeginMessage(list.pattern[25].c_str()) << cur.player[id].activity.flow.downwardsLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendFlowDownwardsRight &&
				sendOnDifferentFromZero(cur.player[id].activity.flow.downwardsRight, old.player[id].activity.flow.downwardsRight))
			{
				list.pattern[26][position[26]] = cs;
				packet << osc::BeginMessage(list.pattern[26].c_str()) << cur.player[id].activity.flow.downwardsRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}

		}


		void MusicEnvironmentSender::sendPlayerLocationResult(const mc::structures::MusicBundle& command, const mc::structures::Result& cur, const mc::structures::Result& old, size_t id, char cs)
		{
			if (command.player[id].sendLocationReady && cur.player[id].location.ready)
			{
				list.pattern[27][position[27]] = cs;
				packet << osc::BeginMessage(list.pattern[27].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}

			if (command.player[id].sendLocationPresent && cur.player[id].location.present)
			{
				list.pattern[28][position[28]] = cs;
				packet << osc::BeginMessage(list.pattern[28].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}

			if (command.player[id].sendLocationCenterX && sendOnChange(cur.player[id].location.centerX, old.player[id].location.centerX, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[29][position[29]] = cs;
				packet << osc::BeginMessage(list.pattern[29].c_str()) << cur.player[id].location.centerX << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}

			if (command.player[id].sendLocationCenterZ && sendOnChange(cur.player[id].location.centerZ, old.player[id].location.centerZ, 100.f))
			{
				list.pattern[30][position[30]] = cs;
				packet << osc::BeginMessage(list.pattern[30].c_str()) << cur.player[id].location.centerZ << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}


			if (command.player[id].sendLocationOutOfRange && cur.player[id].location.outOfRange)
			{
				// this is not a bool, but we need to send a string in oder to nidicate if person is to close or to far away
				// we can internally have to bools: too close, too far, and this is only sended as outOfRange with a string ...
				// we need to provide a metric threshold

				//list.pattern[31][position[31]] = cs;
				//packet << osc::BeginMessage(list.pattern[31].c_str()) << cur.player[id].position.positionHeight << osc::EndMessage;
				//transmitSocket.Send(packet.Data(), packet.Size());
				//packet.Clear();
			}


		}


		void MusicEnvironmentSender::sendPlayerPositionResult(const mc::structures::MusicBundle& command, const mc::structures::Result& cur, const mc::structures::Result& old, size_t id, char cs)
		{


			if (command.player[id].sendPositionHeight && sendOnChange(cur.player[id].position.height, old.player[id].position.height, 2.f / mc::defines::operatingHeight))
			{
				list.pattern[32][position[32]] = cs;
				packet << osc::BeginMessage(list.pattern[32].c_str()) << cur.player[id].position.height << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendPositionHeightLevel && sendOnChange(cur.player[id].position.heightLevel, old.player[id].position.heightLevel, 1))
			{
				list.pattern[33][position[33]] = cs;
				packet << osc::BeginMessage(list.pattern[33].c_str()) << cur.player[id].position.heightLevel << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendPositionVerticalHandLeft && sendOnChange(cur.player[id].position.verticalHandLeft, old.player[id].position.verticalHandLeft, 2.f / mc::defines::operatingHeight))
			{
				list.pattern[34][position[34]] = cs;
				packet << osc::BeginMessage(list.pattern[34].c_str()) << cur.player[id].position.verticalHandLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendPositionVerticalHandRight && sendOnChange(cur.player[id].position.verticalHandRight, old.player[id].position.verticalHandRight, 2.f / mc::defines::operatingHeight))
			{
				list.pattern[35][position[35]] = cs;
				packet << osc::BeginMessage(list.pattern[35].c_str()) << cur.player[id].position.verticalHandRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendPositionSideHandLeft && sendOnChange(cur.player[id].position.sideHandLeft, old.player[id].position.sideHandLeft, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[36][position[36]] = cs;
				packet << osc::BeginMessage(list.pattern[36].c_str()) << cur.player[id].position.sideHandLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendPositionSideHandRight && sendOnChange(cur.player[id].position.sideHandRight, old.player[id].position.sideHandRight, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[37][position[37]] = cs;
				packet << osc::BeginMessage(list.pattern[37].c_str()) << cur.player[id].position.sideHandRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendPositionSideFootLeft && sendOnChange(cur.player[id].position.sideFootLeft, old.player[id].position.sideFootLeft, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[38][position[38]] = cs;
				packet << osc::BeginMessage(list.pattern[38].c_str()) << cur.player[id].position.sideFootLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendPositionSideFootRight && sendOnChange(cur.player[id].position.sideFootRight, old.player[id].position.sideFootRight, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[39][position[39]] = cs;
				packet << osc::BeginMessage(list.pattern[39].c_str()) << cur.player[id].position.sideFootRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendPositionFrontHandLeft && sendOnChange(cur.player[id].position.frontHandLeft, old.player[id].position.frontHandLeft, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[40][position[40]] = cs;
				packet << osc::BeginMessage(list.pattern[40].c_str()) << cur.player[id].position.frontHandLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendPositionFrontHandRight && sendOnChange(cur.player[id].position.frontHandRight, old.player[id].position.frontHandRight, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[41][position[41]] = cs;
				packet << osc::BeginMessage(list.pattern[41].c_str()) << cur.player[id].position.frontHandRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendPositionFrontFootLeft && sendOnChange(cur.player[id].position.frontFootLeft, old.player[id].position.frontFootLeft, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[42][position[42]] = cs;
				packet << osc::BeginMessage(list.pattern[42].c_str()) << cur.player[id].position.frontFootLeft << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendPositionFrontFootRight && sendOnChange(cur.player[id].position.frontFootRight, old.player[id].position.frontFootRight, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[43][position[43]] = cs;
				packet << osc::BeginMessage(list.pattern[43].c_str()) << cur.player[id].position.frontFootRight << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendPositionWidth && sendOnChange(cur.player[id].position.width, old.player[id].position.width, 2.f / mc::defines::operatingWidth))
			{
				list.pattern[44][position[44]] = cs;
				packet << osc::BeginMessage(list.pattern[44].c_str()) << cur.player[id].position.width << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}


			// here we later have peak


		}


		void MusicEnvironmentSender::sendPlayerGestureResult(const mc::structures::MusicBundle& command, const mc::structures::Result& cur, const mc::structures::Result& old, size_t id, char cs)
		{


			if (command.player[id].sendGestureHitOverhead && cur.player[id].gesture.hitOverhead)
			{
				list.pattern[45][position[45]] = cs;
				packet << osc::BeginMessage(list.pattern[45].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendGestureHitDownLeft && cur.player[id].gesture.hitDownLeft)
			{
				list.pattern[46][position[46]] = cs;
				packet << osc::BeginMessage(list.pattern[46].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendGestureHitDownRight && cur.player[id].gesture.hitDownRight)
			{
				list.pattern[47][position[47]] = cs;
				packet << osc::BeginMessage(list.pattern[47].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendGestureHitSideLeft && cur.player[id].gesture.hitSideLeft)
			{
				list.pattern[48][position[48]] = cs;
				packet << osc::BeginMessage(list.pattern[48].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendGestureHitSideRight && cur.player[id].gesture.hitSideRight)
			{
				list.pattern[49][position[49]] = cs;
				packet << osc::BeginMessage(list.pattern[49].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendGestureHitForwardLeft && cur.player[id].gesture.hitForwardLeft)
			{
				list.pattern[50][position[50]] = cs;
				packet << osc::BeginMessage(list.pattern[50].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendGestureHitForwardRight && cur.player[id].gesture.hitForwardRight)
			{
				list.pattern[51][position[51]] = cs;
				packet << osc::BeginMessage(list.pattern[51].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendGestureKickSideLeft && cur.player[id].gesture.kickSideLeft)
			{
				list.pattern[52][position[52]] = cs;
				packet << osc::BeginMessage(list.pattern[52].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendGestureKickSideRight && cur.player[id].gesture.kickSideRight)
			{
				list.pattern[53][position[53]] = cs;
				packet << osc::BeginMessage(list.pattern[53].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendGestureKickForwardLeft && cur.player[id].gesture.kickForwardLeft)
			{
				list.pattern[54][position[54]] = cs;
				packet << osc::BeginMessage(list.pattern[54].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}


			if (command.player[id].sendGestureKickForwardRight && cur.player[id].gesture.kickForwardRight)
			{
				list.pattern[55][position[55]] = cs;
				packet << osc::BeginMessage(list.pattern[55].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendGestureDoubleArmSide && cur.player[id].gesture.doubleArmSide)
			{
				list.pattern[56][position[56]] = cs;
				packet << osc::BeginMessage(list.pattern[56].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendGestureDoubleArmSideClose && cur.player[id].gesture.doubleArmSideClose)
			{
				list.pattern[57][position[57]] = cs;
				packet << osc::BeginMessage(list.pattern[57].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendGestureJump && cur.player[id].gesture.jump)
			{
				list.pattern[58][position[58]] = cs;
				packet << osc::BeginMessage(list.pattern[58].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}



			if (command.player[id].sendGestureClap && cur.player[id].gesture.clap)
			{
				list.pattern[59][position[59]] = cs;
				packet << osc::BeginMessage(list.pattern[59].c_str()) << osc::EndMessage;
				transmitSocket.Send(packet.Data(), packet.Size());
				packet.Clear();
			}
		}

	}
}