#include <filesystem>
#include <gtest/gtest.h>

#include <VorbisTrack.hpp>

using namespace mellophone;

class VorbisTrackTest : public ::testing::Test
{
protected:
  const fs::path oggFile = fs::path("/home/bdavidson/Music/Presidents of the United States, The/The Presidents of the United States/03 - Lump.ogg");
  const string expectedHash = "400e6d56a3d408098d23bc61b89d3e6c7726c42afd509cf4b3d5b509c6045572"; 
};

TEST_F(VorbisTrackTest, CheckTrackFormat)
{

  VorbisTrack oggTrack = VorbisTrack(this->oggFile);

  ASSERT_EQ(Format::vorbis, oggTrack.getFormat());
}

// TEST_F(VorbisTrackTest, CheckMetadata)
// {
//   VorbisTrack oggTrack = VorbisTrack(this->oggFile);

//   oggTrack.importMetadata();

//   ASSERT_EQ("The Presidents of the United States", oggTrack.getArtist());
//   ASSERT_EQ("The Presidents of the United States", oggTrack.getAlbum());
//   ASSERT_EQ(03, oggTrack.getTrackNum());
//   ASSERT_EQ(1, oggTrack.getDiscNum());
//   ASSERT_EQ(1, oggTrack.getTotalDiscs());
// }

TEST_F(VorbisTrackTest, CheckChecksum) 
{
  VorbisTrack oggTrack = VorbisTrack(this->oggFile);

  try
  {
    oggTrack.generateFileHash();
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    FAIL();
  }

  string actualHash = oggTrack.getHashAsString();

  ASSERT_EQ(actualHash, this->expectedHash);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
