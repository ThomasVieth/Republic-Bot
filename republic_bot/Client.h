#pragma once

#include "Database.h"

#include <sleepy_discord\websocketpp_websocket.h>
#include <vector>

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
		std::string showBalanceString_ = "Your ELO balance is: %d ELO";
		std::string giveEloString_ = "You have given %d ELO to "; // The targets are added to the end of the string.
		std::string giveEloArgFailedString_ = "Please supply atleast 1 person and an amount.";

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

		/*
		Function:		Client::addUserIfNotExists
		Parameters:		SleepyDiscord::User <the user to check>
		Return Value:	None
		Description:	Try to add the user to the database if needed.
		*/
		void addUserIfNotExists(User user);

		/*
		Function:		Client::displayBalance
		Parameters:		SleepyDiscord::Message <the message to check>
		Return Value:	None
		Description:	Show the requesters ELO balance.
		*/
		void displayBalance(Message message);

		/*
		Function:		Client::giveElo
		Parameters:		SleepyDiscord::Message <the message to check>
		Return Value:	None
		Description:	Give the users mentioned the amount of ELO.
		*/
		void giveElo(Message message, int toAdd);
};
