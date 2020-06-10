#pragma once

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <cinttypes>

extern "C"
{
    #include <cdio/cdio.h>
}

using std::unique_ptr;
using std::vector;
using std::string;
using std::map;

namespace Mellophone
{
namespace CDImport
{
    /**
     * @brief Base class for CD read exceptions.
     */
    struct cd_read_error : public std::exception
    {
        const char* what() const throw()
        {
            return "Error occurred while attempting to read from CD device.";
        }
    };

    struct cd_text_error : public cd_read_error
    {
        const char* what() const throw()
        {
            return "Error occurred while attempting to read CD-Text.";
        }
    };

    class CD
    {
    private:
        CdIo_t* cdio;

        uint_fast8_t firstTrackNum = 0;
        uint_fast8_t numTracks = 0;
        uint_fast8_t leadoutTrack = 0;
        vector<int_fast32_t> sectorNums;
    public:
        /**
         * Creates a new instance of CD
         */
        CD();

        /**
         * Destroys the instance of CD
         */
        ~CD();

        // ==========
        // Operations
        // ==========

        /**
         * Attmepts to open the CD in the disc drive if one is present.
         * 
         * @throw cd_read_error
         */
        void openDisc();

        /**
         * Reads the CD-Text info for the track at the specified index.
         * 
         * @param index track index to read
         * 
         * @returns string map containing CD-Text data for specified track.
         */
        const map<string,string> readTrackInfo(uint_fast8_t index);

        // =======
        // Getters
        // =======

        /**
         * @returns the index of the first track on the CD.
         */
        const uint_fast8_t& getFirstTrackNum();

        /**
         * @returns the number of tracks on the CD.
         */
        const uint_fast8_t& getNumTracks();

        /**
         * @returns the sector number of the CD's leadout track.
         */
        const uint_fast8_t& getLeadoutTrack();

        /**
         * @returns a vector containing the sector numbers of the CD's tracks.
         */
        const vector<int_fast32_t>& getSectorNums();
    };
} // namespace CDImport
} // namespace Mellophone
