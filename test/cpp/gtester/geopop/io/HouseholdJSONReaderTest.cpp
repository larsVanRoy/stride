/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#include "geopop/io/HouseholdJSONReader.h"
#include "geopop/GeoGridConfig.h"
#include "util/Exception.h"


#include <gtest/gtest.h>
#include <memory>
#include <vector>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

namespace {

TEST(HouseholdJSONReader, validJSON)
{
    string jsonString = R"({
	                            "householdsName": "writeTest",
                                "householdsList": [
		                            [42, 38, 15],
		                            [70, 68],
		                            [40, 39, 9, 6],
		                            [43, 42],
		                            [55, 54],
		                            [40, 40, 3, 3],
		                            [35, 32, 6, 3],
		                            [78, 75]
	                            ]
                            }
                        )";

    GeoGridConfig      geoConfig{};
    auto               instream = make_unique<istringstream>(jsonString);
    HouseholdJSONReader reader(move(instream));

    reader.SetReferenceHouseholds(geoConfig.refHHs[0].person_count, geoConfig.refHHs[0].ages);

    EXPECT_EQ(geoConfig.refHHs[0].person_count, 23U);

    const vector<vector<unsigned int>>& HHages = geoConfig.refHHs[0].ages;

    EXPECT_EQ(HHages.size(), 8U);
    EXPECT_EQ(HHages[0].size(), 3U);
    EXPECT_EQ(HHages[1].size(), 2U);
    EXPECT_EQ(HHages[2].size(), 4U);
    EXPECT_EQ(HHages[3].size(), 2U);
    EXPECT_EQ(HHages[4].size(), 2U);
    EXPECT_EQ(HHages[5].size(), 4U);
    EXPECT_EQ(HHages[6].size(), 4U);
    EXPECT_EQ(HHages[7].size(), 2U);

    EXPECT_EQ(HHages[0][0], 42U);
    EXPECT_EQ(HHages[0][1], 38U);
    EXPECT_EQ(HHages[0][2], 15U);

    EXPECT_EQ(HHages[1][0], 70U);
    EXPECT_EQ(HHages[1][1], 68U);

    EXPECT_EQ(HHages[6][0], 35U);
    EXPECT_EQ(HHages[6][1], 32U);
    EXPECT_EQ(HHages[6][2], 6U);
    EXPECT_EQ(HHages[6][3], 3U);

    EXPECT_EQ(HHages[7][0], 78U);
    EXPECT_EQ(HHages[7][1], 75U);
}

TEST(HouseholdJSONReader, invalidJSON)
{
    string jsonString = R"({
                            invalid
                            }
                        )";

    GeoGridConfig      geoConfig{};
    auto               instream = make_unique<istringstream>(jsonString);
    HouseholdJSONReader reader(move(instream));

    EXPECT_THROW(reader.SetReferenceHouseholds(geoConfig.refHHs[0].person_count, geoConfig.refHHs[0].ages), Exception);
}

} // namespace
