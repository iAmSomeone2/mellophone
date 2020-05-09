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

#pragma once

#include <string>
#include <filesystem>
#include <queue>

#include <sqlite3.h>

using std::queue;
using std::filesystem::path;

namespace mellophone
{

static const std::string CHECK_STMT = "SELECT name FROM sqlite_master WHERE type=\'table\' AND name=\'Tracks\';";

class Database
{
private:
    queue<sqlite3_stmt *> stmtQueue = queue<sqlite3_stmt *>();
    sqlite3 *dbConnection = nullptr;

    void startTransaction();
    void commitTransaction();
    void rollbackTransaction();

public:
    explicit Database(const path &dbPath);

    ~Database();

    /**
     * Compiles the statment into an sqlite3_stmt struct and adds it to the
     * execution queue.
     * 
     * @param statement SQL statement to compile and execute as part of a transaction
     */
    void enqueueStatement(std::string statemtent);

    /**
     * Executes the enqueued statements as a transaction.
     */
    bool submitTransaction();
};
} // namespace mellophone