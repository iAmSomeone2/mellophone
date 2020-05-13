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

// Standard libs
#include <fstream>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cctype>
#include <iomanip>

// Utility libs
#include <boost/format.hpp>
#include <sqlite3.h>

// Local includes
#include "Track.hpp"

using namespace Mellophone::MediaEngine;

string Track::urlEncode(const string &value)
{
    std::ostringstream encodedStr;
    encodedStr.fill('0');
    encodedStr << std::hex;

    for (auto i = value.begin(), n = value.end(); i != n; i++)
    {
        string::value_type c = (*i);

        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            encodedStr << c;
            continue;
        }

        // Encode all other characters
        encodedStr << std::uppercase;
        encodedStr << '%' << std::setw(2) << int((unsigned char)c);
        encodedStr << std::nouppercase;
    }

    return encodedStr.str();
}

Track::Track(const fs::path &trackLocation)
{
    this->trackLocation = trackLocation;
}

void Track::generateFileHash()
{
    std::stringstream errStream;
    std::ifstream trackStream = std::ifstream(this->trackLocation, std::ios::binary);

    if (!trackStream.is_open())
    {
        errStream << boost::format("Unable to open '%s' to generate hash.") % this->trackLocation;
        throw std::runtime_error(errStream.str());
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    uint8_t *buffer = new uint8_t[HASH_BUFF_SIZE];

    uint32_t bytesRead = 0;

    while ((bytesRead = trackStream.readsome(reinterpret_cast<char *>(buffer), HASH_BUFF_SIZE)))
    {
        SHA256_Update(&sha256, buffer, bytesRead);
    }
    SHA256_Final(this->shaDigest.data(), &sha256);

    delete[] buffer;
    trackStream.close();
}

string Track::getHashAsString()
{
    char outBuff[65];

    for (uint32_t i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(outBuff + (i * 2), "%02x", this->shaDigest[i]);
    }

    outBuff[64] = '\0';

    return string(outBuff);
}

Format Track::determineFormat(const fs::path &trackPath)
{
    const string extension = trackPath.extension().c_str();

    std::ifstream trackFile = std::ifstream(trackPath, std::ios::binary);

    if (!trackFile.is_open())
    {
        throw std::runtime_error("Failed to open track.");
    }

    unique_ptr<char> buffer;

    Format trackFormat = Format::unknown;

    if (extension == ".flac")
    {
        // The first 4 characters in the file will confirm the format.
        buffer.reset(new char[5]);
        buffer.get()[4] = '\0';
        trackFile.readsome(buffer.get(), 4);

        int result = strncmp("fLaC", buffer.get(), 4);

        if (result == 0)
        {
            trackFormat = Format::flac;
        }

        trackFile.seekg(std::ios::beg);
    }
    else if (extension == ".ogg" || extension == ".oga")
    {
        // Format declarations are between the OggS tags in the top header.
        // 'vorbis' is always at 0x1D
        buffer.reset(new char[7]);
        buffer.get()[6] = '\0';

        trackFile.seekg(std::ios::beg + 0x1D);
        trackFile.readsome(buffer.get(), 6);

        int result = strncmp("vorbis", buffer.get(), 6);

        if (result == 0)
        {
            trackFormat = Format::vorbis;
        }
    }

    trackFile.close();

    return trackFormat;
}

void Track::parseVorbisCommentMap(const map<string, string> &comments)
{
    this->artist.clear();
    for (const auto &entryPair : comments)
    {
        int16_t findLoc = -1;
        findLoc = entryPair.first.find("ARTIST");

        if (findLoc >= 0)
        {
            // Append this artist to the artists vector
            this->artist.push_back(entryPair.second);
        }
        else
        {
            // Check for any of the other fields.
            if (entryPair.first == "TITLE")
            {
                this->title = entryPair.second;
            }
            else if (entryPair.first == "ALBUM")
            {
                this->album = entryPair.second;
            }
            else if (entryPair.first == "DATE")
            {
                this->date = entryPair.second;
            }
            else if (entryPair.first == "DISCNUMBER")
            {
                this->discNum = static_cast<uint8_t>(strtoul(entryPair.second.c_str(), nullptr, 10));
            }
            else if (entryPair.first == "TOTALDISCS")
            {
                this->totalDiscs = static_cast<uint8_t>(strtoul(entryPair.second.c_str(), nullptr, 10));
            }
            else if (entryPair.first == "TRACKNUMBER")
            {
                this->trackNum = static_cast<uint8_t>(strtoul(entryPair.second.c_str(), nullptr, 10));
            }
            else if (entryPair.first == "TOTALTRACKS")
            {
                this->totalTracks = static_cast<uint8_t>(strtoul(entryPair.second.c_str(), nullptr, 10));
            }
            else if (entryPair.first == "VERSION")
            {
                this->version = entryPair.second;
            }
            else if (entryPair.first == "PERFORMER")
            {
                this->performer = entryPair.second;
            }
            else if (entryPair.first == "GENRE")
            {
                this->genre = entryPair.second;
            }
            else if (entryPair.first == "LICENSE")
            {
                this->licence = entryPair.second;
            }
            else if (entryPair.first == "DESCRIPTION")
            {
                this->description = entryPair.second;
            }
        }
    }

    // Assume that the performer is the first artist in the vector
    if (this->performer == "unknown")
    {
        this->performer = this->getArtist();
    }
}

uint32_t Track::getAlbumID(const string& name, const shared_ptr<sqlite3 *> db)
{

    return 0;
}

uint32_t Track::getArtistID(const string& name, const shared_ptr<sqlite3 *> db)
{
    unique_ptr<sqlite3_stmt*> stmt = std::make_unique<sqlite3_stmt*>();

    sqlite3_prepare_v2(*db, ARTIST_SELECT_SQL.c_str, ARTIST_SELECT_SQL.length(), stmt.get(), nullptr);
    sqlite3_bind_text(*stmt, 1, this->artist[0].c_str(), -1, SQLITE_STATIC);
    int execResult = sqlite3_step(*stmt);

    sqlite3_finalize(*stmt);

    if (execResult == SQLITE_DONE)
    {
        // The artist does not exist, so insert it.
        static const string ARTIST_INSERT_SQL = "INSERT INTO Artists(Name) VALUES(\"@name\");";
        uint32_t stmtLen = ARTIST_INSERT_SQL.length() + this->artist[0].length();
        sqlite3_prepare_v2(*db, ARTIST_INSERT_SQL.c_str(), stmtLen, stmt.get(), nullptr);
        sqlite3_bind_text(*stmt, 1, this->artist[0].c_str(), -1, SQLITE_STATIC);
        sqlite3_step(*stmt);
        sqlite3_finalize(*stmt);
    }

    // Run the search again since a new entry may have been made.
    sqlite3_prepare_v2(*db, ARTIST_SELECT_SQL.c_str, ARTIST_SELECT_SQL.length(), stmt.get(), nullptr);
    sqlite3_bind_text(*stmt, 1, this->artist[0].c_str(), -1, SQLITE_STATIC);
    execResult = sqlite3_step(*stmt);

    if (execResult != SQLITE_ROW)
    {
        throw std::runtime_error("Failed to add artist to database.");
    }

    const uint32_t id = sqlite3_column_int(*stmt, 0);
    sqlite3_finalize(*stmt);

    return id;
}

void Track::addToDatabase(const shared_ptr<sqlite3 *> db)
{
    // Start by determining if the track is already in the DB.
    static const string FIND_CHECKSUM_STMT = "SELECT * FROM Tracks WHERE Checksum == \"?\";";

    unique_ptr<sqlite3_stmt*> stmt = std::make_unique<sqlite3_stmt*>();

    sqlite3_prepare_v2(*db, FIND_CHECKSUM_STMT.c_str, FIND_CHECKSUM_STMT.length() + SHA256_STR_LEN, stmt.get(), nullptr);
    sqlite3_bind_text(*stmt, 1, this->getHashAsString().c_str(), -1, SQLITE_STATIC);
    int execResult = sqlite3_step(*stmt);

    if (execResult != SQLITE_DONE)
    {
        // A file with the same checksum was already in the db.
        sqlite3_finalize(*stmt);
        return;
    }

    // Check if the album exists. Also checks for artist.
}

Format Track::getFormat()
{
    return this->format;
}

/**
 * Retrieves the title of the track.
 */
string Track::getTitle()
{
    return this->title;
}

/**
 * Retreives the album the track is associated with.
 */
string Track::getAlbum()
{
    return this->album;
}

/**
 * Retrieves the entire list of credited artists.
 */
vector<string> Track::getArtists()
{
    return this->artist;
}

/**
 * Retrieves the first artist in the credited artists vector.
 */
string Track::getArtist()
{
    return this->artist[0];
}

/**
 * Returns the date (as a string) the track was released.
 */
string Track::getDate()
{
    return this->date;
}

uint8_t Track::getTrackNum()
{
    return this->trackNum;
};

uint8_t Track::getTotalTracks()
{
    return this->totalTracks;
};

uint8_t Track::getDiscNum()
{
    return this->discNum;
};

uint8_t Track::getTotalDiscs()
{
    return this->totalDiscs;
};
