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

#include <filesystem>
#include <string>

#include <sqlite3.h>

namespace fs = std::filesystem;

namespace mellophone {
    static const std::string CHECK_STMT = "SELECT name FROM sqlite_master WHERE type=\'table\' AND name=\'Tracks\';";

    static const std::string USER_DATA_DIR = "/.local/share/mellophone/";

    class Library {
    private:
        std::shared_ptr<sqlite3*> dbConnection = std::make_shared<sqlite3*>();
        fs::path userMusicDir;
        fs::path userDataDir;


        /**
         * Confirms the existence of the database and connects to it or
         * creates a new database and connects to it.
         * 
         * @param location where to locate database in filesystem
         */
        void initializeDatabase(const fs::path& location);
    public:
        Library();

        ~Library();

        /**
         * Returns a reference to the user's HOME music folder.
         * 
         * @returns reference to the user's HOME music folder path.
         */
        const fs::path getMusicFolderPath();

        /**
         * Scans through the user's HOME music folder to locate songs in supported 
         * formats and adds them to the database.
         */
        void scanLibrary();
    };
}