#pragma once

#include "stdafx.h"

#include "Client.h"


// Configuration.
std::string TOKEN	= "NDQxNjk5OTQxNDIxODA5NjY2.Dc5HAQ.RG1TwJo7xrjLEz-e69LLIZ123XA";
INT8		THREADS = 2;
std::string DATABASE_LOCATION = ":memory:";

/*
Function:		main
Parameters:		None
Return Value:	int <error code>
Description:	Initial entry point to the program.
*/
int main(void) {
	Client client(TOKEN, THREADS, DATABASE_LOCATION);
	client.run();
}
