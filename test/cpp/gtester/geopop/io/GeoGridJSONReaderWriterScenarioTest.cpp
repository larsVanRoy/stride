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

    void getGeoGridFromFile(const string& filename, Population* pop)
    {
        auto file = make_unique<ifstream>();
        file->open(FileSys::GetTestsDir().string() + "/testdata/GeoGridJSON/" + filename);
        GeoGridJSONReader geoGridJSONReader(move(file), pop);
        geoGridJSONReader.Read();
    }


    shared_ptr<GeoGrid> CreateGeoGrid(Population* pop)
    {
        const auto geoGrid = make_shared<GeoGrid>(pop);
        const auto loc     = make_shared<Location>(1, 4, Coordinate(0, 0), "Bavikhove", 2500);

        auto dayPool = pop->RefPoolSys().CreateContactPool(Id::Daycare);
        loc->RefPools(Id::Daycare).emplace_back(dayPool);
        auto prePool = pop->RefPoolSys().CreateContactPool(Id::PreSchool);
        loc->RefPools(Id::PreSchool).emplace_back(prePool);
        auto k12Pool = pop->RefPoolSys().CreateContactPool(Id::K12School);
        loc->RefPools(Id::K12School).emplace_back(k12Pool);
        auto pcPool = pop->RefPoolSys().CreateContactPool(Id::PrimaryCommunity);
        loc->RefPools(Id::PrimaryCommunity).emplace_back(pcPool);
        auto scPool = pop->RefPoolSys().CreateContactPool(Id::SecondaryCommunity);
        loc->RefPools(Id::SecondaryCommunity).emplace_back(scPool);
        auto cPool = pop->RefPoolSys().CreateContactPool(Id::College);
        loc->RefPools(Id::College).emplace_back(cPool);
        auto hPool = pop->RefPoolSys().CreateContactPool(Id::Household);
        loc->RefPools(Id::Household).emplace_back(hPool);
        auto wPool = pop->RefPoolSys().CreateContactPool(Id::Workplace);
        loc->RefPools(Id::Workplace).emplace_back(wPool);

        geoGrid->AddLocation(loc);

        const auto person = geoGrid->GetPopulation()->CreatePerson(
                0, 18, hPool->GetId(), 0, 0, k12Pool->GetId(), cPool->GetId(), wPool->GetId(),
                pcPool->GetId(), scPool->GetId());

        const auto person2 = geoGrid->GetPopulation()->CreatePerson(
                1, 26, 0, 0, prePool->GetId(), 0, 0, 0, 0, 0);

        k12Pool->AddMember(person);
        pcPool->AddMember(person);
        scPool->AddMember(person);
        cPool->AddMember(person);
        hPool->AddMember(person);
        wPool->AddMember(person);

        prePool->AddMember(person2);


        return geoGrid;
    }

    TEST(GeoGridJSONReaderWriterScenarioTest, mainTest)
    {
        auto pop = Population::Create();
        auto geoGrid = CreateGeoGrid(pop.get());

        GeoGridJSONWriter writer;
        stringstream ss;
        writer.Write(*geoGrid, ss);

        string expected1 = R"(
            {
                "locations":[
                    {
                    "contactPools":[
                        {
                            "class":"Household",
                            "pools":[{"id":1,"people":[0]}]
                        },
                        {
                            "class":"Daycare",
                            "pools":[{"id":1,"people":[]}]
                        },
                        {
                            "class":"PreSchool",
                            "pools":[{"id":1,"people":[1]}]
                        },
                        {
                            "class":"K12School",
                            "pools":[{"id":1,"people":[0]}]
                        },
                        {
                            "class":"College",
                            "pools":[{"id":1,"people":[0]}]
                        },
                        {
                            "class":"Workplace",
                            "pools":[{"id":1,"people":[0]}]
                        },
                        {
                            "class":"PrimaryCommunity",
                            "pools":[{"id":1,"people":[0]}]
                        },
                        {
                            "class":"SecondaryCommunity",
                            "pools":[{"id":1,"people":[0]}]
                        }],
                    "coordinate":{"latitude":0.0,"longitude":0.0},
                    "id":1,
                    "name":"Bavikhove",
                    "population":2500,
                    "province":4
                    }
                ],
                "persons":[
                    {
                        "age":18,
                        "college":1,
                        "daycare":0,
                        "household":1,
                        "id":0,
                        "k12School":1,
                        "preSchool":0,
                        "primaryCommunity":1,
                        "secondaryCommunity":1,
                        "workplace":1
                    },
                    {
                        "age":26,
                        "college":0,
                        "daycare":0,
                        "household":0,
                        "id":1,
                        "k12School":0,
                        "preSchool":1,
                        "primaryCommunity":0,
                        "secondaryCommunity":0,
                        "workplace":0
                    }
                ]})";

        ofstream outputFileStream(FileSys::GetTestsDir().string() + "/testdata/GeoGridJSON/OWN_readerwriter.json");
        outputFileStream << ss.str();

        EXPECT_TRUE(compareJSONs(ss.str(), expected1));

        auto pop2 = Population::Create();
        getGeoGridFromFile("OWN_readerwriter.json", pop2.get());
        auto& geoGrid2 = pop2->RefGeoGrid();

        const auto location1 = geoGrid2[0];

        EXPECT_EQ(1, location1->GetID());
        EXPECT_EQ("Bavikhove", location1->GetName());
        EXPECT_EQ(4, location1->GetProvince());
        EXPECT_EQ(2500, location1->GetPopCount());
        EXPECT_EQ(0, get<0>(location1->GetCoordinate()));
        EXPECT_EQ(0, get<1>(location1->GetCoordinate()));

        vector<stride::ContactPool*> preschoolPools;
        for (const auto& p : location1->RefPools(Id::PreSchool)){
            preschoolPools.emplace_back(p);
        }

        auto person_preschool = *(preschoolPools[0]->begin());
        EXPECT_EQ(ToString(Id::PreSchool), ToString(preschoolPools[0]->GetType()));
        EXPECT_EQ(1, person_preschool->GetId());
        EXPECT_EQ(26, person_preschool->GetAge());
        EXPECT_EQ(0, person_preschool->GetPoolId(Id::College));
        EXPECT_EQ(0, person_preschool->GetPoolId(Id::Daycare));
        EXPECT_EQ(0, person_preschool->GetPoolId(Id::Household));
        EXPECT_EQ(0, person_preschool->GetPoolId(Id::K12School));
        EXPECT_EQ(1, person_preschool->GetPoolId(Id::PreSchool));
        EXPECT_EQ(0, person_preschool->GetPoolId(Id::PrimaryCommunity));
        EXPECT_EQ(0, person_preschool->GetPoolId(Id::SecondaryCommunity));
        EXPECT_EQ(0, person_preschool->GetPoolId(Id::Workplace));
    }


} // namespace
