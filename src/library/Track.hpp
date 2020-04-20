#pragma once

#include <filesystem>
#include <string>
#include <vector>

using std::string, std::vector;

namespace fs = std::filesystem;

namespace mellophone {
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

        /**
         * Attempts to fill the Track's metadata entries using the data
         * from the track's file.
         */
        void importMetadata();
    public:
        explicit Track(const fs::path& trackLocation);

        Format getFormat() {
            return this->format;
        }
    };
};