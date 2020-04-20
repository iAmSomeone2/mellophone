#pragma once

#include <filesystem>
#include <string>

#include <Database.hpp>

namespace fs = std::filesystem;

namespace mellophone {
    class Library {
    private:
        std::shared_ptr<Database> database;
        fs::path userMusicFolder;
    public:
        explicit Library(std::shared_ptr<Database> db);

        /**
         * Returns a reference to the user's HOME music folder.
         * 
         * @returns reference to the user's HOME music folder path.
         */
        fs::path& getMusicFolderPath();

        /**
         * Scans through the user's HOME music folder to locate songs in supported 
         * formats and adds them to the database.
         */
        void scanLibrary();
    };
}