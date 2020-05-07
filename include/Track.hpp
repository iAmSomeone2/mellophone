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

// Standard libs
#include <filesystem>
#include <string>
#include <vector>
#include <array>
#include <map>

// Utility libs
#include <openssl/sha.h>

using std::string, std::vector, std::map, std::array;

namespace fs = std::filesystem;

namespace mellophone
{
    static const uint32_t HASH_BUFF_SIZE = 2097152; // 2MB
enum Format
{
    flac,
    vorbis
};

class Track
{
private:
    // Internal data
    Format format;
    fs::path trackLocation;
    array<uint8_t, SHA256_DIGEST_LENGTH> shaDigest;

    // Track metadata
    string title = "unknown";
    string version = "";
    string album = "unknown";
    uint8_t trackNum = 1;
    uint8_t totalTracks = 1;
    uint8_t discNum = 1;
    uint8_t totalDiscs = 1;
    vector<string> artist;
    string performer = "unknown";
    string copyright = "";
    string licence = "";
    string organization = "";
    string description = "";
    string genre = "";
    string date = "";
    string location = "";
    string contact = "";
    string isrcCode = "";

    /**
     * Determines the track's format using the extension as a hint.
     * 
     * FLAC and Vorbis are currently supported.
     */
    bool determineFormat();
    
    /**
     * Reads through a map of tags and values to retrieve the relevant metadata.
     * 
     * @param map containing Vorbis comment trags and values.
     */
    void parseVorbisCommentMap(map<string, string> comments);
public:
    explicit Track(const fs::path &trackLocation);

    /**
     * Thread-safe method for generating the SHA256 hash of the track data.
     */
    void generateFileHash();

    Format getFormat()
    {
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
     * 
     * @returns date (as a string) the track was released
     */
    string getDate();

    /**
     * Returns the track number
     * 
     * @returns track number
     */
    uint8_t getTrackNum();

    /**
     * Returns the total number of tracks in the album or compilation.
     * 
     * @returns total number of tracks in the album or compilation
     */
    uint8_t getTotalTracks();

    /**
     * Returns the associated disc number from the set of discs.
     * 
     * @returns associated disc number from the set of discs
     */
    uint8_t getDiscNum();

    /**
     * Returns the total number of discs in the set.
     * 
     * @returns total number of discs in the set
     */
    uint8_t getTotalDiscs();

    /**
     * Converts the raw SHA256 hash to a string and returns it.
     * 
     * @returns string version of raw SHA256 string
     */
    string getHashAsString();
};
}; // namespace mellophone