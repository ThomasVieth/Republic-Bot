#include "stdafx.h"

#include "Client.h"

/*
Function:		Client::Client
Parameters:		string <the discord bot token>, int <threads to be used>, Database <
Return Value:	None
Description:	Constructor overloading default to add database features.
*/
Client::Client(std::string token, int threads, std::string dbLoc) : DiscordClient(token, threads) {
	db_ = new Database(dbLoc);
}

/*
Function:		Client::onMessage
Parameters:		SleepyDiscord::Message <message context>
Return Value:	None
Description:	Event called upon a message being created.
*/
void Client::onMessage(SleepyDiscord::Message message) {
	// !balance
	if (message.startsWith("!balance")) {
		// Check if the user is existent. If not add their data.
		addUserIfNotExists(message.author);
		// Check for the users balance.
		int value = db_->getIntFromUser(message.author.username, message.author.discriminator, "BALANCE");
		// Setting up the message to be sent back.
		char * buffer = new char[showBalanceString_.length() + 10]();
		sprintf(buffer, showBalanceString_.c_str(), value);
		sendMessage(message.channelID, buffer);
	}
	// !giveelo
	else if (message.startsWith("!giveelo")) {
		int size = message.mentions.size();
		// Setting up message.
		char * cBuffer = new char[showBalanceString_.length() + (size * 64)]();
		// Retrieving the amount to give by splitting the message.
		std::istringstream buf(message.content);
		std::istream_iterator<std::string> beg(buf), end;
		std::vector<std::string> tokens(beg, end);
		int toAdd = atoi(tokens[tokens.size() - 1].c_str());
		// Formatting the addition into the message.
		sprintf(cBuffer, giveEloString_.c_str(), toAdd);
		std::string buffer = cBuffer;
		// Adding for all targets balance and adding to message.
		for (int i = 0; i < size; i++) {
			User target = message.mentions[i];
			// Check if the user is existent. If not add their data.
			addUserIfNotExists(target);
			// Setting up the message to be sent back and setting data.
			int value = db_->getIntFromUser(target.username, target.discriminator, "BALANCE");
			db_->setIntForUser(target.username, target.discriminator, "BALANCE", value + toAdd);
			buffer += target.username.c_str();
			// Just adding extra nice formatting.
			if (i < size - 2) {
				buffer += ", ";
			}
			// Only use add if 1 or more users is specified.
			else if (i == size - 2) {
				buffer += " and ";
			}
		}
		sendMessage(message.channelID, buffer);
	}
}

/*
Function:		Client::addUserIfNotExists
Parameters:		SleepyDiscord::User <the user to check>
Return Value:	None
Description:	Try to add the user to the database if needed.
*/
void Client::addUserIfNotExists(User user) {
	if (!db_->exists(user.username, user.discriminator)) {
		db_->insertUser(user.username, user.discriminator);
	}
}