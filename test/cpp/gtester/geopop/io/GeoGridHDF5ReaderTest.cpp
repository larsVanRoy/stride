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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#include "geopop/io/GeoGridHDF5Reader.h"
#include "geopop/GeoGridConfig.h"

#include "contact/ContactPool.h"
#include "contact/ContactType.h"

#include "pop/Population.h"

#include "util/FileSys.h"
#include "util/Exception.h"

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

namespace {
void getGeoGridFromFile(const string& filename, Population* pop)
{
        auto              file = FileSys::GetTestsDir().string() + "/testdata/GeoGridHDF5/" + filename;
        GeoGridHDF5Reader geoGridHdf5Reader(file, pop);
        geoGridHdf5Reader.Read();
}

TEST(GeoGridHDF5ReaderTest, locationsTest)
{
        auto pop = Population::Create();
        getGeoGridFromFile("locationsTest.h5", pop.get());
        auto& geoGrid = pop->RefGeoGrid();
        {
                auto loc = geoGrid[0];
                EXPECT_EQ(loc->GetID(), 1);
                EXPECT_EQ(loc->GetName(), "Bavikhove");
                EXPECT_EQ(loc->GetProvince(), 4);
                EXPECT_EQ(loc->GetPopCount(), 2500);
                EXPECT_EQ(get<0>(loc->GetCoordinate()), 0);
                EXPECT_EQ(get<1>(loc->GetCoordinate()), 0);
        }
        {
                auto loc = geoGrid[1];
                EXPECT_EQ(loc->GetID(), 2);
                EXPECT_EQ(loc->GetName(), "Gent");
                EXPECT_EQ(loc->GetProvince(), 3);
                EXPECT_EQ(loc->GetPopCount(), 5000);
                EXPECT_EQ(get<0>(loc->GetCoordinate()), 0);
                EXPECT_EQ(get<1>(loc->GetCoordinate()), 0);
        }
        {
                auto loc = geoGrid[2];
                EXPECT_EQ(loc->GetID(), 3);
                EXPECT_EQ(loc->GetName(), "Mons");
                EXPECT_EQ(loc->GetProvince(), 2);
                EXPECT_EQ(loc->GetPopCount(), 2500);
                EXPECT_EQ(get<0>(loc->GetCoordinate()), 0);
                EXPECT_EQ(get<1>(loc->GetCoordinate()), 0);
        }
}

TEST(GeoGridHDF5ReaderTest, commutesTest)
{
        auto pop = Population::Create();
        getGeoGridFromFile("commutesTest.h5", pop.get());
        auto& geoGrid = pop->RefGeoGrid();

        {
                auto loc           = geoGrid[0];
                auto commuting_in  = loc->CRefIncomingCommutes();
                auto commuting_out = loc->CRefOutgoingCommutes();
                EXPECT_EQ(commuting_in.size(), 1);
                EXPECT_EQ(commuting_out.size(), 2);

                EXPECT_EQ(commuting_in[0].first->GetID(), 2);
                EXPECT_DOUBLE_EQ(commuting_in[0].second, 0.75);

                EXPECT_EQ(commuting_out[0].first->GetID(), 2);
                EXPECT_DOUBLE_EQ(commuting_out[0].second, 0.50);
                EXPECT_EQ(commuting_out[1].first->GetID(), 3);
                EXPECT_DOUBLE_EQ(commuting_out[1].second, 0.25);
        }
        {
                auto loc           = geoGrid[1];
                auto commuting_in  = loc->CRefIncomingCommutes();
                auto commuting_out = loc->CRefOutgoingCommutes();
                EXPECT_EQ(commuting_out.size(), 2);
                EXPECT_EQ(commuting_in.size(), 1);

                EXPECT_EQ(commuting_in[0].first->GetID(), 1);
                EXPECT_DOUBLE_EQ(commuting_in[0].second, 0.50);

                EXPECT_EQ(commuting_out[0].first->GetID(), 1);
                EXPECT_DOUBLE_EQ(commuting_out[0].second, 0.75);
                EXPECT_EQ(commuting_out[1].first->GetID(), 3);
                EXPECT_DOUBLE_EQ(commuting_out[1].second, 0.5);
        }
        {
                auto loc           = geoGrid[2];
                auto commuting_in  = loc->CRefIncomingCommutes();
                auto commuting_out = loc->CRefOutgoingCommutes();
                EXPECT_EQ(commuting_out.size(), 0);
                EXPECT_EQ(commuting_in.size(), 2);

                EXPECT_EQ(commuting_in[0].first->GetID(), 1);
                EXPECT_DOUBLE_EQ(commuting_in[0].second, 0.25);
                EXPECT_EQ(commuting_in[1].first->GetID(), 2);
                EXPECT_DOUBLE_EQ(commuting_in[1].second, 0.5);
        }
}

TEST(GeoGridHDF5ReaderTest, contactCentersTest)
{
        auto pop = Population::Create();
        getGeoGridFromFile("contactPoolTest.h5", pop.get());
        auto& geoGrid = pop->RefGeoGrid();

        auto loc = geoGrid[0];
        for (const auto& id : IdList) {
                EXPECT_EQ(1, loc->CRefPools(id).size());
                EXPECT_EQ(1, loc->CRefPools(id)[0]->GetId());
        }
}

TEST(GeoGridHDF5ReaderTest, peopleTest)
{
        auto pop = Population::Create();
        getGeoGridFromFile("peopleTest.h5", pop.get());
        auto& geoGrid = pop->RefGeoGrid();

        auto person = pop->at(0);
        auto loc    = geoGrid[0];

        EXPECT_EQ(0, person.GetId());
        EXPECT_DOUBLE_EQ(18.0, person.GetAge());
        for (Id type : IdList) {
                EXPECT_EQ(1, person.GetPoolId(type));
                EXPECT_EQ(1, loc->CRefPools(type)[0]->GetId());
        }
}

TEST(GeoGridHDF5ReaderTest, emptyFileTest)
{
        H5::Exception::dontPrint();
        auto pop = Population::Create();
        EXPECT_THROW(getGeoGridFromFile("empty.h5", pop.get()), Exception);
}
} // namespace
