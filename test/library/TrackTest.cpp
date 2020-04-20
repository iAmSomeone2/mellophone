#include <filesystem>
#include <gtest/gtest.h>

#include <Library.hpp>

using namespace mellophone;

class TrackTest : public ::testing::Test {
protected:
    const fs::path musicHome = fs::path("/home/bdavidson/Music");
};

TEST_F(TrackTest, CheckTrackFormats) {
    const fs::path flacFile = fs::path("/home/bdavidson/Music/Black Keys, The/Attack & Release (HQ)/02 I Got Mine.flac");
    const fs::path oggFile = fs::path ("/home/bdavidson/Music/Presidents of the United States, The/The Presidents of the United States/03 - Lump.ogg");

    Track flacTrack = Track(flacFile);
    Track oggTrack = Track(oggFile);

    ASSERT_EQ(Format::flac, flacTrack.getFormat());
    ASSERT_EQ(Format::vorbis, oggTrack.getFormat());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}