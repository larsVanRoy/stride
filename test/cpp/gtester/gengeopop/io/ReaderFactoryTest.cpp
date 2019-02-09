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

#include "gengeopop/io/ReaderFactory.h"
#include "gengeopop/GeoGridConfig.h"
#include "gengeopop/io/CitiesCSVReader.h"
#include "gengeopop/io/CommutesCSVReader.h"
#include "gengeopop/io/HouseholdCSVReader.h"
#include "util/FileSys.h"

#include <gtest/gtest.h>
#include <memory>

using namespace std;
using namespace gengeopop;
using namespace stride;
using namespace util;

namespace {

TEST(ReaderFactoryTest, TestCommutes)
{
        ReaderFactory readerFactory;

        const shared_ptr<CommutesReader>& res1 = readerFactory.CreateCommutesReader(string("flanders_cities.csv"));

        EXPECT_NE(dynamic_pointer_cast<CommutesCSVReader>(res1), nullptr);
        EXPECT_THROW(readerFactory.CreateCommutesReader(FileSys::GetTestsDir() / "testdata/io/empty.txt"),
                     runtime_error);
}

TEST(ReaderFactoryTest, TestCommutesFromFile)
{
        ReaderFactory readerFactory;

        const shared_ptr<CommutesReader>& res2 =
            readerFactory.CreateCommutesReader(FileSys::GetTestsDir() / "testdata/io/commutes.csv");
        const auto geoGrid = make_shared<GeoGrid>(Population::Create().get());
        geoGrid->AddLocation(make_shared<Location>(21, 0, 1000));
        geoGrid->AddLocation(make_shared<Location>(22, 0, 1000));

        res2->FillGeoGrid(geoGrid);

        EXPECT_EQ(geoGrid->GetById(21)->GetIncomingCommuterCount(1.0), 500);
        EXPECT_EQ(geoGrid->GetById(22)->GetOutgoingCommuterCount(1.0), 500);
        EXPECT_EQ(geoGrid->GetById(21)->GetIncomingCommuterCount(1.0), 500);
        EXPECT_EQ(geoGrid->GetById(22)->GetOutgoingCommuterCount(1.0), 500);
}

TEST(ReaderFactoryTest, TestCities)
{
        ReaderFactory                   readerFactory;
        const shared_ptr<CitiesReader>& res1 = readerFactory.CreateCitiesReader(string("flanders_cities.csv"));

        EXPECT_NE(dynamic_pointer_cast<CitiesCSVReader>(res1), nullptr);

        EXPECT_THROW(readerFactory.CreateCitiesReader(FileSys::GetTestsDir() / "testdata/io/empty.txt"), runtime_error);
        EXPECT_THROW(readerFactory.CreateCitiesReader(FileSys::GetTestsDir() / "testdata/io/random.txt"),
                     runtime_error);
}

TEST(ReaderFactoryTest, TestHouseHolds)
{

        ReaderFactory                      readerFactory;
        const shared_ptr<HouseholdReader>& res1 = readerFactory.CreateHouseholdReader(string("flanders_cities.csv"));

        EXPECT_NE(dynamic_pointer_cast<HouseholdCSVReader>(res1), nullptr);

        EXPECT_THROW(readerFactory.CreateHouseholdReader(FileSys::GetTestsDir() / "testdata/io/empty.txt"),
                     runtime_error);
}

} // namespace
