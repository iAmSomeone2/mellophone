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

// Utility libs
#include <boost/format.hpp>

// Local includes
#include "Track.hpp"

using std::unique_ptr;

using namespace mellophone;

Track::Track(const fs::path &trackLocation)
{
    this->trackLocation = trackLocation;
}

void Track::generateFileHash()
{
    std::stringstream errStream;
    std::ifstream trackStream = std::ifstream(this->trackLocation, std::ios::binary);

    if (!trackStream.is_open()) {
        errStream << boost::format("Unable to open '%s' to generate hash.") % this->trackLocation;
        throw std::runtime_error(errStream.str());
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    uint8_t* buffer = new uint8_t[HASH_BUFF_SIZE];

    uint32_t bytesRead = 0;

    while((bytesRead = trackStream.readsome(reinterpret_cast<char*>(buffer), HASH_BUFF_SIZE)))
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

    for (uint32_t i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(outBuff + (i * 2), "%02x", this->shaDigest[i]);
    }

    outBuff[64] = '\0';

    return string(outBuff);
}

Format Track::determineFormat(const fs::path& trackPath)
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
