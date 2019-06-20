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

#include "geopop/io/EpiJSONWriter.h"
//#include "geopop/GeoGridConfig.h"

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
using namespace stride::util;
using boost::property_tree::ptree;

namespace {

bool compareJSONs(string given, string expected)
{
    given.erase(remove_if(given.begin(), given.end(), ::isspace), given.end());
    expected.erase(remove_if(expected.begin(), expected.end(), ::isspace), expected.end());

    return given == expected;
}

TEST(EpiJSONWriterUnitTest, locationsTest)
{
    auto pop     = Population::Create();
    auto geoGrid = GeoGrid(pop.get());
    geoGrid.AddLocation(make_shared<Location<Coordinate>>(1, 4, Coordinate(10.0, 8.0), "Bavikhove", 2500));
    geoGrid.AddLocation(make_shared<Location<Coordinate>>(2, 3, Coordinate(20.0, 55.0), "Gent", 5000));
    geoGrid.AddLocation(make_shared<Location<Coordinate>>(3, 2, Coordinate(30., 40.0), "Mons", 2500));

    string expectedJSON = R"(
{
    "history": [],
    "locations": [
        {
            "coordinates": {
                "lat": 8.0,
                "long": 10.0
            },
            "id": 1,
            "name": "Bavikhove",
            "population": 2500
        },
        {
            "coordinates": {
                "lat": 55.0,
                "long": 20.0
            },
            "id": 2,
            "name": "Gent",
            "population": 5000
        },
        {
            "coordinates": {
                "lat": 40.0,
                "long": 30.0
            },
            "id": 3,
            "name": "Mons",
            "population": 2500
        }
    ]
}

            )";

    EpiJSONWriter writer("");
    stringstream ss;
    writer.Initialize(geoGrid);
    writer.Finalize(ss);
    EXPECT_TRUE(compareJSONs(ss.str(), expectedJSON));
}

TEST(EpiJSONWriterUnitTest, contactPoolsTest)
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
                            "Susceptible": 0.0004,
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
                            "Susceptible": 0.0004,
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
                "lat": 0.0,
                "long": 0.0
            },
            "id": 1,
            "name": "Bavikhove",
            "population": 2500
        }
    ]
}

            )";

    auto pop = Population::Create();
    std::shared_ptr<GeoGrid> geogrid = GetPopulatedGeoGrid(pop.get());

    EpiJSONWriter writer("");
    stringstream ss;
    writer.Initialize(*geogrid.get());
    writer.Write(*geogrid.get(),0);

    writer.Write(*geogrid.get(),1);
    writer.Finalize(ss);
    EXPECT_TRUE(compareJSONs(ss.str(), expectedJSON));
}
} // namespace