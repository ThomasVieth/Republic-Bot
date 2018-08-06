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
	commandMap["!balance"] = &Client::displayBalance;
	commandMap["!giveelo"] = &Client::giveElo;
	commandMap["!takeelo"] = &Client::takeElo;
	commandMap["!faction"] = &Client::handleFaction;
	commandMap["!toggle"]  = &Client::handleToggle;
	commandMap["!mute"] = &Client::handleMute;
	commandMap["!unmute"] = &Client::handleUnmute;
	commandMap["!ismuted"] = &Client::handleIsMuted;
}

/*
Function:		Client::onMessage
Parameters:		SleepyDiscord::Message <message context>
Return Value:	None
Description:	Event called upon a message being created.
*/
void Client::onMessage(SleepyDiscord::Message message) {
	// Check if the user is existent. If not add their data.
	addUserIfNotExists(message.author);

	// Just improves efficiency.
	if (!message.startsWith("!")) {
		// Checking for mutes.
		if (isMuted(message.author)) {
			deleteMessage(message.channelID, message.ID);
		}
		return;
	}
	// Retrieving the tokens to use by splitting the message.
	std::istringstream buf(message.content);
	std::istream_iterator<std::string> beg(buf), end;
	std::vector<std::string> tokens(beg, end);
	// Handles all commands.
	if (commandMap.find(tokens[0]) != commandMap.end()) {
		commandMap.at(tokens[0])(this, message, tokens);
		return;
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
Function:		Client::handleToggle
Parameters:		SleepyDiscord::Message <the message to check>
Return Value:	None
Description:	Handle the toggle command.
*/
void Client::handleToggle(Message message, std::vector<std::string> tokens) {
	if (tokens.size() != 2) {
		message.reply(this, toggleFailed_);
	}
	else if (tokens[1].compare("factions") == 0) {
		commandFactionEnabled = !commandFactionEnabled;
		char * buffer = new char[toggleSuccess_.length() + 10]();
		sprintf(buffer, toggleSuccess_.c_str(), "factions", commandFactionEnabled ? "ON" : "OFF");
		message.reply(this, buffer);
	}
	else if (tokens[1].compare("elo") == 0) {
		commandBalanceEnabled = !commandBalanceEnabled;
		char * buffer = new char[toggleSuccess_.length() + 10]();
		sprintf(buffer, toggleSuccess_.c_str(), "ELO", commandBalanceEnabled ? "ON" : "OFF");
		message.reply(this, buffer);
	}
	else {
		message.reply(this, toggleFailed_);
	}
}

/*
Function:		Client::handleFaction
Parameters:		SleepyDiscord::Message <the message to check>
Return Value:	None
Description:	Handle the faction command.
*/
void Client::handleFaction(Message message, std::vector<std::string> tokens) {
	if (!commandFactionEnabled)
		return;

	if (tokens.size() == 1) {
		displayFaction(message);
	}
	else {
		std::string faction = "";
		for (int i = 1; i < tokens.size(); i++) {
			faction += tokens[i];
			if (i < tokens.size() - 1) {
				faction += "_";
			}
		}
		if (std::find(allFactions.begin(), allFactions.end(), faction) != allFactions.end()) {
			changeFaction(message, faction);
		}
		else {
			message.reply(this, showFalseFactionString_);
		}
	}
}

void Client::handleMute(Message message, std::vector<std::string> tokens) {
	if (tokens.size() > 2) {
		if (!isdigit(tokens[tokens.size() - 1][0])) {
			message.reply(this, muteUserFailedString_);
			return;
		}
		int minutes = atoi(tokens[tokens.size() - 1].c_str());
		// Create the char array buffer to format into, ensure its size.
		int size = message.mentions.size();
		// Setting up the message for formatting.
		std::string buffer = muteUserString_;
		// Adding for all targets balance and adding to message.
		for (int i = 0; i < size; i++) {
			User target = message.mentions[i];
			// Check if the user is existent. If not add their data.
			addUserIfNotExists(target);
			// Setting up the message to be sent back and setting data.
			muteUser(target, minutes);
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
	else {
		message.reply(this, muteUserFailedString_);
	}
}

void Client::handleUnmute(Message message, std::vector<std::string> tokens) {
	if (tokens.size() > 1) {
		// Create the char array buffer to format into, ensure its size.
		int size = message.mentions.size();
		// Setting up the message for formatting.
		std::string buffer = unmuteUserString_;
		// Adding for all targets balance and adding to message.
		for (int i = 0; i < size; i++) {
			User target = message.mentions[i];
			// Check if the user is existent. If not add their data.
			addUserIfNotExists(target);
			// Setting up the message to be sent back and setting data.
			unmuteUser(target);
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
	else {
		message.reply(this, unmuteUserFailedString_);
	}
}

void Client::handleIsMuted(Message message, std::vector<std::string> tokens) {
	// Create the char array buffer to format into, ensure its size.
	int size = message.mentions.size();
	// Adding for all targets balance and adding to message.
	for (int i = 0; i < size; i++) {
		char * buffer = new char[isUserMutedString_.length() + 67];
		User target = message.mentions[i];
		bool targetMuted = isMuted(target);
		sprintf(buffer, isUserMutedString_.c_str(), target.username.c_str(), targetMuted ? "Yes" : "No");
		message.reply(this, buffer);
	}
}

/*
Function:		Client::displayFaction
Parameters:		SleepyDiscord::Message <the message to check>
Return Value:	None
Description:	Show the requesters faction.
*/
void Client::displayFaction(Message message) {
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
void Client::displayBalance(Message message, std::vector<std::string> tokens) {
	if (!commandBalanceEnabled)
		return;

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
void Client::giveElo(Message message, std::vector<std::string> tokens) {
	if (tokens.size() > 2) {
		if (!isdigit(tokens[tokens.size() - 1][0])) {
			message.reply(this, giveEloArgFailedString_);
			return;
		}
		int toAdd = atoi(tokens[tokens.size() - 1].c_str());
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
	else {
		message.reply(this, giveEloArgFailedString_);
	}
}

/*
Function:		Client::takeElo
Parameters:		SleepyDiscord::Message <the message to check>
Return Value:	None
Description:	Takes the users mentioned the amount of ELO.
*/
void Client::takeElo(Message message, std::vector<std::string> tokens) {
	if (tokens.size() > 2) {
		if (!isdigit(tokens[tokens.size() - 1][0])) {
			message.reply(this, takeEloArgFailedString_);
			return;
		}
		int toSub = atoi(tokens[tokens.size() - 1].c_str());
		// Create the char array buffer to format into, ensure its size.
		int size = message.mentions.size();
		char * cBuffer = new char[showBalanceString_.length() + (size * 64)]();
		// Formatting the addition into the message.
		sprintf(cBuffer, takeEloString_.c_str(), toSub);
		std::string buffer = cBuffer;
		// Adding for all targets balance and adding to message.
		for (int i = 0; i < size; i++) {
			User target = message.mentions[i];
			// Check if the user is existent. If not add their data.
			addUserIfNotExists(target);
			// Setting up the message to be sent back and setting data.
			int value = db_->getIntFromUser(target.username, target.discriminator, "BALANCE");
			db_->setIntForUser(target.username, target.discriminator, "BALANCE", value - toSub);
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
	else {
		message.reply(this, takeEloArgFailedString_);
	}
}

/*
Function:		Client::muteUser
Parameters:		SleepyDiscord::Message <the message to check>
Return Value:	None
Description:	Mutes the indicated user for a period of time.
*/
void Client::muteUser(User user, int minutes) {
	int epoch = std::time(nullptr);
	int mutedEpoch = epoch + (minutes * 60);
	db_->setIntForUser(user.username, user.discriminator, "UNMUTE_TIME", mutedEpoch);
}

/*
Function:		Client::unmuteUser
Parameters:		SleepyDiscord::Message <the message to check>
Return Value:	None
Description:	Unmutes the indicated user.
*/
void Client::unmuteUser(User user) {
	db_->setIntForUser(user.username, user.discriminator, "UNMUTE_TIME", 0);
}

bool Client::isMuted(User user) {
	int value = db_->getIntFromUser(user.username, user.discriminator, "UNMUTE_TIME");
	int epoch = std::time(nullptr);
	if (value > epoch) {
		return true;
	}
	return false;
}
