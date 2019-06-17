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
#include "geopop/io/GeoGridHDF5Writer.h"
#include "geopop/GeoGridConfig.h"

#include "contact/ContactPool.h"
#include "contact/ContactType.h"

#include "pop/Population.h"

#include "GeoGridIOUtils.h"
#include "geopop/io/GeoGridHDF5Types.h"
#include "util/FileSys.h"

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;
using namespace H5;
namespace {

bool compareLocations(const shared_ptr<Location<Coordinate>>& loc, const Group& group)
{
        unsigned int id, province, popCount;
        double       longitude, latitude;
        string       name;

        ReadHDF5Attribute("id", &id, group);
        ReadHDF5Attribute("province", &province, group);
        ReadHDF5Attribute("population", &popCount, group);
        ReadHDF5Attribute("longitude", &longitude, group);
        ReadHDF5Attribute("latitude", &latitude, group);
        ReadHDF5Attribute("name", &name, group);

        Location comp(id, province, Coordinate(longitude, latitude), name, popCount);

        return *loc == comp;
}

TEST(GeoGridHDF5WriterTest, locationTest)
{
        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        geoGrid.AddLocation(make_shared<Location<Coordinate>>(1, 4, Coordinate(0, 0), "Bavikhove", 2500));
        geoGrid.AddLocation(make_shared<Location<Coordinate>>(2, 3, Coordinate(0, 0), "Gent", 5000));
        geoGrid.AddLocation(make_shared<Location<Coordinate>>(3, 2, Coordinate(0, 0), "Mons", 2500));

        GeoGridHDF5Writer writer;
        string            filename = FileSys::GetTestsDir().string() + "/testdata/GeoGridHDF5/test.h5";
        writer.Write(geoGrid, filename);
        H5File file(filename, H5F_ACC_RDONLY);

        auto loc1 = file.openGroup("/Locations/Location1");
        auto loc2 = file.openGroup("/Locations/Location2");
        auto loc3 = file.openGroup("/Locations/Location3");

        EXPECT_TRUE(compareLocations(geoGrid[0], loc1));
        EXPECT_TRUE(compareLocations(geoGrid[1], loc2));
        EXPECT_TRUE(compareLocations(geoGrid[2], loc3));
}
TEST(GeoGridHDF5WriterTest, contactCentersTest)
{
        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());

        auto location = make_shared<Location<Coordinate>>(1, 4, Coordinate(0, 0), "Bavikhove", 2500);

        auto pool0 = pop->RefPoolSys().CreateContactPool(Id::Household);
        location->RegisterPool(pool0, Id::Household);

        auto pool1 = pop->RefPoolSys().CreateContactPool(Id::Daycare);
        location->RegisterPool(pool1, Id::Daycare);

        auto pool2 = pop->RefPoolSys().CreateContactPool(Id::PreSchool);
        location->RegisterPool(pool2, Id::PreSchool);

        auto pool3 = pop->RefPoolSys().CreateContactPool(Id::K12School);
        location->RegisterPool(pool3, Id::K12School);

        auto pool4 = pop->RefPoolSys().CreateContactPool(Id::College);
        location->RegisterPool(pool4, Id::College);

        auto pool5 = pop->RefPoolSys().CreateContactPool(Id::Workplace);
        location->RegisterPool(pool5, Id::Workplace);

        auto pool6 = pop->RefPoolSys().CreateContactPool(Id::PrimaryCommunity);
        location->RegisterPool(pool6, Id::PrimaryCommunity);

        auto pool7 = pop->RefPoolSys().CreateContactPool(Id::SecondaryCommunity);
        location->RegisterPool(pool7, Id::SecondaryCommunity);

        geoGrid.AddLocation(location);

        GeoGridHDF5Writer writer;
        string            filename = FileSys::GetTestsDir().string() + "/testdata/GeoGridHDF5/test.h5";
        writer.Write(geoGrid, filename);
        H5File file(filename, H5F_ACC_RDONLY);

        for (const auto& id : IdList) {
                auto dataset = file.openDataSet("/Locations/Location1/ContactPools/Pool" + to_string(ToSizeT(id) + 1));

                unsigned int pool_id;
                ReadHDF5Attribute("id", &pool_id, dataset);
                EXPECT_EQ(1, pool_id);

                string type;
                ReadHDF5Attribute("type", &type, dataset);
                EXPECT_EQ(ToString(id), type);

                size_t size;
                ReadHDF5Attribute("size", &size, dataset);
                EXPECT_EQ(0, size);
        }
        file.close();
}

TEST(GeoGridHDF5WriterTest, peopleTest)
{
        auto pop     = Population::Create();
        auto geoGrid = GetPopulatedGeoGrid(pop.get());

        GeoGridHDF5Writer writer;
        string            filename = FileSys::GetTestsDir().string() + "/testdata/GeoGridHDF5/test.h5";
        writer.Write(*geoGrid, filename);
        H5File file(filename, H5F_ACC_RDONLY);

        size_t size;
        ReadHDF5Attribute("size", &size, file);
        EXPECT_EQ(1, size);

        vector<H5Person> persons(1);
        ReadHDF5Dataset("People", persons.data(), file);
        auto p1          = pop->at(0);
        auto hdf5_person = persons[0];
        auto p2          = Person(hdf5_person.id, hdf5_person.age, hdf5_person.houseHold, hdf5_person.daycare,
                         hdf5_person.preschool, hdf5_person.k12School, hdf5_person.college, hdf5_person.workplace,
                         hdf5_person.primaryCommunity, hdf5_person.secondaryCommunity);

        EXPECT_EQ(p1.GetId(), p2.GetId());
        EXPECT_EQ(p1.GetAge(), p2.GetAge());
        for (auto id : IdList)
                EXPECT_EQ(p1.GetPoolId(id), p2.GetPoolId((id)));

        auto pools = file.openGroup("/Locations/Location1/ContactPools");
        {
                vector<unsigned int> data(1);
                ReadHDF5Dataset("Pool1", data.data(), pools);
                EXPECT_EQ(0, data[0]);
        }
        {
                size_t pool_size;
                auto   DCpool = file.openDataSet("/Locations/Location1/ContactPools/Pool2");
                ReadHDF5Attribute("size", &pool_size, DCpool);
                EXPECT_EQ(0, pool_size);
        }
        {
                size_t pool_size;
                auto   PSpool = file.openDataSet("/Locations/Location1/ContactPools/Pool3");
                ReadHDF5Attribute("size", &pool_size, PSpool);
                EXPECT_EQ(0, pool_size);
        }
        {
                vector<unsigned int> data(1);
                ReadHDF5Dataset("Pool4", data.data(), pools);
                EXPECT_EQ(0, data[0]);
        }
        {
                vector<unsigned int> data(1);
                ReadHDF5Dataset("Pool5", data.data(), pools);
                EXPECT_EQ(0, data[0]);
        }
        {
                vector<unsigned int> data(1);
                ReadHDF5Dataset("Pool6", data.data(), pools);
                EXPECT_EQ(0, data[0]);
        }
        {
                vector<unsigned int> data(1);
                ReadHDF5Dataset("Pool7", data.data(), pools);
                EXPECT_EQ(0, data[0]);
        }
        {
                vector<unsigned int> data(1);
                ReadHDF5Dataset("Pool8", data.data(), pools);
                EXPECT_EQ(0, data[0]);
        }
}

TEST(GeoGridHDF5WriterTest, commutesTest)
{
        auto pop     = Population::Create();
        auto geoGrid = GetCommutesGeoGrid(pop.get());

        GeoGridHDF5Writer writer;
        string            filename = FileSys::GetTestsDir().string() + "/testdata/GeoGridHDF5/test.h5";
        writer.Write(*geoGrid, filename);
        H5File file(filename, H5F_ACC_RDONLY);
        auto                   loc1 = file.openGroup("/Locations/Location1");
        std::vector<H5Commute> comm1(2);
        ReadHDF5Dataset("Commutes", comm1.data(), loc1);
        EXPECT_EQ(2, comm1[0].to);
        EXPECT_EQ(0.5, comm1[0].proportion);
        EXPECT_EQ(3, comm1[1].to);
        EXPECT_EQ(0.25, comm1[1].proportion);

        auto                   loc2 = file.openGroup("/Locations/Location2");
        std::vector<H5Commute> comm2(2);
        ReadHDF5Dataset("Commutes", comm2.data(), loc2);
        EXPECT_EQ(1, comm2[0].to);
        EXPECT_EQ(0.75, comm2[0].proportion);
        EXPECT_EQ(3, comm2[1].to);
        EXPECT_EQ(0.5, comm2[1].proportion);

        auto   comm3 = file.openDataSet("/Locations/Location3/Commutes");
        size_t size;
        ReadHDF5Attribute("size", &size, comm3);
        EXPECT_EQ(0, size);
}

} // namespace
