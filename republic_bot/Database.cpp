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
	sqlite3_exec(db_, constructionOfTables_.c_str(), NULL, NULL, NULL);
}

/*
Function:		Database::insertUser
Parameters:		None
Return Value:	None
Description:	Adds a new user to the database.
*/
void Database::insertUser(std::string username, std::string discriminator) {
	// Generate our sql query using the username and discriminator of the user.
	char * buffer = new char[insertStatement_.length() + username.length() + discriminator.length()];
	sprintf(buffer, insertStatement_.c_str(), username.c_str(), discriminator.c_str());
	// Execute the statement without result handling.
	sqlite3_exec(db_, buffer, NULL, NULL, NULL);
}

/*
Function:		Database::exists
Parameters:		string <a username>, string <a discriminator>
Return Value:	bool <was the user existent>
Description:	Returns true or false if the user is existent.
*/
bool Database::exists(std::string username, std::string discriminator) {
	// Generate our sql query using the username and discriminator of the user.
	char * buffer = new char[existsStatement_.length() + username.length() + discriminator.length()];
	sprintf(buffer, existsStatement_.c_str(), username.c_str(), discriminator.c_str());
	// Are there any rows returned? If so the user was existent.
	if (sqlite3_prepare_v2(db_, buffer, -1, &stmt, NULL) == SQLITE_OK) {
		if (int res = sqlite3_step(stmt) == SQLITE_ROW)
			return true;
	}
	return false;
}

/*
Function:		Database::getIntFromUser
Parameters:		string <a username>, string <a discriminator>, string <a detail to fetch>
Return Value:	int
Description:	Retrieves a column value from the database relative to a user.
*/
int Database::getIntFromUser(std::string username, std::string discriminator, std::string column)
{
	char * buffer = new char[getFromStatement_.length() + username.length() + discriminator.length() + column.length()];
	sprintf(buffer, getFromStatement_.c_str(), column.c_str(), username.c_str(), discriminator.c_str());
	// Are there any rows returned? If so the user was existent.
	if (sqlite3_prepare_v2(db_, buffer, -1, &stmt, NULL) == SQLITE_OK) {
		if (int res = sqlite3_step(stmt) == SQLITE_ROW) {
			return sqlite3_column_int(stmt, 0);
		} else {
			return 0;
		}
		sqlite3_finalize(stmt);
	} else {
		return 0;
	}
}

/*
Function:		Database::getStrFromUser
Parameters:		string <a username>, string <a discriminator>, string <a detail to fetch>
Return Value:	char *
Description:	Retrieves a column value from the database relative to a user.
*/
char * Database::getStrFromUser(std::string username, std::string discriminator, std::string column)
{
	char * buffer = new char[getFromStatement_.length() + username.length() + discriminator.length() + column.length()];
	sprintf(buffer, getFromStatement_.c_str(), column.c_str(), username.c_str(), discriminator.c_str());
	// Are there any rows returned? If so the user was existent.
	if (sqlite3_prepare_v2(db_, buffer, -1, &stmt, NULL) == SQLITE_OK) {
		if (int res = sqlite3_step(stmt) == SQLITE_ROW) {
			return (char *)sqlite3_column_text(stmt, 0);
		}
		else {
			return nullptr;
		}
		sqlite3_finalize(stmt);
	}
	else {
		return nullptr;
	}
}

/*
Function:		Database::setIntFromUser
Parameters:		string <a username>, string <a discriminator>, string <a detail to fetch>, int <value to set>
Return Value:	None
Description:	Sets a integer column value to the database relative to a user.
*/
void Database::setIntForUser(std::string username, std::string discriminator, std::string column, int value) {
	// Generate our sql query using the username and discriminator of the user.
	char * buffer = new char[setFromStatementInt_.length() + username.length() + discriminator.length() + column.length() + 10]();
	sprintf(buffer, setFromStatementInt_.c_str(), column.c_str(), std::to_string(value).c_str(), username.c_str(), discriminator.c_str());
	// Execute the statement without result handling.
	sqlite3_exec(db_, buffer, NULL, NULL, NULL);
}

/*
Function:		Database::setStrFromUser
Parameters:		string <a username>, string <a discriminator>, string <a detail to fetch>, string <value to set>
Return Value:	None
Description:	Sets a text column value to the database relative to a user.
*/
void Database::setStrForUser(std::string username, std::string discriminator, std::string column, std::string value) {
	// Generate our sql query using the username and discriminator of the user.
	char * buffer = new char[setFromStatementString_.length() + username.length() + discriminator.length() + column.length() + value.length()]();
	sprintf(buffer, setFromStatementString_.c_str(), column.c_str(), value.c_str(), username.c_str(), discriminator.c_str());
	// Execute the statement without result handling.
	sqlite3_exec(db_, buffer, NULL, NULL, NULL);
}
