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

#include <FLAC++/metadata.h>

#include "FLACTrack.hpp"

using namespace mellophone;

FLACTrack::FLACTrack(const fs::path &trackLocation) : Track(trackLocation)
{
    this->format = Format::flac;
}

void FLACTrack::importMetadata()
{
    FLAC::Metadata::VorbisComment* trackComment = nullptr;

    if (!FLAC::Metadata::get_tags(this->trackLocation.c_str(), trackComment))
    {
        throw std::runtime_error("Failed to located metadata in FLAC file.");
    }

    uint32_t numComments = trackComment->get_num_comments();
    map<string, string> commentEntries;

    uint8_t artistCount = 0;
    for (uint32_t i = 0; i < numComments; i++)
    {
        string name = trackComment->get_comment(i).get_field_name();
        string value = trackComment->get_comment(i).get_field_value();

        if (name == "ARTIST")
        {
            artistCount++;
            name.append(std::to_string(artistCount));
        }

        commentEntries[name] = value;
    }

    // FLAC uses the standard Vorbis comment system
    this->parseVorbisCommentMap(commentEntries);
}
