#include <filesystem>
#include <gtest/gtest.h>

#include <Database.hpp>

using namespace mellophone;

class DatabaseTest : public ::testing::Test {

};

TEST_F(DatabaseTest, OpenTestDBConnection) {
    const path dbPath = path("./testDB.sqlite");
    Database db = Database(dbPath);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}