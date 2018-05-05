#pragma once

#include "Database.h"

#include <sleepy_discord\websocketpp_websocket.h>

using namespace SleepyDiscord;

/*
Function:		Client <class>
Parameters:		string <the bot token>, int <amount of threads to use>
Return Value:	this <its self>
Description:	Subclass of the Discord API being used.
*/
class Client : public DiscordClient {
	private:
		Database * db_;

	public:
		// Used to initialise the DiscordClient before overloading methods.
		using DiscordClient::DiscordClient;

		/*
		Function:		Client::Client
		Parameters:		string <the discord bot token>, int <threads to be used>, Database <
		Return Value:	this <its self>
		Description:	Constructor overloading default to add database features.
		*/
		Client(std::string token, int threads, std::string dbLoc);

		/*
		Function:		Client::onMessage
		Parameters:		SleepyDiscord::Message <message context>
		Return Value:	None
		Description:	Event called upon a message being created.
		*/
		void onMessage(Message message);
};
