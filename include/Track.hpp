#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <map>

using std::string, std::vector, std::map;

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
        uint8_t trackNum        = 1;
        uint8_t totalTracks     = 1;
        uint8_t discNum         = 1;
        uint8_t totalDiscs      = 1;
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

        /**
         * Attempts to fill the Track's metadata entries using the data
         * from the track's file.
         */
        void importVorbisMetadata();

        /**
         * Attempts to fill the Track's metadata entries using the data
         * from the track's file.
         */
        void importFLACMetadata();

        /**
         * Retrieves the title of the track.
         */
        string getTitle();

        /**
         * Retreives the album the track is associated with.
         */
        string getAlbum();

        /**
         * Retrieves the entire list of credited artists.
         */
        vector<string> getArtists();

        /**
         * Retrieves the first artist in the credited artists vector.
         */
        string getArtist();

        /**
         * Returns the date (as a string) the track was released.
         */
        string getDate();

        uint8_t getTrackNum();

        uint8_t getTotalTracks();

        uint8_t getDiscNum();

        uint8_t getTotalDiscs();
    };
};