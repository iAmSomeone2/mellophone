#include <filesystem>
#include <gtest/gtest.h>

#include <Library.hpp>
#include <Database.hpp>

using namespace mellophone;

class LibraryTest : public ::testing::Test {
protected:
  Database* testDb = nullptr;

  LibraryTest() {
    this->testDb = new Database(fs::path("./testDb.sqlite"));
  }

  ~LibraryTest() override {
    delete this->testDb;
  }
};

TEST_F(LibraryTest, DetectUserMusicFolder) {
    const fs::path musicHome = fs::path("/home/bdavidson/Music");
    Library lib = Library(this->testDb);

    ASSERT_EQ(lib.getMusicFolderPath(), musicHome);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}