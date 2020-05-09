#include <filesystem>
#include <memory>
#include <gtest/gtest.h>

#include <Library.hpp>

using namespace mellophone;

class LibraryTest : public ::testing::Test {
protected:
};

TEST_F(LibraryTest, DetectUserMusicFolder) {
    const fs::path musicHome = fs::path("/home/bdavidson/Music");
    Library lib = Library();

    ASSERT_EQ(lib.getMusicFolderPath(), musicHome);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}