#include "stdafx.h"

#include "Database.h"

/*
Function:		Database::Database
Parameters:		string <the filename to use>
Return Value:	this <its self>
Description:	Creates the connection to the database.
*/
Database::Database(std::string filename) {
	// Requires a char array as is a C standard library.
	int rC = sqlite3_open(filename.c_str(), &db_);
	// If rC was anything other than 0, we failed to open the DB.
	if (rC) {
		printf("WARNING: Failed to connect to speficied database. (%s)\n",
			sqlite3_errmsg(db_));
	} else {
		printf("INFO: Successfully connection to the database.\n");
		constructTables();
	}
}

/*
Function:		Database::~Database
Parameters:		None
Return Value:	None
Description:	Destroys the connection and saves the data.
*/
Database::~Database() {
	sqlite3_close(db_);
}

/*
Function:		Database::constructTables
Parameters:		None
Return Value:	None
Description:	Constructs all Tables needed for the database.
*/
void Database::constructTables(void) {
	sqlite3_exec(db_, constructionOfTables_.c_str(), 0, 0, &errorMessage_);
}

/*
Function:		Database::exists
Parameters:		string <a username>, string <a discriminator>
Return Value:	bool <was the user existent>
Description:	Returns true or false if the user is existent.
*/
bool Database::exists(std::string username, std::string discriminator) {
	bool * exists = new bool(false);
	char * buffer = new char[59 + username.length() + discriminator.length()];
	sprintf(buffer, existsStatement_.c_str(), username.c_str(), discriminator.c_str());
	sqlite3_exec(db_, buffer, existsCallback, (void *)exists, &errorMessage_);
	return *exists;
}

/*
Function:		Database::existsCallback
Parameters:		...
Return Value:	int
Description:	Handles the sqlite exec and modifies vExists accordingly.
*/
int Database::existsCallback(void * vExists, int argc, char ** argv, char ** azColName) {
	bool * exists = (bool *)vExists;
	if (argc) {
		*exists = true;
	} else {
		*exists = false;
	}
	return 0;
}
