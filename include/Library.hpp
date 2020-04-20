#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <memory>

#include <Database.hpp>

using std::string, std::vector;

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

    enum Format {
        flac,
        vorbis
    };

    class Track {
    private:
        Format format;
        fs::path trackLocation;

        // Track metadata
        string title        = "unknown";
        string version      = "";
        string album        = "unknown";
        int trackNum        = 1;
        int totalTracks     = 1;
        int discNum         = 1;
        int totalDiscs      = 1;
        vector<string> artist;
        string performer    = "unknown";
        string copyright    = "";
        string licence      = "";
        string organization = "";
        string description  = "";
        string genre        = "";
        string date         = "";
        string location     = "";
        string contact      = "";
        string isrcCode     = "";

        /**
         * Determines the track's format using the extension as a hint.
         * 
         * FLAC and Vorbis are currently supported.
         */
        bool determineFormat();
    public:
        explicit Track(const fs::path& trackLocation);

        Format getFormat() {
            return this->format;
        }
    };
}