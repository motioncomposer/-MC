
#include "receiver.h"

namespace mc
{
	namespace receiver
	{

		// ==========================================================================================================================
		//
		// helper function to interpret the incoming messages
		//
		// ==========================================================================================================================


		size_t getOffset(const std::vector<std::string>& pattern, char delimiter)
		{
			if (pattern.empty())
				return std::string::npos;

			for (auto& it : pattern)
				if (it[0] != delimiter)
					return std::string::npos;

			size_t smallest(0);
			for (auto&& c = 0; c < pattern.size(); ++c)
				if (pattern[c].length() < pattern[smallest].length())
					smallest = c;

			if (pattern[smallest].empty())
				return std::string::npos;

			size_t offset(0);
			for (auto&& c = 1; c < pattern[smallest].length(); ++c)
			{
				char comp = pattern[smallest][c];

				for (auto& it : pattern)
				{
					if (it[c] != comp)
						return offset;
				}

				if (comp == delimiter)
					offset = c;
			}

			for (auto&& c = 0; c < pattern.size(); ++c)
			{
				if (c == smallest)
					continue;

				if (pattern[c][pattern[smallest].length()] != delimiter)
					return offset;
			}

			return pattern[smallest].length();
		}

		size_t checkSample(const std::string& sample, const std::string& pattern, int32_t prelude, char delimiter, char placeholder)
		{

			if (pattern.length() < sample.length() || sample.length() < prelude)
				return std::string::npos;

			size_t s_index(0);

			while (s_index < prelude)
			{
				if (sample[s_index] != pattern[s_index])
					return std::string::npos;

				++s_index;
			}

			if (prelude == sample.length())
				return 10;

			size_t id(10);
			size_t p_index = s_index;
			bool placeholder_found(false);

			while (s_index < sample.length())
			{

				if (sample[s_index] != pattern[p_index])
				{
					if (pattern[p_index] == placeholder && sample[s_index] > 47 && sample[s_index] < 58 && !placeholder_found)
					{
						placeholder_found = true;
						id = sample[s_index] - 48;
					}
					else if (pattern[p_index] == placeholder && !placeholder_found)
					{
						placeholder_found = true;
						id = 10;
						--s_index;
						++p_index;
					}
					else
					{
						id = std::string::npos;
						return id;
					}
				}

				++s_index;
				++p_index;
			}


			if (p_index < pattern.length() && pattern[p_index] != delimiter)
				id = std::string::npos;

			return id;
		}


		// ==========================================================================================================================
		//
		// CM handler functions to manipulate CM commands
		//
		// ==========================================================================================================================


		void handleInitRequest(mc::command::CMCommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t)
		{
			ref->stateControl = 1;

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- received init command" << std::endl;
#endif
		}


		void handleStopRequest(mc::command::CMCommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t)
		{
			ref->stateControl = -1;

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- received stop command" << std::endl;
#endif
		}


		void handlePlayerTrackingRequest(mc::command::CMCommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			// actually we have to make here some special handling for the player tracking stuff

			if (player == 10)
			{
				ref->setPlayerTracking[0] = (dmy != 0);
				ref->setPlayerTracking[1] = (dmy != 0);
				// actually there are 3 tracking codes we can receive ...
			}
			else
				ref->setPlayerTracking[(player > 0 ? player - 1 : player)] = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- received player tracking command" << std::endl;
			std::cout << "-- tracking player: " << (player == 10 ? "all" : std::to_string(player)) << " switched to " << (dmy != 0 ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerBlobRequest(mc::command::CMCommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			// actually we have to make here some special handling for the player tracking stuff

			if (player == 10)
			{
				ref->setPlayerTracking[0] = (dmy != 0);
				ref->setPlayerTracking[1] = (dmy != 0);
			}
			else
			{
				// here we still have to check if the player id is in the correct range ...
				// we need some kind of player mode ...
				ref->setPlayerTracking[(player > 0 ? player - 1 : player)] = (dmy != 0);
			}

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- received player blob command" << std::endl;
			std::cout << "-- blob player: " << (player == 10 ? "all" : std::to_string(player)) << " is " << dmy << std::endl;
#endif
		}


		void handleZoneTrackingRequest(mc::command::CMCommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			// actually we have to make here some special handling for the player tracking stuff

			if (zone == 10)
			{
				ref->setZoneTracking[0] = (dmy != 0);
				ref->setZoneTracking[1] = (dmy != 0);
				ref->setZoneTracking[2] = (dmy != 0);
				ref->setZoneTracking[3] = (dmy != 0);
				ref->setZoneTracking[4] = (dmy != 0);
				ref->setZoneTracking[5] = (dmy != 0);
			}
			else
				ref->setZoneTracking[zone] = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- received zone tracking command" << std::endl;
			std::cout << "-- tracking zone: " << (zone == 10 ? "all" : std::to_string(zone)) << " switched to " << (dmy != 0 ? "on" : "off") << std::endl;
#endif
		}


		void handleZoneBlobRequest(mc::command::CMCommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			// actually we have to make here some special handling for the player tracking stuff

			// we need to add the a player mode to (state space machine ...)

			// as we handle the player mode implicitly

			if (zone == 10)
			{
				ref->setZoneTracking[0] = (dmy != 0);
				ref->setZoneTracking[1] = (dmy != 0);
				ref->setZoneTracking[2] = (dmy != 0);
				ref->setZoneTracking[3] = (dmy != 0);
				ref->setZoneTracking[4] = (dmy != 0);
				ref->setZoneTracking[5] = (dmy != 0);
			}
			else
				ref->setZoneTracking[zone - 1] = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- received zone tracking command" << std::endl;
			std::cout << "-- tracking zone: " << (zone == 10 ? "all" : std::to_string(zone)) << " switched to " << (dmy != 0 ? "on" : "off") << std::endl;
#endif
		}


		// ==========================================================================================================================
		//
		// listener object that maps the received osc messages to CM handler functions
		//
		// ==========================================================================================================================


		void ControlModuleListener::ProcessMessage(const osc::ReceivedMessage& message,
			const IpEndpointName& remoteEndpoint)
		{
			(void)remoteEndpoint; // suppress unused parameter warning

			try
			{
				std::string address = message.AddressPattern();
				osc::ReceivedMessageArgumentStream args = message.ArgumentStream();

				for (auto&& c = 0; c < list.pattern.size(); ++c)
				{
					auto id = checkSample(address, list.pattern[c], offset, mc::defines::delimiter, mc::defines::placeholder);

					if (id != std::string::npos)
						handlers[c](args, id);				
				}
			}
			catch (...)
			{
				++error;
			}
		}


		// ==========================================================================================================================
		//
		// receiver object that handles the incoming osc messages from CM
		//
		// ==========================================================================================================================


		void ControlModuleReceiver::receiveFromNetwork()
		{
			socket.reset(new UdpListeningReceiveSocket(
				IpEndpointName(IpEndpointName::ANY_ADDRESS, port),
				&listener));

			socket->Run();
		}


		// ==========================================================================================================================
		//
		// ME handler functions to manipulate ME commands
		//
		// ==========================================================================================================================


		void handlePlayerDiscreteHandLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendDiscreteHandLeft = (dmy != 0);
				ref->playerCommands[1].sendDiscreteHandLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendDiscreteHandLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteHandLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerDiscreteHandRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendDiscreteHandRight = (dmy != 0);
				ref->playerCommands[1].sendDiscreteHandRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendDiscreteHandRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteHandRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerDiscreteHeadRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendDiscreteHead = (dmy != 0);
				ref->playerCommands[1].sendDiscreteHead = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendDiscreteHead = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteHead of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerDiscreteLegLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendDiscreteLegLeft = (dmy != 0);
				ref->playerCommands[1].sendDiscreteLegLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendDiscreteLegLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteLegLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerDiscreteLegRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendDiscreteLegRight = (dmy != 0);
				ref->playerCommands[1].sendDiscreteLegRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendDiscreteLegRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteLegRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerDiscreteBodyUpperRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendDiscreteBodyUpper = (dmy != 0);
				ref->playerCommands[1].sendDiscreteBodyUpper = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendDiscreteBodyUpper = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteBodyUpper of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerDiscreteBodyLowerRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendDiscreteBodyLower = (dmy != 0);
				ref->playerCommands[1].sendDiscreteBodyLower = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendDiscreteBodyLower = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteBodyLower of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerDiscreteBodyLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendDiscreteBodyLeft = (dmy != 0);
				ref->playerCommands[1].sendDiscreteBodyLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendDiscreteBodyLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteBodyLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerDiscreteBodyRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendDiscreteBodyRight = (dmy != 0);
				ref->playerCommands[1].sendDiscreteBodyRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendDiscreteBodyRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteBodyRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalHandLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendNormalHandLeft = (dmy != 0);
				ref->playerCommands[1].sendNormalHandLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendNormalHandLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalHandLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalHandRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendNormalHandRight = (dmy != 0);
				ref->playerCommands[1].sendNormalHandRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendNormalHandRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalHandRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalHeadRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendNormalHead = (dmy != 0);
				ref->playerCommands[1].sendNormalHead = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendNormalHead = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalHead of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalLegLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendNormalLegLeft = (dmy != 0);
				ref->playerCommands[1].sendNormalLegLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendNormalLegLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalLegLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalLegRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendNormalLegRight = (dmy != 0);
				ref->playerCommands[1].sendNormalLegRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendNormalLegRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalLegRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalBodyUpperRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendNormalBodyUpper = (dmy != 0);
				ref->playerCommands[1].sendNormalBodyUpper = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendNormalBodyUpper = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalBodyUpper of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalBodyLowerRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendNormalBodyLower = (dmy != 0);
				ref->playerCommands[1].sendNormalBodyLower = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendNormalBodyLower = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalBodyLower of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalBodyLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendNormalBodyLeft = (dmy != 0);
				ref->playerCommands[1].sendNormalBodyLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendNormalBodyLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalBodyLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalBodyRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendNormalBodyRight = (dmy != 0);
				ref->playerCommands[1].sendNormalBodyRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendNormalBodyRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalBodyRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPeakRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendPeak = (dmy != 0);
				ref->playerCommands[1].sendPeak = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendPeak = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPeak of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerFlowLeftwardsLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendFlowLeftwardsLeft = (dmy != 0);
				ref->playerCommands[1].sendFlowLeftwardsLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendFlowLeftwardsLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowLeftwardsLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerFlowLeftwardsRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendFlowLeftwardsRight = (dmy != 0);
				ref->playerCommands[1].sendFlowLeftwardsRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendFlowLeftwardsRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowLeftwardsRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerFlowRightwardsLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendFlowRightwardsLeft = (dmy != 0);
				ref->playerCommands[1].sendFlowRightwardsLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendFlowRightwardsLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowRightwardsLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerFlowRightwardsRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendFlowRightwardsRight = (dmy != 0);
				ref->playerCommands[1].sendFlowRightwardsRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendFlowRightwardsRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowRightwardsRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerFlowUpwardsLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendFlowUpwardsLeft = (dmy != 0);
				ref->playerCommands[1].sendFlowUpwardsLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendFlowUpwardsLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowUpwardsLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerFlowUpwardsRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendFlowUpwardsRight = (dmy != 0);
				ref->playerCommands[1].sendFlowUpwardsRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendFlowUpwardsRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowUpwardsRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerFlowDownwardsLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendFlowDownwardsLeft = (dmy != 0);
				ref->playerCommands[1].sendFlowDownwardsLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendFlowDownwardsLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowDownwardsLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerFlowDownwardsRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendFlowDownwardsRight = (dmy != 0);
				ref->playerCommands[1].sendFlowDownwardsRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendFlowDownwardsRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowDownwardsRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerLocationReadyRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendLocationReady = (dmy != 0);
				ref->playerCommands[1].sendLocationReady = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendLocationReady = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendLocationReady of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerLocationPresentRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendLocationPresent = (dmy != 0);
				ref->playerCommands[1].sendLocationPresent = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendLocationPresent = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendLocationPresent of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerLocationCenterXRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendLocationCenterX = (dmy != 0);
				ref->playerCommands[1].sendLocationCenterX = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendLocationCenterX = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendLocationCenterX of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerLocationCenterZRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendLocationCenterZ = (dmy != 0);
				ref->playerCommands[1].sendLocationCenterZ = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendLocationCenterZ = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendLocationCenterZ of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerLocationOutOfRangeRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendLocationOutOfRange = (dmy != 0);
				ref->playerCommands[1].sendLocationOutOfRange = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendLocationOutOfRange = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendLocationOutOfRange of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionHeightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendPositionHeight = (dmy != 0);
				ref->playerCommands[1].sendPositionHeight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendPositionHeight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionHeight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionHeightLevelRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendPositionHeightLevel = (dmy != 0);
				ref->playerCommands[1].sendPositionHeightLevel = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendPositionHeightLevel = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionHeightLevel of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionVerticalHandLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendPositionVerticalHandLeft = (dmy != 0);
				ref->playerCommands[1].sendPositionVerticalHandLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendPositionVerticalHandLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionVerticalHandLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionVerticalHandRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendPositionVerticalHandRight = (dmy != 0);
				ref->playerCommands[1].sendPositionVerticalHandRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendPositionVerticalHandRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionVerticalHandRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionSideHandLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendPositionSideHandLeft = (dmy != 0);
				ref->playerCommands[1].sendPositionSideHandLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendPositionSideHandLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionSideHandLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionSideHandRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendPositionSideHandRight = (dmy != 0);
				ref->playerCommands[1].sendPositionSideHandRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendPositionSideHandRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionSideHandRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionSideFootLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendPositionSideFootLeft = (dmy != 0);
				ref->playerCommands[1].sendPositionSideFootLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendPositionSideFootLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionSideFootLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionSideFootRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendPositionSideFootRight = (dmy != 0);
				ref->playerCommands[1].sendPositionSideFootRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendPositionSideFootRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionSideFootRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionFrontHandLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendPositionFrontHandLeft = (dmy != 0);
				ref->playerCommands[1].sendPositionFrontHandLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendPositionFrontHandLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionFrontHandLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionFrontHandRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendPositionFrontHandRight = (dmy != 0);
				ref->playerCommands[1].sendPositionFrontHandRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendPositionFrontHandRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionFrontHandRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionFrontFootLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendPositionFrontFootLeft = (dmy != 0);
				ref->playerCommands[1].sendPositionFrontFootLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendPositionFrontFootLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionFrontFootLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionFrontFootRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendPositionFrontFootRight = (dmy != 0);
				ref->playerCommands[1].sendPositionFrontFootRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendPositionFrontFootRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionFrontFootRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionWidthRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendPositionWidth = (dmy != 0);
				ref->playerCommands[1].sendPositionWidth = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendPositionWidth = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionWidth of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureHitOverheadRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendGestureHitOverhead = (dmy != 0);
				ref->playerCommands[1].sendGestureHitOverhead = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendGestureHitOverhead = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureHitOverhead of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureHitSideLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendGestureHitSideLeft = (dmy != 0);
				ref->playerCommands[1].sendGestureHitSideLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendGestureHitSideLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureHitSideLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureHitSideRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendGestureHitSideRight = (dmy != 0);
				ref->playerCommands[1].sendGestureHitSideRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendGestureHitSideRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureHitSideRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureHitDownLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendGestureHitDownLeft = (dmy != 0);
				ref->playerCommands[1].sendGestureHitDownLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendGestureHitDownLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureHitDownLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureHitDownRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendGestureHitDownRight = (dmy != 0);
				ref->playerCommands[1].sendGestureHitDownRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendGestureHitDownRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureHitDownRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureHitForwardLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendGestureHitForwardLeft = (dmy != 0);
				ref->playerCommands[1].sendGestureHitForwardLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendGestureHitForwardLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureHitForwardLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureHitForwardRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendGestureHitForwardRight = (dmy != 0);
				ref->playerCommands[1].sendGestureHitForwardRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendGestureHitForwardRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureHitForwardRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureKickSideLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendGestureKickSideLeft = (dmy != 0);
				ref->playerCommands[1].sendGestureKickSideLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendGestureKickSideLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureKickSideLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureKickSideRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendGestureKickSideRight = (dmy != 0);
				ref->playerCommands[1].sendGestureKickSideRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendGestureKickSideRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureKickSideRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureKickForwardLeftRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendGestureKickForwardLeft = (dmy != 0);
				ref->playerCommands[1].sendGestureKickForwardLeft = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendGestureKickForwardLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureKickForwardLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureKickForwardRightRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendGestureKickForwardRight = (dmy != 0);
				ref->playerCommands[1].sendGestureKickForwardRight = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendGestureKickForwardRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureKickForwardRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureDoubleArmSideRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendGestureDoubleArmSide = (dmy != 0);
				ref->playerCommands[1].sendGestureDoubleArmSide = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendGestureDoubleArmSide = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureDoubleArmSide of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureDoubleArmSideCloseRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendGestureDoubleArmSideClose = (dmy != 0);
				ref->playerCommands[1].sendGestureDoubleArmSideClose = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendGestureDoubleArmSideClose = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureDoubleArmSideClose of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureJumpRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendGestureJump = (dmy != 0);
				ref->playerCommands[1].sendGestureJump = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendGestureJump = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureJump of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureClapRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->playerCommands[0].sendGestureClap = (dmy != 0);
				ref->playerCommands[1].sendGestureClap = (dmy != 0);
			}
			else
				ref->playerCommands[(player > 0 ? player - 1 : player)].sendGestureClap = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureClap of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handleZoneDiscreteRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (zone == 10)
			{
				ref->zoneCommands[0].sendDiscrete = (dmy != 0);
				ref->zoneCommands[1].sendDiscrete = (dmy != 0);
				ref->zoneCommands[2].sendDiscrete = (dmy != 0);
				ref->zoneCommands[3].sendDiscrete = (dmy != 0);
				ref->zoneCommands[4].sendDiscrete = (dmy != 0);
				ref->zoneCommands[5].sendDiscrete = (dmy != 0);
			}
			else
				ref->zoneCommands[zone - 1].sendDiscrete = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscrete of " << (zone == 10 ? "all" : std::to_string(zone))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handleZoneNormalRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (zone == 10)
			{
				ref->zoneCommands[0].sendNormal = (dmy != 0);
				ref->zoneCommands[1].sendNormal = (dmy != 0);
				ref->zoneCommands[2].sendNormal = (dmy != 0);
				ref->zoneCommands[3].sendNormal = (dmy != 0);
				ref->zoneCommands[4].sendNormal = (dmy != 0);
				ref->zoneCommands[5].sendNormal = (dmy != 0);
			}
			else
				ref->zoneCommands[zone - 1].sendNormal = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormal of " << (zone == 10 ? "all" : std::to_string(zone))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handleZoneFlowLeftwardsRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (zone == 10)
			{
				ref->zoneCommands[0].sendFlowLeftwards = (dmy != 0);
				ref->zoneCommands[1].sendFlowLeftwards = (dmy != 0);
				ref->zoneCommands[2].sendFlowLeftwards = (dmy != 0);
				ref->zoneCommands[3].sendFlowLeftwards = (dmy != 0);
				ref->zoneCommands[4].sendFlowLeftwards = (dmy != 0);
				ref->zoneCommands[5].sendFlowLeftwards = (dmy != 0);
			}
			else
				ref->zoneCommands[zone - 1].sendFlowLeftwards = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowLeftwards of " << (zone == 10 ? "all" : std::to_string(zone))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handleZoneFlowRightwardsRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (zone == 10)
			{
				ref->zoneCommands[0].sendFlowRightwards = (dmy != 0);
				ref->zoneCommands[1].sendFlowRightwards = (dmy != 0);
				ref->zoneCommands[2].sendFlowRightwards = (dmy != 0);
				ref->zoneCommands[3].sendFlowRightwards = (dmy != 0);
				ref->zoneCommands[4].sendFlowRightwards = (dmy != 0);
				ref->zoneCommands[5].sendFlowRightwards = (dmy != 0);
			}
			else
				ref->zoneCommands[zone - 1].sendFlowRightwards = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowRightwards of " << (zone == 10 ? "all" : std::to_string(zone))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handleZoneFlowUpwardsRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (zone == 10)
			{
				ref->zoneCommands[0].sendFlowUpwards = (dmy != 0);
				ref->zoneCommands[1].sendFlowUpwards = (dmy != 0);
				ref->zoneCommands[2].sendFlowUpwards = (dmy != 0);
				ref->zoneCommands[3].sendFlowUpwards = (dmy != 0);
				ref->zoneCommands[4].sendFlowUpwards = (dmy != 0);
				ref->zoneCommands[5].sendFlowUpwards = (dmy != 0);
			}
			else
				ref->zoneCommands[zone - 1].sendFlowUpwards = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowUpwards of " << (zone == 10 ? "all" : std::to_string(zone))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handleZoneFlowDownwardsRequest(mc::command::MECommandBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (zone == 10)
			{
				ref->zoneCommands[0].sendFlowDownwards = (dmy != 0);
				ref->zoneCommands[1].sendFlowDownwards = (dmy != 0);
				ref->zoneCommands[2].sendFlowDownwards = (dmy != 0);
				ref->zoneCommands[3].sendFlowDownwards = (dmy != 0);
				ref->zoneCommands[4].sendFlowDownwards = (dmy != 0);
				ref->zoneCommands[5].sendFlowDownwards = (dmy != 0);
			}
			else
				ref->zoneCommands[zone - 1].sendFlowDownwards = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowDownwards of " << (zone == 10 ? "all" : std::to_string(zone))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		// ==========================================================================================================================
		//
		// listener object that maps the received osc messages to ME handler functions
		//
		// ==========================================================================================================================


		void MusicEnvironmentListener::ProcessMessage(const osc::ReceivedMessage& message,
			const IpEndpointName& remoteEndpoint)
		{
			(void)remoteEndpoint; // suppress unused parameter warning

			try
			{
				std::string address = message.AddressPattern();
				osc::ReceivedMessageArgumentStream args = message.ArgumentStream();

				for (auto&& c = 0; c < list.pattern.size(); ++c)
				{
					auto id = checkSample(address, list.pattern[c], offset, mc::defines::delimiter, mc::defines::placeholder);

					if (id != std::string::npos)
						handlers[c](args, id);
				}
			}
			catch (...)
			{
				++error;
			}
		}


		// ==========================================================================================================================
		//
		// receiver object that handles the incoming osc messages from ME
		//
		// ==========================================================================================================================


		void MusicEnvironmentReceiver::receiveFromNetwork()
		{
			socket.reset(new UdpListeningReceiveSocket(
				IpEndpointName(IpEndpointName::ANY_ADDRESS, port),
				&listener));

			socket->Run();
		}
	}
}