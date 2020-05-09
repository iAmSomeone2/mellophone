#include <filesystem>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <Database.hpp>

using std::string;
using std::vector;

namespace fs = std::filesystem;

using namespace mellophone;

class DatabaseTest : public ::testing::Test
{
protected:
  const path dbPath = path("./testDb.sqlite");
  vector<string> testTransaction;

  DatabaseTest()
  {
    this->testTransaction.push_back(string("INSERT INTO Artists(Name) VALUES(\"Test Artist\");"));
    this->testTransaction.push_back(string("INSERT INTO Albums(Name, Artist) VALUES(\"Pop Tarts\", 1);"));
    this->testTransaction.push_back(string("INSERT INTO Tracks(Checksum, FileLocation, Title, Album) VALUES(\"NotChecksum\", \"./01-Brown_Sugar_Cinnamon.flac\", \"Brown Sugar Cinnamon\", 1);"));
  }
};

/**
 * Confirms that this class is capable of creating a new DB if the
 * target doesn't exist.
 */
TEST_F(DatabaseTest, CreateNewDB)
{
  if (fs::exists(this->dbPath))
  {
    fs::remove(this->dbPath);
  }

  Database db = Database(this->dbPath);
}

TEST_F(DatabaseTest, SubmitDataTransaction)
{
  Database db = Database(this->dbPath);

  for (const auto &command : this->testTransaction)
  {
    db.enqueueStatement(command);
  }

  ASSERT_TRUE(db.submitTransaction());
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}