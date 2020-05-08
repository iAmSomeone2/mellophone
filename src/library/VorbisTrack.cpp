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

#include <boost/format.hpp>

#include <vorbis/vorbisfile.h>

#include "VorbisTrack.hpp"

using namespace mellophone;

VorbisTrack::VorbisTrack(const fs::path &trackLocation) : Track(trackLocation)
{
    this->format = Format::vorbis;
}

void VorbisTrack::importMetadata()
{
    std::stringstream errStream;
    OggVorbis_File* vorbisFile = nullptr;

    const char* path = this->trackLocation.c_str();

    /*
        Using ov_fopen doesn't seem to work with files that have spaces in them. Because of this (and to support Windows in the future),
        I'll need to set up the data handling callbacks.
    */
    if(int result = ov_fopen(path, vorbisFile) != 0) {
        switch (result) {
            case OV_EREAD:
                errStream << boost::format("Failed to open '%s'") % this->trackLocation.string();
                break;
            case OV_ENOTVORBIS:
                errStream << boost::format("'%s' does not contain any Vorbis data.") % this->trackLocation.string();
                break;
            case OV_EBADHEADER:
                errStream << boost::format("Invalid Vorbis bitstream header in file: '%s'.") % this->trackLocation.string();
                break;
            default:
                errStream << boost::format("Unknown error ocurred when attempting to import metadata from '%s'.") % this->trackLocation.string();
        }

        throw std::runtime_error(errStream.str());
    }

    vorbis_comment* trackComment = ov_comment(vorbisFile, -1);

    if (!trackComment) {
        errStream << boost::format("Missing comment in file: '%s'") % this->trackLocation.string();
        throw std::runtime_error(errStream.str());
    }

    // Read track comments into a map to feed to the parser.
    map<string, string> comments;
    uint32_t numComments = static_cast<uint32_t>(trackComment->comments);

    uint32_t artistCount = 0;
    for (uint32_t i = 0; i < numComments; i++)
    {
        string commentStr = trackComment->user_comments[i];
        uint32_t delimLoc = commentStr.find('=');

        string key = commentStr.substr(0, delimLoc+1);
        string value = commentStr.substr(delimLoc);

        if (key == "ARTIST")
        {
            artistCount++;
            key.append(std::to_string(artistCount));
        }

        comments[key] = value;
    }

    ov_clear(vorbisFile);
    this->parseVorbisCommentMap(comments);
}
