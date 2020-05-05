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

#include <fstream>
#include <memory>
#include <cstring>

#include <FLAC++/metadata.h>

#include "Track.hpp"

using std::unique_ptr;

using namespace mellophone;

Track::Track(const fs::path &trackLocation)
{
    this->trackLocation = trackLocation;

    bool success = this->determineFormat();

    if (!success)
    {
        throw std::runtime_error("Failed to create Track object from file.");
    }
}

bool Track::determineFormat()
{
    const string extension = this->trackLocation.extension().c_str();

    std::ifstream trackFile = std::ifstream(this->trackLocation, std::ios::binary);

    if (!trackFile.is_open())
    {
        throw std::runtime_error("Failed to open track.");
    }

    unique_ptr<char> buffer;

    bool confirmedFormat = false;

    if (extension == ".flac")
    {
        // The first 4 characters in the file will confirm the format.
        buffer.reset(new char[5]);
        buffer.get()[4] = '\0';
        trackFile.readsome(buffer.get(), 4);

        int result = strncmp("fLaC", buffer.get(), 4);

        if (result == 0)
        {
            this->format = Format::flac;
            confirmedFormat = true;
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
            this->format = Format::vorbis;
            confirmedFormat = true;
        }
    }

    trackFile.close();

    return confirmedFormat;
}

void Track::importFLACMetadata()
{
    FLAC::Metadata::VorbisComment *trackComment = nullptr;

    if (!FLAC::Metadata::get_tags(this->trackLocation.c_str(), trackComment))
    {
        throw std::runtime_error("Failed to located metadata in FLAC file.");
    }

    uint32_t numComments = trackComment->get_num_comments();
    vector<FLAC::Metadata::VorbisComment::Entry> commentEntries;
    commentEntries.reserve(numComments);

    for (uint32_t i = 0; i < numComments; i++)
    {
        commentEntries.push_back(trackComment->get_comment(i));
    }

    for (auto const &entry : commentEntries)
    {
        const string fieldName = entry.get_field_name();
        const string fieldValue = entry.get_field_value();

        if (fieldName == "TITLE")
        {
            this->title = fieldValue;
        }
        else if (fieldName == "ALBUM")
        {
            this->album = fieldValue;
        }
        else if (fieldName == "ARTIST")
        {
            this->artist.push_back(fieldValue);
        }
        else if (fieldName == "DATE")
        {
            this->date = fieldValue;
        }
        else if (fieldName == "DISCNUMBER")
        {
            this->discNum = static_cast<uint8_t>(strtoul(fieldValue.c_str(), nullptr, 10));
        }
        else if (fieldName == "TOTALDISCS")
        {
            this->totalDiscs = static_cast<uint8_t>(strtoul(fieldValue.c_str(), nullptr, 10));
        }
        else if (fieldName == "TRACKNUMBER")
        {
            this->trackNum = static_cast<uint8_t>(strtoul(fieldValue.c_str(), nullptr, 10));
        }
        else if (fieldName == "TOTALTRACKS")
        {
            this->totalTracks = static_cast<uint8_t>(strtoul(fieldValue.c_str(), nullptr, 10));
        }
    }
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
