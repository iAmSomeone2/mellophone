#include "CD.hpp"

#include <iostream>

extern "C"
{
    #include <cdio/cdtext.h>
}

using namespace Mellophone::CDImport;

// =========================
// Constructors & Destructor
// =========================

CD::CD()
{
    this->cdio = nullptr;
}

CD::~CD()
{
    if (this->cdio != nullptr)
    {
        cdio_destroy(this->cdio);
    }
}

// =======
// Getters
// =======

const uint_fast8_t& CD::getFirstTrackNum()
{
    return this->firstTrackNum;
}

const uint_fast8_t& CD::getNumTracks()
{
    return this->numTracks;
}

const uint_fast8_t& CD::getLeadoutTrack()
{
    return this->leadoutTrack;
}

const vector<int_fast32_t>& CD::getSectorNums()
{
    return this->sectorNums;
}

// ==========
// Operations
// ==========

void CD::openDisc()
{
    this->cdio = cdio_open("/dev/cdrom", DRIVER_DEVICE);
    if (this->cdio == nullptr)
    {
        throw cd_read_error();
    }

    this->firstTrackNum = static_cast<int_fast32_t>(cdio_get_first_track_num(this->cdio));
    this->numTracks = static_cast<int_fast32_t>(cdio_get_num_tracks(this->cdio));

    this->sectorNums.reserve(this->numTracks);
    for (int i = this->firstTrackNum; i < this->numTracks; i++)
    {
        int_fast32_t lsn = static_cast<int_fast32_t>(cdio_get_track_lsn(this->cdio, i));
        if (lsn != CDIO_INVALID_LSN)
        {
            this->sectorNums.push_back(lsn);
        }
    }

    this->leadoutTrack = static_cast<int_fast32_t>(cdio_get_track_lsn(this->cdio, CDIO_CDROM_LEADOUT_TRACK));
}

const map<string,string> CD::readTrackInfo(uint_fast8_t index)
{
    map<string,string> textFields;

    cdtext_t* cdText = cdtext_init();
    cdText = cdio_get_cdtext(this->cdio);

    if (cdText == nullptr)
    {
        throw cd_text_error();
    }

    uint_fast16_t i;
    for (i = 0; i < MAX_CDTEXT_FIELDS; i++)
    {
        
    }

    cdtext_destroy(cdText);
    return textFields;
}
