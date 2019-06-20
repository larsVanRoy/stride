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

#include "GeoGridIOUtils.h"
#include "geopop/EpiGrid.h"
#include "geopop/io/EpiGridHDF5Types.h"
#include "geopop/io/EpiHDF5Writer.h"
#include "pop/Population.h"
#include "util/FileSys.h"

#include <algorithm>
#include <gtest/gtest.h>
#include <iterator>

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;
using namespace geopop;
using namespace H5;
using boost::geometry::get;

namespace {

TEST(EpiGridHDF5WriterTest, locationTest)
{
        auto    pop = Population::Create();
        GeoGrid geoGrid(pop.get());

        geoGrid.AddLocation(make_shared<Location<Coordinate>>(1, 4, Coordinate(0, 0), "Bavikhove", 2500));
        geoGrid.AddLocation(make_shared<Location<Coordinate>>(2, 3, Coordinate(0, 0), "Gent", 5000));
        geoGrid.AddLocation(make_shared<Location<Coordinate>>(3, 2, Coordinate(0, 0), "Mons", 2500));

        string filename = FileSys::GetTestsDir().string() + "/testdata/GeoGridHDF5/test.h5";
        auto   writer   = EpiHDF5Writer(filename);
        writer.Initialize(geoGrid);
        writer.Write(geoGrid, 1);
        writer.Finalize();

        auto file = H5File(filename, H5F_ACC_RDONLY);

        size_t nrTimesteps;
        ReadHDF5Attribute("nrTimesteps", &nrTimesteps, file);
        EXPECT_EQ(1, nrTimesteps);

        std::vector<unsigned int> timesteps(nrTimesteps);
        ReadHDF5Dataset("Timesteps", timesteps.data(), file);
        EXPECT_EQ(1, timesteps[0]);

        {
                auto locations_group = file.openDataSet("Locations");

                size_t nrLocations = 0;
                ReadHDF5Attribute("size", &nrLocations, locations_group);
                EXPECT_EQ(3, nrLocations);

                std::vector<H5GeoLocation> locations(nrLocations);
                ReadHDF5Dataset("Locations", locations.data(), file);
                for (size_t i = 0; i < geoGrid.size(); ++i) {
                        auto loc    = geoGrid[i];
                        auto h5_loc = locations[i];
                        EXPECT_EQ(loc->GetID(), h5_loc.id);
                        EXPECT_EQ(loc->GetProvince(), h5_loc.province);
                        EXPECT_EQ(loc->GetName(), std::string(h5_loc.name));
                        EXPECT_EQ(loc->GetPopCount(), h5_loc.population);
                        EXPECT_EQ(get<0>(loc->GetCoordinate()), h5_loc.longitude);
                        EXPECT_EQ(get<1>(loc->GetCoordinate()), h5_loc.latitude);
                }
        }

        for (auto timestep : timesteps) {
                auto step_group = file.openGroup("Steps").openGroup("Step" + to_string(timestep));

                unsigned int nrLocations;
                ReadHDF5Attribute("nrLocations", &nrLocations, step_group);
                EXPECT_EQ(3, nrLocations);

                for (size_t i = 0; i < nrLocations; ++i) {
                        unsigned int id;
                        auto         location_group = step_group.openGroup("Location" + to_string(i));
                        ReadHDF5Attribute("id", &id, location_group);
                        EXPECT_EQ(i + 1, id);
                }
        }
        file.close();


TEST(EpiGridHDF5WriterTest, contactPoolsTest)
{
        auto    pop = Population::Create();
        GeoGrid geoGrid(pop.get());

        auto loc = make_shared<Location<Coordinate>>(1, 4, Coordinate(0, 0), "Bavikhove", 2500);

        loc->RefPools(Id::Daycare).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Daycare));
        loc->RefPools(Id::PreSchool).emplace_back(pop->RefPoolSys().CreateContactPool(Id::PreSchool));
        loc->RefPools(Id::K12School).emplace_back(pop->RefPoolSys().CreateContactPool(Id::K12School));
        loc->RefPools(Id::PrimaryCommunity).emplace_back(pop->RefPoolSys().CreateContactPool(Id::PrimaryCommunity));
        loc->RefPools(Id::College).emplace_back(pop->RefPoolSys().CreateContactPool(Id::College));
        loc->RefPools(Id::Household).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Household));
        loc->RefPools(Id::Workplace).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Workplace));

        geoGrid.AddLocation(loc);

        string filename = FileSys::GetTestsDir().string() + "/testdata/GeoGridHDF5/test.h5";
        auto   writer   = EpiHDF5Writer(filename);
        writer.Initialize(geoGrid);
        writer.Write(geoGrid, 1);
        writer.Finalize();

        auto file = H5File(filename, H5F_ACC_RDONLY);
        auto brackets_group =
            file.openGroup("Steps").openGroup("Step1").openGroup("Location0").openGroup("AgeBrackets");

        for(const AgeBrackets::AgeBracket& bracket : AgeBrackets::AgeBracketList) {
                std::vector<double> health_statuses(NumOfHealthStatus());
                ReadHDF5Dataset(AgeBrackets::AgeBracketToString(bracket), health_statuses.data(), brackets_group);
                for(auto status : health_statuses){
                        EXPECT_EQ(0, status);
                }

        }
        file.close();
}
} // namespace
