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
	printf("User %s exists? %d",
		message.author.username.c_str(),
		db_->exists(message.author.username, message.author.discriminator));
	if (message.startsWith("!hello")) {
		sendMessage(message.channelID, "Hello " + message.author.username);
	}
}