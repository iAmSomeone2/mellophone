#include <filesystem>
#include <gtest/gtest.h>

#include <Track.hpp>

using namespace mellophone;

class TrackTest : public ::testing::Test
{
protected:
  const fs::path flacFile = fs::path("/home/bdavidson/Music/Black Keys, The/Attack & Release (HQ)/02 I Got Mine.flac");
  const fs::path oggFile = fs::path("/home/bdavidson/Music/Presidents of the United States, The/The Presidents of the United States/03 - Lump.ogg");
};

TEST_F(TrackTest, CheckTrackFormats)
{

  Track flacTrack = Track(this->flacFile);
  Track oggTrack = Track(this->oggFile);

  ASSERT_EQ(Format::flac, flacTrack.getFormat());
  ASSERT_EQ(Format::vorbis, oggTrack.getFormat());
}

TEST_F(TrackTest, CheckFLACMetadata)
{
  Track flacTrack = Track(this->flacFile);

  flacTrack.importFLACMetadata();

  ASSERT_EQ("The Black Keys", flacTrack.getArtist());
  ASSERT_EQ("Attack & Release", flacTrack.getAlbum());
  ASSERT_EQ(2, flacTrack.getTrackNum());
  ASSERT_EQ(11, flacTrack.getTotalTracks());
  ASSERT_EQ(1, flacTrack.getDiscNum());
  ASSERT_EQ(1, flacTrack.getTotalDiscs());
}

TEST_F(TrackTest, CheckFLACChecksum) 
{
  string expectedHash = "fa16f387aa12c4232adda38e474d62901c6f951c8cb53125073359563391f3ad";

  Track flacTrack = Track(this->flacFile);

  try
  {
    flacTrack.generateFileHash();
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    FAIL();
  }

  string actualHash = flacTrack.getHashAsString();

  ASSERT_EQ(actualHash, expectedHash);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}