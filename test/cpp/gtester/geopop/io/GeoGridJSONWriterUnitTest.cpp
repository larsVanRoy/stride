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
#include "geopop/GeoGridConfig.h"

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

/*void sortContactCenters(ptree& pt)
{
        auto& contactCenters       = pt.get_child("contactPools");
        auto  compareContactCenter = [](const pair<string, ptree>& a, const pair<string, ptree>& b) {
                return a.second.get<string>("class") < b.second.get<string>("class");
        };
        contactCenters.sort<decltype(compareContactCenter)>(compareContactCenter);
}*/

/*void sortTree(ptree& tree)
{
        auto compareLocation = [](const pair<string, ptree>& a, const pair<string, ptree>& b) {
                return a.second.get<string>("id") < b.second.get<string>("id");
        };
        auto& locations = tree.get_child("locations");
        locations.sort<decltype(compareLocation)>(compareLocation);

        for (auto it = locations.begin(); it != locations.end(); it++) {
                sortContactCenters(it->second.get_child(""));
        }
}*/

bool compareJSONs(string given, string expected)
{
        given.erase(remove_if(given.begin(), given.end(), ::isspace), given.end());
        expected.erase(remove_if(expected.begin(), expected.end(), ::isspace), expected.end());

        return given == expected;
}

/*bool compareGeoGrid(GeoGrid& geoGrid, const string& testname)
{
        GeoGridJSONWriter writer;
        GeoGridJSONWriter w2;

        stringstream      ss;

        ofstream outputFileStream(FileSys::GetTestsDir().string() + "/testdata/GeoGridJSON/OWN_" + testname);

        writer.Write(geoGrid, ss);
        w2.Write(geoGrid, outputFileStream);
        ptree result;
        read_json(ss, result);
        sortTree(result);

        ptree expected;
        read_json(FileSys::GetTestsDir().string() + "/testdata/GeoGridJSON/" + testname, expected);
        sortTree(expected);

        ostringstream oss1, oss2;
        boost::property_tree::xml_parser::write_xml(oss1, result);
        boost::property_tree::xml_parser::write_xml(oss2, expected);

        return oss1.str() == oss2.str();
}*/

TEST(GeoGridJSONWriterUnitTest, locationTest)
{
        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        geoGrid.AddLocation(make_shared<Location<Coordinate>>(1, 4, Coordinate(0, 0), "Bavikhove", 2500));
        geoGrid.AddLocation(make_shared<Location<Coordinate>>(2, 3, Coordinate(0, 0), "Gent", 5000));
        geoGrid.AddLocation(make_shared<Location<Coordinate>>(3, 2, Coordinate(0, 0), "Mons", 2500));

        string expectedJSON = R"({
            "locations":[
            {
                "contactPools": [],
                "coordinate": {
                    "latitude": 0.0,
                    "longitude": 0.0
                },
                "id": 1,
                "name": "Bavikhove",
                "population": 2500,
                "province": 4
            },
            {
                "contactPools": [],
                "coordinate": {
                    "latitude": 0.0,
                    "longitude": 0.0
                },
                "id": 2,
                "name": "Gent",
                "population": 5000,
                "province": 3
            },
            {
                "contactPools": [],
                "coordinate": {
                    "latitude": 0.0,
                    "longitude": 0.0
                },
                "id": 3,
                "name": "Mons",
                "population": 2500,
                "province": 2
            }
            ],
            "persons": []
            })";

        GeoGridJSONWriter writer;
        stringstream ss;
        writer.Write(geoGrid, ss);

        EXPECT_TRUE(compareJSONs(ss.str(), expectedJSON));
}


TEST(GeoGridJSONWriterUnitTest, contactPoolTest)
{
        auto pop      = Population::Create();
        auto geoGrid  = GeoGrid(pop.get());

        auto location = make_shared<Location<Coordinate>>(1, 4, Coordinate(0, 0), "Bavikhove", 2500);

        auto pool1 = pop->RefPoolSys().CreateContactPool(Id::K12School);
        location->RegisterPool(pool1, Id::K12School);

        auto pool2 = pop->RefPoolSys().CreateContactPool(Id::PrimaryCommunity);
        location->RegisterPool(pool2, Id::PrimaryCommunity);

        auto pool3 = pop->RefPoolSys().CreateContactPool(Id::College);
        location->RegisterPool(pool3, Id::College);

        auto pool4 = pop->RefPoolSys().CreateContactPool(Id::Household);
        location->RegisterPool(pool4, Id::Household);

        auto pool5 = pop->RefPoolSys().CreateContactPool(Id::Workplace);
        location->RegisterPool(pool5, Id::Workplace);

        geoGrid.AddLocation(location);

        GeoGridJSONWriter writer;
        stringstream ss;
        writer.Write(geoGrid, ss);

        string expectedJSON = R"({
            "locations": [
            {
                "contactPools": [
                    {
                        "class": "Household",
                        "pools": [
                            {
                                "id": 1,
                                "people": []
                            }
                        ]
                    },
                    {
                        "class": "K12School",
                        "pools": [
                            {
                                "id": 1,
                                "people": []
                            }
                       ]
                    },
                    {
                        "class": "College",
                        "pools": [
                            {
                                "id": 1,
                                "people": []
                            }
                        ]
                    },
                    {
                        "class": "Workplace",
                        "pools": [
                            {
                                "id": 1,
                                "people": []
                            }
                        ]
                    },
                    {
                        "class": "PrimaryCommunity",
                        "pools": [
                            {
                                "id": 1,
                                "people": []
                            }
                        ]
                    }
                ],
                "coordinate": {
                    "latitude": 0.0,
                    "longitude": 0.0
                },
                "id": 1,
                "name": "Bavikhove",
                "population": 2500,
                "province": 4
            }],
            "persons": []
        })";

        EXPECT_TRUE(compareJSONs(ss.str(), expectedJSON));
}

TEST(GeoGridJSONWriterUnitTest, peopleTest)
{
        auto pop = Population::Create();
        auto geoGrid = GetPopulatedGeoGrid(pop.get());

        string expectedJSON = R"({
            "locations": [
            {
                "contactPools": [
                    {
                        "class": "Household",
                        "pools": [
                            {
                                "id": 1,
                                "people": [0]
                            }
                        ]
                    },
                    {
                        "class": "Daycare",
                        "pools": [
                           {
                                "id": 1,
                                "people": []
                           }
                        ]
                    },
                    {
                        "class": "PreSchool",
                        "pools": [
                           {
                                "id": 1,
                                "people": []
                           }
                        ]
                    },
                    {
                        "class": "K12School",
                        "pools": [
                            {
                                "id": 1,
                                "people": [0]
                            }
                        ]
                    },
                    {
                        "class": "College",
                        "pools": [
                            {
                                "id": 1,
                                "people": [0]
                            }
                        ]
                    },
                    {
                        "class": "Workplace",
                        "pools": [
                            {
                                "id": 1,
                                "people": [0]
                            }
                        ]
                    },
                    {
                        "class": "PrimaryCommunity",
                        "pools": [
                            {
                                "id": 1,
                                "people": [0]
                            }
                        ]
                    },
                    {
                        "class": "SecondaryCommunity",
                        "pools": [
                            {
                                "id": 1,
                                "people": [0]
                            }
                        ]
                    }
                ],
                "coordinate": {
                    "latitude": 0.0,
                    "longitude": 0.0
                },
                "id": 1,
                "name": "Bavikhove",
                "population": 2500,
                "province": 4
            }
        ],
        "persons": [
            {
                "age": 18,
                "college": 1,
                "daycare": 0,
                "household": 1,
                "id": 0,
                "k12School": 1,
                "preSchool": 0,
                "primaryCommunity": 1,
                "secondaryCommunity": 1,
                "workplace": 1
            }
        ]
        })";

        GeoGridJSONWriter writer;
        stringstream ss;
        writer.Write(*geoGrid, ss);

        EXPECT_TRUE(compareJSONs(ss.str(), expectedJSON));
}

TEST(GeoGridJSONWriterUnitTest, commutesTest)
{

        string expectedJSON = R"({
            "locations": [
            {
                "commutes": [
                    { "2": 0.5 },
                    { "3": 0.25}
                ],
                "contactPools": [],
                "coordinate": {
                    "latitude": 0.0,
                    "longitude": 0.0
                },
                "id": 1,
                "name": "Bavikhove",
                "population": 2500,
                "province": 4
            },
            {
                "commutes": [
                    {"1": 0.75},
                    {"3": 0.5}
                ],
                "contactPools": [],
                "coordinate": {
                    "latitude": 0.0,
                    "longitude": 0.0
                },
                "id": 2,
                "name": "Gent",
                "population": 2500,
                "province": 4
            },
            {
                "contactPools": [],
                "coordinate": {
                    "latitude": 0.0,
                    "longitude": 0.0
                },
                "id": 3,
                "name": "Mons",
                "population": 2500,
                "province": 4
            }
        ],
        "persons": []
        })";

        auto pop = Population::Create();
        auto geoGrid = GetCommutesGeoGrid(pop.get());

        GeoGridJSONWriter writer;
        stringstream ss;
        writer.Write(*geoGrid, ss);

        EXPECT_TRUE(compareJSONs(ss.str(), expectedJSON));
}

} // namespace
