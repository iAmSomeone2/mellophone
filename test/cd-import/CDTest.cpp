#include <fstream>
#include <sstream>
#include <string>
#include <cinttypes>
#include <vector>

#include <gtest/gtest.h>
#include <CD.hpp>

using namespace Mellophone::CDImport;

class CDTest : public ::testing::Test {
protected:
    int_fast32_t EXPECTED_SECTORS[10] = {
        0, 9594, 22876, 38022, 50076, 68246,
        80114, 100382, 114491, 137995
    }; 
};

TEST_F(CDTest, OpenDisc) {
    CD cd = CD();
    cd.openDisc();

    ASSERT_EQ(1, cd.getFirstTrackNum());
    ASSERT_EQ(11, cd.getNumTracks());
    ASSERT_EQ(133, cd.getLeadoutTrack());

    auto const& sectors = cd.getSectorNums();
    for (int i = 0; i < sectors.size(); i++)
    {
        ASSERT_EQ(EXPECTED_SECTORS[i], sectors[i]);
    }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
