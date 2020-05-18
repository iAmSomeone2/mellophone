#include <filesystem>
#include <gtest/gtest.h>

#include <FLACTrack.hpp>

using namespace Mellophone::MediaEngine;

class FLACTrackTest : public ::testing::Test
{
protected:
  const fs::path flacFile = fs::path("/home/bdavidson/Music/Black Keys, The/Attack & Release (HQ)/02 I Got Mine.flac");
  const string expectedHash = "fa16f387aa12c4232adda38e474d62901c6f951c8cb53125073359563391f3ad";

  const fs::path badFile = fs::path("/dev/null");
};

TEST_F(FLACTrackTest, CheckTrackFormat)
{

  FLACTrack track = FLACTrack(this->flacFile);

  ASSERT_EQ(Format::flac, track.getFormat());
}

TEST_F(FLACTrackTest, ReadBadFile)
{
  FLACTrack badTrack = FLACTrack(this->badFile);
  EXPECT_ANY_THROW(badTrack.importMetadata());
}

TEST_F(FLACTrackTest, CheckMetadata)
{
  FLACTrack track = FLACTrack(this->flacFile);

  track.importMetadata();

  ASSERT_EQ("The Black Keys", track.getArtist());
  ASSERT_EQ("Attack & Release", track.getAlbum());
  ASSERT_EQ(2, track.getTrackNum());
  ASSERT_EQ(11, track.getTotalTracks());
  ASSERT_EQ(1, track.getDiscNum());
  ASSERT_EQ(1, track.getTotalDiscs());
}

TEST_F(FLACTrackTest, CheckChecksum) 
{
  FLACTrack track = FLACTrack(this->flacFile);

  try
  {
    track.generateFileHash();
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    FAIL();
  }

  string actualHash = track.getHashAsString();

  ASSERT_EQ(actualHash, this->expectedHash);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}