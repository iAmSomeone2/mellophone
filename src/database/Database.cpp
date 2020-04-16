/*
    Copyright 2020 Brenden Davidson

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <iostream>

#include <sqlite_init.h>
#include "Database.hpp"

using namespace mellophone;

Database::Database(const path& dbPath) {
    int result = sqlite3_open_v2(dbPath.c_str(), &this->dbConnection,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    
    if (result != SQLITE_OK) {
        sqlite3_close_v2(this->dbConnection);
        throw std::runtime_error("Failed to open database.");
    }

    // Set up the database if it's empty.
    sqlite3_stmt* checkStmt;
    sqlite3_prepare_v2(this->dbConnection, CHECK_STMT.c_str(), CHECK_STMT.length(), &checkStmt, nullptr);
    result = sqlite3_step(checkStmt);

    sqlite3_finalize(checkStmt);

    if (result == SQLITE_DONE){
        // No rows were returned, so the db can be assumed to be empty.
        sqlite3_exec(this->dbConnection, SQLITE_INIT_STMT, nullptr, nullptr, nullptr);
    }
            
}

Database::~Database() {
    sqlite3_close_v2(this->dbConnection);
}

/**
 * Compiles the statment into an sqlite3_stmt struct and adds it to the
 * execution queue.
 * 
 * @param statement SQL statement to compile and execute as part of a transaction
 */
void Database::enqueueStatement(std::string statement) {
    sqlite3_stmt* preppedStmt;
    int result = sqlite3_prepare_v2(this->dbConnection, statement.c_str(),
        statement.length(), &preppedStmt, nullptr);
    
    if (result != SQLITE_OK) {
        sqlite3_finalize(preppedStmt);
        throw std::runtime_error("Failed to compile SQL query: " + statement);
    }

    this->stmtQueue.push(preppedStmt);
}

bool Database::submitTransaction() {
    this->startTransaction();

    int result = 0;
    sqlite3_stmt* stmt;
    while (!this->stmtQueue.empty()) {
        stmt = this->stmtQueue.front();

        do {
            result = sqlite3_step(stmt);

            if (result == SQLITE_ERROR || result == SQLITE_BUSY || result == SQLITE_MISUSE) {
                    this->rollbackTransaction();
                    std::cerr << "Failed to complete transaction due to database error." << "\n";
                    return false;
            }
            // TODO: Deal with data returned from statements.

        } while (result != SQLITE_DONE);

        sqlite3_finalize(stmt);
        this->stmtQueue.pop();
    }

    try{
        this->commitTransaction();
    } catch (const std::runtime_error& err) {
        this->rollbackTransaction();
        std::cerr << err.what() << "\n";
        return false;
    }

    return true;
}

void Database::startTransaction() {
    int result = sqlite3_exec(this->dbConnection, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

    if (result != SQLITE_OK) {
        throw std::runtime_error("Failed to begin transaction.");
    }
}

void Database::commitTransaction() {
    int result = sqlite3_exec(this->dbConnection, "COMMIT;", nullptr, nullptr, nullptr);

    if (result != SQLITE_OK) {
        throw std::runtime_error("Failed to commit transaction.");
    }
}

void Database::rollbackTransaction() {
    int result = sqlite3_exec(this->dbConnection, "ROLLBACK;", nullptr, nullptr, nullptr);

    if (result != SQLITE_OK) {
        throw std::runtime_error("Failed to rollback transaction.");
    }
}