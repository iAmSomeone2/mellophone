#include <fstream>
#include <memory>
#include <cstring>

#include <FLAC++/metadata.h>

#include "Track.hpp"

using std::unique_ptr;

using namespace mellophone;

Track::Track(const fs::path& trackLocation) {
    this->trackLocation = trackLocation;

    bool success = this->determineFormat();

    if (!success) {
        throw std::runtime_error("Failed to create Track object from file.");
    }


}

bool Track::determineFormat() {
    string extension = this->trackLocation.extension().generic_string();

    std::ifstream trackFile = std::ifstream(this->trackLocation, std::ios::binary);

    if (!trackFile.is_open()) {
        throw std::runtime_error("Failed to open track.");
    }

    unique_ptr<char> buffer;

    bool confirmedFormat = false;

    if (extension == ".flac") {
        // The first 4 characters in the file will confirm the format.
        buffer.reset(new char[5]);
        buffer.get()[4] = '\0';

        trackFile.readsome(buffer.get(), 4);

        int result = strncmp("fLaC", buffer.get(), 4);

        if (result == 0) {
            this->format = Format::flac;
            confirmedFormat = true;
        }

        trackFile.seekg(std::ios::beg);
    } else if (extension == ".ogg" || extension == ".oga") {
        // Format declarations are between the OggS tags in the top header.
        // 'vorbis' is always at 0x2D
        buffer.reset(new char[7]);
        buffer.get()[6] = '\0';

        trackFile.seekg(std::ios::beg + 0x1D);
        trackFile.readsome(buffer.get(), 6);

        int result = strncmp("vorbis", buffer.get(), 6);

        if (result == 0) {
            this->format = Format::vorbis;
            confirmedFormat = true;
        }
    }

    trackFile.close();

    return confirmedFormat;
}

void Track::importMetadata() {
    
}