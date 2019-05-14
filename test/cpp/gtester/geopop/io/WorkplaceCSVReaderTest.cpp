//
// Created by lars on 28-3-19.
//

#include "geopop/io/WorkplaceCSVReader.h"
#include "geopop/GeoGridConfig.h"

#include <gtest/gtest.h>

using namespace std;
using namespace geopop;
using namespace stride;

TEST(WorkplaceCSVReader, Test1){
    string csvString =
    R"(ratio,size_min,size_max
    0.778532842256952,1,9
    0.171901116625764,10,49
    0.0410039025210945,50,199
    0.00856213859618965,200,400)";

    GeoGridConfig      geoConfig{};
    auto               instream = make_unique<istringstream>(csvString);
    WorkplaceCSVReader reader(move(instream));

    std::vector<std::vector<double>> reference_workplaces;

    reader.SetReferenceWorkplaces(reference_workplaces);

    EXPECT_EQ(reference_workplaces.size(), 4U);
    EXPECT_EQ(reference_workplaces[0].size(), 3U);
    EXPECT_EQ(reference_workplaces[1].size(), 3U);
    EXPECT_EQ(reference_workplaces[2].size(), 3U);
    EXPECT_EQ(reference_workplaces[3].size(), 3U);

    EXPECT_EQ(reference_workplaces[0][0], 0.778532842256952);
    EXPECT_EQ(reference_workplaces[0][1], 1);
    EXPECT_EQ(reference_workplaces[0][2], 9);

    EXPECT_EQ(reference_workplaces[1][0], 0.171901116625764);
    EXPECT_EQ(reference_workplaces[1][1], 10);
    EXPECT_EQ(reference_workplaces[1][2], 49);

    EXPECT_EQ(reference_workplaces[2][0], 0.0410039025210945);
    EXPECT_EQ(reference_workplaces[2][1], 50);
    EXPECT_EQ(reference_workplaces[2][2], 199);

    EXPECT_EQ(reference_workplaces[3][0], 0.00856213859618965);
    EXPECT_EQ(reference_workplaces[3][1], 200);
    EXPECT_EQ(reference_workplaces[3][2], 400);
}
