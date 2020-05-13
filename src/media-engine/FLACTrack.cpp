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

#include <memory>

#include <FLAC/format.h>
#include <FLAC/metadata.h>
#include <FLAC++/metadata.h>

#include "FLACTrack.hpp"

using std::shared_ptr;
using std::unique_ptr;

using namespace Mellophone::MediaEngine;

FLACTrack::FLACTrack(const fs::path &trackLocation) : Track(trackLocation)
{
    this->format = Format::flac;
}

void FLACTrack::importMetadata()
{
    FLAC__StreamMetadata *streamMetadata = FLAC__metadata_object_new(FLAC__METADATA_TYPE_VORBIS_COMMENT);

    if (!FLAC__metadata_get_tags(this->trackLocation.c_str(), &streamMetadata))
    {
        throw std::runtime_error("Failed to locate metadata in FLAC file.");
    }

    const auto &vorbisComment = streamMetadata->data.vorbis_comment;

    uint32_t numComments = vorbisComment.num_comments;
    map<string, string> commentEntries;

    uint8_t artistCount = 0;
    for (uint32_t i = 0; i < numComments; i++)
    {
        string entry = reinterpret_cast<char *>(vorbisComment.comments[i].entry);

        uint64_t splitLoc = entry.find('=');

        string name = entry.substr(0, splitLoc);
        string value = entry.substr(splitLoc + 1);

        if (name == "ARTIST")
        {
            artistCount++;
            name.append(std::to_string(artistCount));
        }

        commentEntries[name] = value;
    }

    FLAC__metadata_object_delete(streamMetadata);

    // FLAC uses the standard Vorbis comment system
    this->parseVorbisCommentMap(commentEntries);
}
