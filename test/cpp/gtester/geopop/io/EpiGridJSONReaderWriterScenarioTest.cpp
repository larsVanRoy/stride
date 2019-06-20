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

#include "geopop/io/GeoGridJSONWriter.h"
#include "geopop/io/GeoGridJSONReader.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/io/EpiJSONWriter.h"
#include "geopop/io/EpiJSONReader.h"
#include "contact/ContactType.h"
#include "contact/ContactPool.h"

#include "pop/Population.h"

#include "util/FileSys.h"
#include "GeoGridIOUtils.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;
using boost::property_tree::ptree;

namespace {

bool compareJSONs(string given, string expected)
{
    given.erase(remove_if(given.begin(), given.end(), ::isspace), given.end());
    expected.erase(remove_if(expected.begin(), expected.end(), ::isspace), expected.end());

    return given == expected;
}

shared_ptr<EpiGrid> getEpiGridFromFile(const string& filename)
{
    auto file = make_unique<ifstream>();
    file->open(FileSys::GetTestsDir().string() + "/testdata/EpiOutputJSON/" + filename);
    EpiJSONReader reader(move(file));
    return reader.Read();
}

TEST(GeoGridJSONReaderWriterScenarioTest, mainTest)
{
    string expectedJSON = R"(
{
    "history": [
        {
            "locations": [
                {
                    "agebrackets": {
                        "College": {
                            "Exposed": 0.0,
                            "Immune": 0.0,
                            "Infectious": 0.0,
                            "InfectiousAndSymptomatic": 0.0,
                            "Recovered": 0.0,
                            "Susceptible": 0.0,
                            "Symptomatic": 0.0
                        },
                        "Daycare": {
                            "Exposed": 0.0,
                            "Immune": 0.0,
                            "Infectious": 0.0,
                            "InfectiousAndSymptomatic": 0.0,
                            "Recovered": 0.0,
                            "Susceptible": 0.0,
                            "Symptomatic": 0.0
                        },
                        "K12School": {
                            "Exposed": 0.0,
                            "Immune": 0.0,
                            "Infectious": 0.0,
                            "InfectiousAndSymptomatic": 0.0,
                            "Recovered": 0.0,
                            "Susceptible": 0.0,
                            "Symptomatic": 0.0
                        },
                        "PreSchool": {
                            "Exposed": 0.0,
                            "Immune": 0.0,
                            "Infectious": 0.0,
                            "InfectiousAndSymptomatic": 0.0,
                            "Recovered": 0.0,
                            "Susceptible": 0.0,
                            "Symptomatic": 0.0
                        },
                        "Retired": {
                            "Exposed": 0.0,
                            "Immune": 0.0,
                            "Infectious": 0.0,
                            "InfectiousAndSymptomatic": 0.0,
                            "Recovered": 0.0,
                            "Susceptible": 0.0,
                            "Symptomatic": 0.0
                        },
                        "Workplace": {
                            "Exposed": 0.0,
                            "Immune": 0.0,
                            "Infectious": 0.0,
                            "InfectiousAndSymptomatic": 0.0,
                            "Recovered": 0.0,
                            "Susceptible": 0.0,
                            "Symptomatic": 0.0
                        }
                    },
                    "id": 1
                }
            ],
            "timestep": 0
        },
        {
            "locations": [
                {
                    "agebrackets": {
                        "College": {
                            "Exposed": 0.0,
                            "Immune": 0.0,
                            "Infectious": 0.0,
                            "InfectiousAndSymptomatic": 0.0,
                            "Recovered": 0.0,
                            "Susceptible": 0.0,
                            "Symptomatic": 0.0
                        },
                        "Daycare": {
                            "Exposed": 0.0,
                            "Immune": 0.0,
                            "Infectious": 0.0,
                            "InfectiousAndSymptomatic": 0.0,
                            "Recovered": 0.0,
                            "Susceptible": 0.0,
                            "Symptomatic": 0.0
                        },
                        "K12School": {
                            "Exposed": 0.0,
                            "Immune": 0.0,
                            "Infectious": 0.0,
                            "InfectiousAndSymptomatic": 0.0,
                            "Recovered": 0.0,
                            "Susceptible": 0.0,
                            "Symptomatic": 0.0
                        },
                        "PreSchool": {
                            "Exposed": 0.0,
                            "Immune": 0.0,
                            "Infectious": 0.0,
                            "InfectiousAndSymptomatic": 0.0,
                            "Recovered": 0.0,
                            "Susceptible": 0.0,
                            "Symptomatic": 0.0
                        },
                        "Retired": {
                            "Exposed": 0.0,
                            "Immune": 0.0,
                            "Infectious": 0.0,
                            "InfectiousAndSymptomatic": 0.0,
                            "Recovered": 0.0,
                            "Susceptible": 0.0,
                            "Symptomatic": 0.0
                        },
                        "Workplace": {
                            "Exposed": 0.0,
                            "Immune": 0.0,
                            "Infectious": 0.0,
                            "InfectiousAndSymptomatic": 0.0,
                            "Recovered": 0.0,
                            "Susceptible": 0.0,
                            "Symptomatic": 0.0
                        }
                    },
                    "id": 1
                }
            ],
            "timestep": 1
        }
    ],
    "locations": [
        {
            "coordinates": {
                "lat": 8.0,
                "long": 10.0
            },
            "id": 1,
            "name": "Bavikhove",
            "population": 2500
        }
    ]
}
            )";

    auto pop = Population::Create();
    auto& grid = pop->RefGeoGrid();

    grid.AddLocation(make_shared<Location<Coordinate>>(1, 4, Coordinate(10.0, 8.0), "Bavikhove", 2500));

    geopop::EpiJSONWriter writer(FileSys::GetTestsDir().string() + "/testdata/EpiOutputJSON/OWN_readerwriter.json");
    stringstream ss;
    writer.Initialize(grid);
    writer.Write(grid,0);

    writer.Write(grid,1);
    writer.Finalize(ss);
    writer.Finalize();
    EXPECT_TRUE(compareJSONs(ss.str(), expectedJSON));


    std::shared_ptr<EpiGrid> grid2 = getEpiGridFromFile("OWN_readerwriter.json");

    const auto& location1 = grid2->GetById(1);

    EXPECT_EQ(location1->GetID(), 1);
    EXPECT_EQ(location1->GetName(), "Bavikhove");
    EXPECT_EQ(location1->GetPopCount(), 2500);
    EXPECT_EQ(get<0>(location1->GetCoordinate()), 10.0);
    EXPECT_EQ(get<1>(location1->GetCoordinate()), 8.0);
    EXPECT_EQ(location1->maxDays(), 2);

    auto pool = location1->GetPoolStatus(0);

    for(const auto& b : stride::AgeBrackets::AgeBracketList){
        for(const auto& h : stride::HealthStatusList){
            EXPECT_EQ(pool->getPercentage(b,{h}), 0);
        }
    }

    pool = location1->GetPoolStatus(1);

    for(const auto& b : stride::AgeBrackets::AgeBracketList){
        for(const auto& h : stride::HealthStatusList){
            EXPECT_EQ(pool->getPercentage(b,{h}), 0);
        }
    }
}


} // namespace
