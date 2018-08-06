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
		sqlite3_stmt *stmt;
		char * errorMessage_;
		std::string constructionOfTables_ = "CREATE TABLE Users (" \
											 "NAME			TEXT	NOT NULL," \
											 "DISCRIMINATOR	INT		NOT NULL," \
											 "BALANCE		INT," \
											 "FACTION		TEXT," \
											 "UNMUTE_TIME	INT," \
											 "PRIMARY KEY (NAME, DISCRIMINATOR)" \
											 ");";
		std::string insertStatement_ = "INSERT INTO Users VALUES ('%s', %s, 0, '', 0)";
		std::string existsStatement_ = "SELECT * FROM Users WHERE NAME = '%s' AND DISCRIMINATOR = '%s'";
		std::string getFromStatement_ = "SELECT %s FROM Users WHERE NAME = '%s' AND DISCRIMINATOR = '%s'";
		std::string setFromStatementInt_ = "UPDATE Users SET %s=%s WHERE NAME = '%s' AND DISCRIMINATOR = '%s'";
		std::string setFromStatementString_ = "UPDATE Users SET %s='%s' WHERE NAME = '%s' AND DISCRIMINATOR = '%s'";

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
		Function:		Database::insertUser
		Parameters:		string <a username>, string <a discriminator>
		Return Value:	bool <was statement successful?>
		Description:	Adds a new user to the database.
		*/
		void insertUser(std::string username, std::string discriminator);

		/*
		Function:		Database::exists
		Parameters:		string <a username>, string <a discriminator>
		Return Value:	bool <was the user existent>
		Description:	Returns true or false if the user is existent.
		*/
		bool exists(std::string username, std::string discriminator);

		/*
		Function:		Database::getIntFromUser
		Parameters:		string <a username>, string <a discriminator>, string <a detail to fetch>
		Return Value:	int
		Description:	Retrieves a integer column value from the database relative to a user.
		*/
		int getIntFromUser(std::string username, std::string discriminator, std::string column);

		/*
		Function:		Database::getStrFromUser
		Parameters:		string <a username>, string <a discriminator>, string <a detail to fetch>
		Return Value:	char *
		Description:	Retrieves a integer column value from the database relative to a user.
		*/
		char * getStrFromUser(std::string username, std::string discriminator, std::string column);

		/*
		Function:		Database::setIntFromUser
		Parameters:		string <a username>, string <a discriminator>, string <a detail to fetch>, int <value to set>
		Return Value:	None
		Description:	Sets a integer column value to the database relative to a user.
		*/
		void setIntForUser(std::string username, std::string discriminator, std::string column, int value);

		/*
		Function:		Database::setStrFromUser
		Parameters:		string <a username>, string <a discriminator>, string <a detail to fetch>, string <value to set>
		Return Value:	None
		Description:	Sets a text column value to the database relative to a user.
		*/
		void setStrForUser(std::string username, std::string discriminator, std::string column, std::string value);
};

