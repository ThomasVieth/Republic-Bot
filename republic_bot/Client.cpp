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
	// Just improves efficiency.
	if (!message.startsWith("!")) {
		return;
	}
	// Retrieving the tokens to use by splitting the message.
	std::istringstream buf(message.content);
	std::istream_iterator<std::string> beg(buf), end;
	std::vector<std::string> tokens(beg, end);
	// !balance
	if (tokens[0].compare("!balance") == 0) {
		displayBalance(message);
	}
	// !giveelo
	else if (tokens[0].compare("!giveelo") == 0) {
		// Retrieve the amount to give from the tokens.
		if (tokens.size() > 2) {
			int toAdd = atoi(tokens[tokens.size() - 1].c_str());
			giveElo(message, toAdd);
		} else {
			message.reply(this, giveEloArgFailedString_);
		}
	}
	// !faction
	else if (tokens[0].compare("!faction") == 0) {
		if (tokens.size() == 1) {
			displayFaction(message);
		} else {
			std::string faction = "";
			for (int i = 1; i < tokens.size(); i++) {
				faction += tokens[i];
				if (i < tokens.size() - 1) {
					faction += "_";
				}
			}
			if (std::find(allFactions.begin(), allFactions.end(), faction) != allFactions.end()) {
				changeFaction(message, faction);
			} else {
				message.reply(this, showFalseFactionString_);
			}
		}
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

/*
Function:		Client::displayFaction
Parameters:		SleepyDiscord::Message <the message to check>
Return Value:	None
Description:	Show the requesters faction.
*/
void Client::displayFaction(Message message) {
	// Check if the user is existent. If not add their data.
	addUserIfNotExists(message.author);
	// Check for the users balance.
	char * value = db_->getStrFromUser(message.author.username, message.author.discriminator, "FACTION");
	// Setting up the message to be sent back.
	char * buffer = new char[showFactionString_.length() + 10]();
	sprintf(buffer, showFactionString_.c_str(), value);
	message.reply(this, strcmp(value, "") != 0 ? buffer : showNoFactionString_);
}

/*
Function:		Client::changeFaction
Parameters:		SleepyDiscord::Message <the message to check>
Return Value:	None
Description:	Changes the requesters faction.
*/
void Client::changeFaction(Message message, std::string faction) {
	// Check if the user is existent. If not add their data.
	addUserIfNotExists(message.author);
	// Setting up the users new faction.
	db_->setStrForUser(message.author.username, message.author.discriminator, "FACTION", faction);
	// Setting up the message to be sent back.
	char * buffer = new char[showFactionChangeString_.length() + faction.length()]();
	sprintf(buffer, showFactionChangeString_.c_str(), faction.c_str());
	message.reply(this, buffer);
}

/*
Function:		Client::displayBalance
Parameters:		SleepyDiscord::Message <the message to check>
Return Value:	None
Description:	Show the requesters ELO balance.
*/
void Client::displayBalance(Message message) {
	// Check if the user is existent. If not add their data.
	addUserIfNotExists(message.author);
	// Check for the users balance.
	int value = db_->getIntFromUser(message.author.username, message.author.discriminator, "BALANCE");
	// Setting up the message to be sent back.
	char * buffer = new char[showBalanceString_.length() + 10]();
	sprintf(buffer, showBalanceString_.c_str(), value);
	message.reply(this, buffer);
}

/*
Function:		Client::giveElo
Parameters:		SleepyDiscord::Message <the message to check>
Return Value:	None
Description:	Give the users mentioned the amount of ELO.
*/
void Client::giveElo(Message message, int toAdd) {
	// Create the char array buffer to format into, ensure its size.
	int size = message.mentions.size();
	char * cBuffer = new char[showBalanceString_.length() + (size * 64)]();
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
		else {
			buffer += ".";
		}
	}
	message.reply(this, buffer);
}