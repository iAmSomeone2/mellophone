#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <filesystem>
#include <gtest/gtest.h>

#include <Database.hpp>

namespace fs = std::filesystem;

using namespace mellophone;

class DatabaseTest : public ::testing::Test {

};

/**
 * Confirms that this class is capable of creating a new DB if the
 * target doesn't exist.
 */
TEST_F(DatabaseTest, CreateNewDB) {
    const path dbPath = path("./newDB.sqlite");

    if(fs::exists(dbPath)) {
      fs::remove(dbPath);
    }

    Database db = Database(dbPath);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}