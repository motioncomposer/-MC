
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


		void handleInitRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t)
		{
			std::unique_lock<std::mutex> lck(ref->mtx);
			ref->state = 1;
			ref->cond.notify_one();

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- received init command" << std::endl;
#endif
		}


		void handleStopRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t)
		{
			std::unique_lock<std::mutex> lck(ref->mtx);
			ref->state = -1;
			ref->cond.notify_one();

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- received stop command" << std::endl;
#endif
		}


		void handlePlayerBlobRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				std::lock_guard<std::mutex> guard(ref->mtx);
				ref->player[0].time = std::chrono::high_resolution_clock::now();
				ref->player[1].time = ref->player[0].time;
				ref->player[0].blob = dmy;
				ref->player[1].blob = dmy;
			}
			else if(player == 0)
			{
				std::lock_guard<std::mutex> guard(ref->mtx);

				if (ref->player[1].blob < 0)
				{
					ref->player[0].time = std::chrono::high_resolution_clock::now();
					ref->player[0].blob = dmy;
				}
				
			}
			else if (player == 1 || player == 2)
			{
				std::lock_guard<std::mutex> guard(ref->mtx);

				if (ref->player[0].blob >= 0 && ref->player[1].blob >= 0)
				{
					ref->player[player - 1].time = std::chrono::high_resolution_clock::now();
					ref->player[player - 1].blob = dmy;

					if (ref->player[0].blob < 0)
					{
						std::swap(ref->player[0].time, ref->player[1].time);
						std::swap(ref->player[0].blob, ref->player[1].blob);
					}
				}
			}


#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- received player blob command" << std::endl;
			std::cout << "-- blob player: " << (player == 10 ? "all" : std::to_string(player)) << " is " << dmy << std::endl;
#endif
		}


		void handlePlayerTrackingRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				std::lock_guard<std::mutex> guard(ref->mtx);
				ref->player[0].tracking = (dmy != 0);
				ref->player[1].tracking = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
			{
				std::lock_guard<std::mutex> guard(ref->mtx);
				ref->player[(player > 0 ? player - 1 : player)].tracking = (dmy != 0);
			}

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- received player tracking command" << std::endl;
			std::cout << "-- tracking player: " << (player == 10 ? "all" : std::to_string(player)) << " switched to " << (dmy != 0 ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerResetRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			// here we need something ... as discussed with robert ...
			// we need a special structure PlayerReset = std::array<...>;
			// same procedure like for activation ...
		}


		void handleZoneBlobRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			int32_t dmy[4];
			args >> dmy[0] >> dmy[1] >> dmy[2] >> dmy[3] >> osc::EndMessage;


			if (zone == 10)
			{
				std::lock_guard<std::mutex> guard(ref->mtx);
				ref->zone[0].blob = cv::Rect(dmy[0], dmy[1], dmy[2], dmy[3]);
				ref->zone[1].blob = cv::Rect(dmy[0], dmy[1], dmy[2], dmy[3]);
				ref->zone[2].blob = cv::Rect(dmy[0], dmy[1], dmy[2], dmy[3]);
				ref->zone[3].blob = cv::Rect(dmy[0], dmy[1], dmy[2], dmy[3]);
				ref->zone[4].blob = cv::Rect(dmy[0], dmy[1], dmy[2], dmy[3]);
				ref->zone[5].blob = cv::Rect(dmy[0], dmy[1], dmy[2], dmy[3]);
			}
			else if (zone == 0 || zone == 1 || zone == 2 || zone == 3 || zone == 4 || zone == 5)
			{
				std::lock_guard<std::mutex> guard(ref->mtx);
				ref->zone[zone].blob = cv::Rect(dmy[0], dmy[1], dmy[2], dmy[3]);
			}

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- received zone tracking command" << std::endl;
			std::cout << "-- tracking zone: " << (zone == 10 ? "all" : std::to_string(zone)) << " switched to " << (dmy != 0 ? "on" : "off") << std::endl;
#endif
		}


		void handleZoneTrackingRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;


			if (zone == 10)
			{
				std::lock_guard<std::mutex> guard(ref->mtx);
				ref->zone[0].tracking = (dmy != 0);
				ref->zone[1].tracking = (dmy != 0);
				ref->zone[2].tracking = (dmy != 0);
				ref->zone[3].tracking = (dmy != 0);
				ref->zone[4].tracking = (dmy != 0);
				ref->zone[5].tracking = (dmy != 0);
			}
			else if (zone == 0 || zone == 1 || zone == 2 || zone == 3 || zone == 4 || zone == 5)
			{
				std::lock_guard<std::mutex> guard(ref->mtx);
				ref->zone[zone].tracking = (dmy != 0);
			}

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- received zone tracking command" << std::endl;
			std::cout << "-- tracking zone: " << (zone == 10 ? "all" : std::to_string(zone)) << " switched to " << (dmy != 0 ? "on" : "off") << std::endl;
#endif
		}

		
		void handleZoneResetRequest(mc::structures::ControlBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			// actually we do not need a zone reset as at the moment there are no values learned by the system
			// for each zone ...
			// thats why i think we can remove this ...
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


		void handlePlayerDiscreteHandLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendDiscreteHandLeft = (dmy != 0);
				ref->player[1].sendDiscreteHandLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendDiscreteHandLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteHandLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerDiscreteHandRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendDiscreteHandRight = (dmy != 0);
				ref->player[1].sendDiscreteHandRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendDiscreteHandRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteHandRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerDiscreteHeadRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendDiscreteHead = (dmy != 0);
				ref->player[1].sendDiscreteHead = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendDiscreteHead = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteHead of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerDiscreteLegLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendDiscreteLegLeft = (dmy != 0);
				ref->player[1].sendDiscreteLegLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendDiscreteLegLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteLegLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerDiscreteLegRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendDiscreteLegRight = (dmy != 0);
				ref->player[1].sendDiscreteLegRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendDiscreteLegRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteLegRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerDiscreteBodyUpperRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendDiscreteBodyUpper = (dmy != 0);
				ref->player[1].sendDiscreteBodyUpper = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendDiscreteBodyUpper = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteBodyUpper of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerDiscreteBodyLowerRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendDiscreteBodyLower = (dmy != 0);
				ref->player[1].sendDiscreteBodyLower = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendDiscreteBodyLower = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteBodyLower of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerDiscreteBodyLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendDiscreteBodyLeft = (dmy != 0);
				ref->player[1].sendDiscreteBodyLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendDiscreteBodyLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteBodyLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerDiscreteBodyRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendDiscreteBodyRight = (dmy != 0);
				ref->player[1].sendDiscreteBodyRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendDiscreteBodyRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscreteBodyRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalHandLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendNormalHandLeft = (dmy != 0);
				ref->player[1].sendNormalHandLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendNormalHandLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalHandLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalHandRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendNormalHandRight = (dmy != 0);
				ref->player[1].sendNormalHandRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendNormalHandRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalHandRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalHeadRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendNormalHead = (dmy != 0);
				ref->player[1].sendNormalHead = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendNormalHead = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalHead of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalLegLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendNormalLegLeft = (dmy != 0);
				ref->player[1].sendNormalLegLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendNormalLegLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalLegLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalLegRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendNormalLegRight = (dmy != 0);
				ref->player[1].sendNormalLegRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendNormalLegRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalLegRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalBodyUpperRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendNormalBodyUpper = (dmy != 0);
				ref->player[1].sendNormalBodyUpper = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendNormalBodyUpper = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalBodyUpper of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalBodyLowerRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendNormalBodyLower = (dmy != 0);
				ref->player[1].sendNormalBodyLower = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendNormalBodyLower = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalBodyLower of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalBodyLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendNormalBodyLeft = (dmy != 0);
				ref->player[1].sendNormalBodyLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendNormalBodyLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalBodyLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerNormalBodyRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendNormalBodyRight = (dmy != 0);
				ref->player[1].sendNormalBodyRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendNormalBodyRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormalBodyRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		//////////////////////////////////////////////
		///////////////////////////////////////////////

		// this needs to be re-ordered in case we really move peak to position ...

		void handlePlayerPeakRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendPositionPeak = (dmy != 0);
				ref->player[1].sendPositionPeak = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendPositionPeak = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPeak of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		////////////////////////////////////////////////
		///////////////////////////////////////////////////


		void handlePlayerFlowLeftwardsLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendFlowLeftwardsLeft = (dmy != 0);
				ref->player[1].sendFlowLeftwardsLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendFlowLeftwardsLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowLeftwardsLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerFlowLeftwardsRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendFlowLeftwardsRight = (dmy != 0);
				ref->player[1].sendFlowLeftwardsRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendFlowLeftwardsRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowLeftwardsRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerFlowRightwardsLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendFlowRightwardsLeft = (dmy != 0);
				ref->player[1].sendFlowRightwardsLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendFlowRightwardsLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowRightwardsLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerFlowRightwardsRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendFlowRightwardsRight = (dmy != 0);
				ref->player[1].sendFlowRightwardsRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendFlowRightwardsRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowRightwardsRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerFlowUpwardsLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendFlowUpwardsLeft = (dmy != 0);
				ref->player[1].sendFlowUpwardsLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendFlowUpwardsLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowUpwardsLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerFlowUpwardsRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendFlowUpwardsRight = (dmy != 0);
				ref->player[1].sendFlowUpwardsRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendFlowUpwardsRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowUpwardsRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerFlowDownwardsLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendFlowDownwardsLeft = (dmy != 0);
				ref->player[1].sendFlowDownwardsLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendFlowDownwardsLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowDownwardsLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerFlowDownwardsRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendFlowDownwardsRight = (dmy != 0);
				ref->player[1].sendFlowDownwardsRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendFlowDownwardsRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowDownwardsRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerLocationReadyRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendLocationReady = (dmy != 0);
				ref->player[1].sendLocationReady = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendLocationReady = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendLocationReady of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerLocationPresentRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendLocationPresent = (dmy != 0);
				ref->player[1].sendLocationPresent = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendLocationPresent = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendLocationPresent of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerLocationCenterXRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendLocationCenterX = (dmy != 0);
				ref->player[1].sendLocationCenterX = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendLocationCenterX = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendLocationCenterX of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerLocationCenterZRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendLocationCenterZ = (dmy != 0);
				ref->player[1].sendLocationCenterZ = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendLocationCenterZ = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendLocationCenterZ of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerLocationOutOfRangeRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendLocationOutOfRange = (dmy != 0);
				ref->player[1].sendLocationOutOfRange = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendLocationOutOfRange = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendLocationOutOfRange of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionHeightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendPositionHeight = (dmy != 0);
				ref->player[1].sendPositionHeight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendPositionHeight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionHeight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionHeightLevelRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendPositionHeightLevel = (dmy != 0);
				ref->player[1].sendPositionHeightLevel = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendPositionHeightLevel = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionHeightLevel of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionVerticalHandLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendPositionVerticalHandLeft = (dmy != 0);
				ref->player[1].sendPositionVerticalHandLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendPositionVerticalHandLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionVerticalHandLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionVerticalHandRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendPositionVerticalHandRight = (dmy != 0);
				ref->player[1].sendPositionVerticalHandRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendPositionVerticalHandRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionVerticalHandRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionSideHandLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendPositionSideHandLeft = (dmy != 0);
				ref->player[1].sendPositionSideHandLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendPositionSideHandLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionSideHandLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionSideHandRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendPositionSideHandRight = (dmy != 0);
				ref->player[1].sendPositionSideHandRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendPositionSideHandRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionSideHandRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionSideFootLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendPositionSideFootLeft = (dmy != 0);
				ref->player[1].sendPositionSideFootLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendPositionSideFootLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionSideFootLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionSideFootRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendPositionSideFootRight = (dmy != 0);
				ref->player[1].sendPositionSideFootRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendPositionSideFootRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionSideFootRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionFrontHandLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendPositionFrontHandLeft = (dmy != 0);
				ref->player[1].sendPositionFrontHandLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendPositionFrontHandLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionFrontHandLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionFrontHandRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendPositionFrontHandRight = (dmy != 0);
				ref->player[1].sendPositionFrontHandRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendPositionFrontHandRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionFrontHandRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionFrontFootLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendPositionFrontFootLeft = (dmy != 0);
				ref->player[1].sendPositionFrontFootLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendPositionFrontFootLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionFrontFootLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionFrontFootRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendPositionFrontFootRight = (dmy != 0);
				ref->player[1].sendPositionFrontFootRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendPositionFrontFootRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionFrontFootRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerPositionWidthRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendPositionWidth = (dmy != 0);
				ref->player[1].sendPositionWidth = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendPositionWidth = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendPositionWidth of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureHitOverheadRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendGestureHitOverhead = (dmy != 0);
				ref->player[1].sendGestureHitOverhead = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendGestureHitOverhead = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureHitOverhead of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureHitSideLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendGestureHitSideLeft = (dmy != 0);
				ref->player[1].sendGestureHitSideLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendGestureHitSideLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureHitSideLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureHitSideRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendGestureHitSideRight = (dmy != 0);
				ref->player[1].sendGestureHitSideRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendGestureHitSideRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureHitSideRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureHitDownLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendGestureHitDownLeft = (dmy != 0);
				ref->player[1].sendGestureHitDownLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendGestureHitDownLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureHitDownLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureHitDownRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendGestureHitDownRight = (dmy != 0);
				ref->player[1].sendGestureHitDownRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendGestureHitDownRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureHitDownRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureHitForwardLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendGestureHitForwardLeft = (dmy != 0);
				ref->player[1].sendGestureHitForwardLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendGestureHitForwardLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureHitForwardLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureHitForwardRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendGestureHitForwardRight = (dmy != 0);
				ref->player[1].sendGestureHitForwardRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendGestureHitForwardRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureHitForwardRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureKickSideLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendGestureKickSideLeft = (dmy != 0);
				ref->player[1].sendGestureKickSideLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendGestureKickSideLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureKickSideLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureKickSideRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendGestureKickSideRight = (dmy != 0);
				ref->player[1].sendGestureKickSideRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendGestureKickSideRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureKickSideRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureKickForwardLeftRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendGestureKickForwardLeft = (dmy != 0);
				ref->player[1].sendGestureKickForwardLeft = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendGestureKickForwardLeft = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureKickForwardLeft of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureKickForwardRightRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendGestureKickForwardRight = (dmy != 0);
				ref->player[1].sendGestureKickForwardRight = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendGestureKickForwardRight = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureKickForwardRight of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureDoubleArmSideRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendGestureDoubleArmSide = (dmy != 0);
				ref->player[1].sendGestureDoubleArmSide = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendGestureDoubleArmSide = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureDoubleArmSide of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureDoubleArmSideCloseRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendGestureDoubleArmSideClose = (dmy != 0);
				ref->player[1].sendGestureDoubleArmSideClose = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendGestureDoubleArmSideClose = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureDoubleArmSideClose of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureJumpRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendGestureJump = (dmy != 0);
				ref->player[1].sendGestureJump = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendGestureJump = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureJump of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handlePlayerGestureClapRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t player)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (player == 10)
			{
				ref->player[0].sendGestureClap = (dmy != 0);
				ref->player[1].sendGestureClap = (dmy != 0);
			}
			else if (player == 0 || player == 1 || player == 2)
				ref->player[(player > 0 ? player - 1 : player)].sendGestureClap = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendGestureClap of " << (player == 10 ? "all" : std::to_string(player))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handleZoneDiscreteRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (zone == 10)
			{
				ref->zone[0].sendDiscrete = (dmy != 0);
				ref->zone[1].sendDiscrete = (dmy != 0);
				ref->zone[2].sendDiscrete = (dmy != 0);
				ref->zone[3].sendDiscrete = (dmy != 0);
				ref->zone[4].sendDiscrete = (dmy != 0);
				ref->zone[5].sendDiscrete = (dmy != 0);
			}
			else if (zone == 0 || zone == 1 || zone == 2 || zone == 3 || zone == 4 || zone == 5)
				ref->zone[zone].sendDiscrete = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendDiscrete of " << (zone == 10 ? "all" : std::to_string(zone))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handleZoneNormalRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (zone == 10)
			{
				ref->zone[0].sendNormal = (dmy != 0);
				ref->zone[1].sendNormal = (dmy != 0);
				ref->zone[2].sendNormal = (dmy != 0);
				ref->zone[3].sendNormal = (dmy != 0);
				ref->zone[4].sendNormal = (dmy != 0);
				ref->zone[5].sendNormal = (dmy != 0);
			}
			else if (zone == 0 || zone == 1 || zone == 2 || zone == 3 || zone == 4 || zone == 5)
				ref->zone[zone].sendNormal = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendNormal of " << (zone == 10 ? "all" : std::to_string(zone))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handleZoneFlowLeftwardsRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (zone == 10)
			{
				ref->zone[0].sendFlowLeftwards = (dmy != 0);
				ref->zone[1].sendFlowLeftwards = (dmy != 0);
				ref->zone[2].sendFlowLeftwards = (dmy != 0);
				ref->zone[3].sendFlowLeftwards = (dmy != 0);
				ref->zone[4].sendFlowLeftwards = (dmy != 0);
				ref->zone[5].sendFlowLeftwards = (dmy != 0);
			}
			else if (zone == 0 || zone == 1 || zone == 2 || zone == 3 || zone == 4 || zone == 5)
				ref->zone[zone].sendFlowLeftwards = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowLeftwards of " << (zone == 10 ? "all" : std::to_string(zone))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handleZoneFlowRightwardsRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (zone == 10)
			{
				ref->zone[0].sendFlowRightwards = (dmy != 0);
				ref->zone[1].sendFlowRightwards = (dmy != 0);
				ref->zone[2].sendFlowRightwards = (dmy != 0);
				ref->zone[3].sendFlowRightwards = (dmy != 0);
				ref->zone[4].sendFlowRightwards = (dmy != 0);
				ref->zone[5].sendFlowRightwards = (dmy != 0);
			}
			else if (zone == 0 || zone == 1 || zone == 2 || zone == 3 || zone == 4 || zone == 5)
				ref->zone[zone].sendFlowRightwards = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowRightwards of " << (zone == 10 ? "all" : std::to_string(zone))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handleZoneFlowUpwardsRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (zone == 10)
			{
				ref->zone[0].sendFlowUpwards = (dmy != 0);
				ref->zone[1].sendFlowUpwards = (dmy != 0);
				ref->zone[2].sendFlowUpwards = (dmy != 0);
				ref->zone[3].sendFlowUpwards = (dmy != 0);
				ref->zone[4].sendFlowUpwards = (dmy != 0);
				ref->zone[5].sendFlowUpwards = (dmy != 0);
			}
			else if (zone == 0 || zone == 1 || zone == 2 || zone == 3 || zone == 4 || zone == 5)
				ref->zone[zone].sendFlowUpwards = (dmy != 0);

#ifdef MC_RECEIVING_VERBOSE
			std::cout << "-- sendFlowUpwards of " << (zone == 10 ? "all" : std::to_string(zone))
				<< " set to: " << ((dmy != 0) ? "on" : "off") << std::endl;
#endif
		}


		void handleZoneFlowDownwardsRequest(mc::structures::MusicBundle* ref, osc::ReceivedMessageArgumentStream args, size_t zone)
		{
			int32_t dmy;
			args >> dmy >> osc::EndMessage;

			if (zone == 10)
			{
				ref->zone[0].sendFlowDownwards = (dmy != 0);
				ref->zone[1].sendFlowDownwards = (dmy != 0);
				ref->zone[2].sendFlowDownwards = (dmy != 0);
				ref->zone[3].sendFlowDownwards = (dmy != 0);
				ref->zone[4].sendFlowDownwards = (dmy != 0);
				ref->zone[5].sendFlowDownwards = (dmy != 0);
			}
			else if (zone == 0 || zone == 1 || zone == 2 || zone == 3 || zone == 4 || zone == 5)
				ref->zone[zone].sendFlowDownwards = (dmy != 0);

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