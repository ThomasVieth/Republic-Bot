#pragma once

#include <string>
#include <sqlite3.h>

/*
Function:		Database <class>
Parameters:		string <the filename to use>
Return Value:	this <its self>
Description:	Handles all sql manipulation
*/
class Database
{
	private:
		sqlite3 * db_;
		char * errorMessage_;
		std::string constructionOfTables_ = "CREATE TABLE IF NOT EXISTS Users (" \
											 "DISCRIMINATOR	INT		NOT NULL," \
											 "NAME			TEXT	NOT NULL," \
											 "BALANCE		INT," \
											 "PRIMARY KEY (DISCRIMINATOR, NAME)" \
											 ");";
		std::string existsStatement_ = "SELECT * FROM Users WHERE DISCRIMINATOR = %s AND NAME = %s";

	public:
		/*
		Function:		Database::Database
		Parameters:		None
		Return Value:	this <its self>
		Description:	Default constructor.
		*/
		Database(void) {}

		/*
		Function:		Database::Database
		Parameters:		string <the filename to use>
		Return Value:	this <its self>
		Description:	Creates the connection to the database.
		*/
		Database(std::string filename);

		/*
		Function:		Database::~Database
		Parameters:		None
		Return Value:	None
		Description:	Destroys the connection and saves the data.
		*/
		~Database();

		/*
		Function:		Database::constructTables
		Parameters:		None
		Return Value:	None
		Description:	Attempts to construct any tables currently unassembled.
		*/
		void constructTables(void);

		/*
		Function:		Database::exists
		Parameters:		string <a username>, string <a discriminator>
		Return Value:	bool <was the user existent>
		Description:	Returns true or false if the user is existent.
		*/
		bool exists(std::string username, std::string discriminator);

		/*
		Function:		Database::existsCallback
		Parameters:		...
		Return Value:	int
		Description:	Handles the sqlite exec and modifies vExists accordingly.
		*/
		static int existsCallback(void *notUsed, int argc, char ** argv, char ** azColName);
};

