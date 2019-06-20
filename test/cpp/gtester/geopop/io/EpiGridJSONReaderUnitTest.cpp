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

#include "geopop/io/EpiJSONReader.h"
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
#include <memory>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::util;
using boost::property_tree::ptree;

namespace {

std::shared_ptr<EpiGrid> getEpiGridFromFile(const string& filename)
{
    auto file = make_unique<ifstream>();
    file->open(FileSys::GetTestsDir().string() + "/testdata/EpiOutputJSON/" + filename);
    if(!file->is_open()) {
        std::string s =
                "File not found: " + FileSys::GetTestsDir().string() + "/testdata/EpiOutputJSON/" + filename + "\n";
        throw (Exception(s));
    }
    EpiJSONReader reader(move(file));
    return reader.Read();
}


TEST(EpiGridJSONReaderTest, locationsTest)
{
    std::shared_ptr<EpiGrid> epiGrid = getEpiGridFromFile("test0.json");

    map<unsigned int, shared_ptr<EpiLocation<Coordinate>>> epilocations;
    epilocations[(*epiGrid.get())[0]->GetID()] = (*epiGrid.get())[0];
    epilocations[(*epiGrid.get())[1]->GetID()] = (*epiGrid.get())[1];
    epilocations[(*epiGrid.get())[2]->GetID()] = (*epiGrid.get())[2];

    const auto location1 = epilocations[1];
    const auto location2 = epilocations[2];
    const auto location3 = epilocations[3];

    EXPECT_EQ(location1->GetID(), 1);
    EXPECT_EQ(location1->GetName(), "Bavikhove");
    EXPECT_EQ(location1->GetPopCount(), 2500);
    EXPECT_EQ(get<0>(location1->GetCoordinate()), 10.0);
    EXPECT_EQ(get<1>(location1->GetCoordinate()), 8.0);

    EXPECT_EQ(location2->GetID(), 2);
    EXPECT_EQ(location2->GetName(), "Gent");
    EXPECT_EQ(location2->GetPopCount(), 5000);
    EXPECT_EQ(get<0>(location2->GetCoordinate()), 20.0);
    EXPECT_EQ(get<1>(location2->GetCoordinate()), 55.0);

    EXPECT_EQ(location3->GetID(), 3);
    EXPECT_EQ(location3->GetName(), "Mons");
    EXPECT_EQ(location3->GetPopCount(), 2500);
    EXPECT_EQ(get<0>(location3->GetCoordinate()), 30.0);
    EXPECT_EQ(get<1>(location3->GetCoordinate()), 40.0);
}

TEST(EpiGridJSONReaderTest, locationTest)
{
    std::shared_ptr<EpiGrid> epiGrid = getEpiGridFromFile("test1.json");

    const auto& location = (*epiGrid.get())[0];

    EXPECT_EQ(location->GetID(), 1);
    EXPECT_EQ(location->GetName(), "Bavikhove");
    EXPECT_EQ(location->GetPopCount(), 2500);
    EXPECT_EQ(get<0>(location->GetCoordinate()), 10.0);
    EXPECT_EQ(get<1>(location->GetCoordinate()), 8.0);
    EXPECT_EQ(location->maxDays(), 1);

    const auto& pool = location->GetPoolStatus(0);

    for(const auto& b : stride::AgeBrackets::AgeBracketList){
        for(const auto& h : stride::HealthStatusList){
            EXPECT_EQ(pool->getPercentage(b,{h}), 0);
        }
    }
}

TEST(EpiGridJSONReaderTest, historyTest)
{
    std::shared_ptr<EpiGrid> epiGrid = getEpiGridFromFile("test2.json");

    const auto& location = (*epiGrid.get())[0];

    EXPECT_EQ(location->GetID(), 1);
    EXPECT_EQ(location->GetName(), "Bavikhove");
    EXPECT_EQ(location->GetPopCount(), 84);
    EXPECT_EQ(get<0>(location->GetCoordinate()), 10.0);
    EXPECT_EQ(get<1>(location->GetCoordinate()), 8.0);
    EXPECT_EQ(location->maxDays(), 2);

    auto pool = location->GetPoolStatus(0);

    for(const auto& b : stride::AgeBrackets::AgeBracketList){
        for(const auto& h : stride::HealthStatusList){
            EXPECT_EQ(0.0, pool->getPercentage(b, {h}));
        }
    }

    pool = location->GetPoolStatus(1);

    for(const auto& b : stride::AgeBrackets::AgeBracketList){
        for(const auto& h : stride::HealthStatusList){
            EXPECT_EQ(0.0, pool->getPercentage(b, {h}));
        }
    }
}

TEST(EpiGridJSONReaderTest, agebracketTest)
{
    std::shared_ptr<EpiGrid> epiGrid = getEpiGridFromFile("test3.json");

    const auto& location = (*epiGrid.get())[0];

    EXPECT_EQ(location->GetID(), 1);
    EXPECT_EQ(location->GetName(), "Bavikhove");
    EXPECT_EQ(location->GetPopCount(), 42);
    EXPECT_EQ(get<0>(location->GetCoordinate()), 10.0);
    EXPECT_EQ(get<1>(location->GetCoordinate()), 8.0);
    EXPECT_EQ(location->maxDays(), 2);

    auto pool = location->GetPoolStatus(0);

    for(const auto& b : stride::AgeBrackets::AgeBracketList) {
        for(const auto& h : stride::HealthStatusList) {
            if(h == stride::HealthStatus::Susceptible) {
                EXPECT_DOUBLE_EQ(0.16666666666666666, pool->getPercentage(b, {h}));
            }
            else {
                EXPECT_DOUBLE_EQ(0.0, pool->getPercentage(b, {h}));
            }
        }
    }

    pool = location->GetPoolStatus(1);

    for(const auto& b : stride::AgeBrackets::AgeBracketList) {
        for (const auto &h : stride::HealthStatusList) {
            EXPECT_DOUBLE_EQ(0.023809523809523808, pool->getPercentage(b, {h}));
        }
    }
}
} // namespace