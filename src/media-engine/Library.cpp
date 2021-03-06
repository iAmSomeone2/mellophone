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

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "sqlite_init.h"
#include "Library.hpp"

using namespace Mellophone::MediaEngine;

Library::Library()
{
    // Determine the user's Music directory.
    const char *userHome = getenv("HOME");
    if (userHome == nullptr)
    {
        userHome = getpwuid(getuid())->pw_dir;
    }

    if (userHome != nullptr)
    {
        std::string pathStr = userHome;
        pathStr.append("/Music");
        this->userMusicDir = fs::path(pathStr);
    }
    else
    {
        this->userMusicDir = fs::path(".");
    }

    if (!fs::exists(this->userMusicDir))
    {
        fs::create_directories(this->userMusicDir);
    }

    // Determine the user's application data directory.
    const char *dataHome = getenv("XDG_DATA_HOME");
    std::string dataDir;
    if (dataHome == nullptr)
    {
        dataDir = userHome;
        dataDir.append(USER_DATA_DIR);
        this->userDataDir = fs::path(dataDir);
    }
    else
    {
        dataDir = dataHome;
        dataDir.append("/mellophone/");
        this->userDataDir = dataDir;
    }

    if (!fs::exists(this->userDataDir))
    {
        fs::create_directories(this->userDataDir);
    }

    fs::path dbPath = this->userDataDir;
    dbPath /= "media_library.sqlite";

    try
    {
        this->initializeDatabase(dbPath);
    }
    catch (const std::runtime_error &err)
    {
        throw err;
    }
}

Library::~Library()
{
    sqlite3_close_v2(*this->dbConnection);
}

void Library::initializeDatabase(const fs::path &location)
{
    int result = sqlite3_open_v2(location.c_str(), this->dbConnection.get(),
                                 SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

    if (result != SQLITE_OK)
    {
        sqlite3_close_v2(*this->dbConnection.get());
        throw std::runtime_error("Failed to open database.");
    }

    // Set up the database if it's empty.
    sqlite3_stmt *checkStmt;
    sqlite3_prepare_v2(*this->dbConnection, CHECK_STMT.c_str(), CHECK_STMT.length(), &checkStmt, nullptr);
    result = sqlite3_step(checkStmt);

    sqlite3_finalize(checkStmt);

    if (result == SQLITE_DONE)
    {
        // No rows were returned, so the db can be assumed to be empty.
        sqlite3_exec(*this->dbConnection, SQLITE_INIT_STMT, nullptr, nullptr, nullptr);
    }
}

/**
 * Returns a reference to the user's HOME music folder.
 * 
 * @returns reference to the user's HOME music folder path.
 */
const fs::path Library::getMusicFolderPath()
{
    return this->userMusicDir;
}

/**
 * Scans through the user's HOME music folder to locate songs in supported 
 * formats and adds them to the database.
 */
void Library::scanLibrary()
{
}