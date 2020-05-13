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
#include <memory>
#include <vector>
#include <array>
#include <map>

// Utility libs
#include <openssl/sha.h>
#include <sqlite3.h>

using std::string;
using std::vector;
using std::map;
using std::array;
using std::shared_ptr;
using std::unique_ptr;

namespace fs = std::filesystem;

namespace Mellophone
{
namespace MediaEngine
{
static const uint8_t SHA256_STR_LEN = 65;

static const uint32_t KILOBYTE = 1024;
static const uint32_t MEGABYTE = 1048576;

static const uint32_t HASH_BUFF_SIZE = MEGABYTE;
enum Format
{
    flac,
    vorbis,
    unknown
};

class Track
{
private:
    const string ARTIST_SELECT_SQL = "SELECT ID FROM Artists WHERE Name == \"@name\";";
    const string ALBUM_SELECT_SQL = "SELECT ID FROM Albums WHERE Name == \"@name\";";

    const string INSERT_TRACK_SQL = "INSERT INTO Tracks VALUES(\"@chksum\",\"@loc\",\"@title\",@album,@trackNum,@totalTracks,@discNum,@totalDiscs);";

protected:
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
    string description = "";
    string genre = "";
    string date = "";

    static string urlEncode(const string &value);

    /**
     * Reads through a map of tags and values to retrieve the relevant metadata.
     * 
     * @param map containing Vorbis comment trags and values.
     */
    void parseVorbisCommentMap(const map<string, string> &comments);

    /**
     * Attempts to locate the album ID in the database. In the event that the album
     * does not exist in the database, a new entry is created and the ID of that is returned.
     * 
     * @param name name of album to search for or create
     * @param db shared pointer to the database connection
     * 
     * @returns integer ID of the album.
     */
    uint32_t getAlbumID(const string& name, const shared_ptr<sqlite3 *> db);

    /**
     * Attempts to locate the artist ID in the database. In the event that the artist
     * does not exist in the database, a new entry is created and the ID of that is returned.
     * 
     * @param name name of artsit to search for or create
     * @param db shared pointer to the database connection
     * 
     * @returns integer ID of the artist.
     */
    uint32_t getArtistID(const string& name, const shared_ptr<sqlite3 *> db);

public:
    explicit Track(const fs::path &trackLocation);

    /**
     * Determines the track's format using the extension as a hint.
     * 
     * FLAC and Vorbis are currently supported.
     */
    static Format determineFormat(const fs::path &trackPath);

    /**
     * Thread-safe method for generating the SHA256 hash of the track data.
     */
    void generateFileHash();

    /**
     * Adds the track and supporting data to the database.
     * 
     * Tracks in the database rely on mapping to an album and an artist, so if the
     * corresponding data is missing it will be created.
     * 
     * @param db database connection to use.
     */
    void addToDatabase(const shared_ptr<sqlite3 *> db);

    /**
     * Retrieves the track's associated format.
     */
    Format getFormat();

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
}; // namespace MediaEngine
}; // namespace Mellophone