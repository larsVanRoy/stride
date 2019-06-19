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


#include "geopop/io/proto_pb/epigeogrid.pb.h"
#include "pop/Population.h"
#include "geopop/EpiGrid.h"
#include "geopop/io/EpiProtoWriter.h"

#include <gtest/gtest.h>
#include <algorithm>
#include <iterator>

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace geopop;
using namespace proto;
using boost::geometry::get;

namespace {

void CompareCoordinate(const Coordinate& coordinate, const EpiGeoGrid_Location_Coordinates& epiCoordinate)
{
        using boost::geometry::get;
        EXPECT_EQ(get<0>(coordinate), epiCoordinate.longitude());
        EXPECT_EQ(get<1>(coordinate), epiCoordinate.latitude());
}

void CompareLocation(const Location<Coordinate>& location, const EpiGeoGrid_Location& protoLocation)
{
        EXPECT_EQ(location.GetName(), protoLocation.name());
        EXPECT_EQ(location.GetPopCount(), protoLocation.population());
        CompareCoordinate(location.GetCoordinate(), protoLocation.coordinates());
}
void compareGeoGrid(const GeoGrid& geoGrid, const EpiGeoGrid& protoGrid)
{
        EXPECT_EQ(geoGrid.size(), protoGrid.locations_size());
        EXPECT_EQ(protoGrid.history_size(), 1);

        for (int idx = 0; idx < protoGrid.locations_size(); idx ++)
        {
            const auto& protoLocation = protoGrid.locations(idx);
            auto        location      = geoGrid.GetById(static_cast<unsigned int>(protoLocation.id()));
            CompareLocation(*location, protoLocation);
        }

        const auto& history = protoGrid.history(0);

        EXPECT_EQ(history.timestep(), 0);

        for (int idx = 0; idx < history.poolsforlocations_size(); idx ++)
        {
            const auto& poolsForLocations = history.poolsforlocations(idx);

            EXPECT_EQ(poolsForLocations.pools_size(), 6);
        }
}

void CompareEpiGrid(const GeoGrid& geoGrid, const char* filename)
{
    EpiProtoWriter writer(filename);
    writer.Initialize(geoGrid);
    writer.Write(geoGrid, 0);
    writer.Finalize();
    EpiGeoGrid     protoGrid;
    stringstream   ss;
    ifstream       ifs;
    ifs.open(filename);
    copy(istreambuf_iterator<char>(ifs),
            istreambuf_iterator<char>(),
                    ostreambuf_iterator<char>(ss));
    ifs.close();
    protoGrid.ParseFromIstream(&ss);
    compareGeoGrid(geoGrid, protoGrid);
    remove(filename);
}

TEST(EpiGridProtoWriterTest, locationTest)
{
        auto pop = Population::Create();
        GeoGrid geoGrid(pop.get());

        geoGrid.AddLocation(make_shared<Location<Coordinate>>(1, 4, Coordinate(0, 0), "Bavikhove", 2500));
        geoGrid.AddLocation(make_shared<Location<Coordinate>>(2, 3, Coordinate(0, 0), "Gent", 5000));
        geoGrid.AddLocation(make_shared<Location<Coordinate>>(3, 2, Coordinate(0, 0), "Mons", 2500));

        CompareEpiGrid(geoGrid, "location.proto");
}

TEST(EpiGridProtoWriterTest, contactPoolsTest)
{
        auto pop = Population::Create();
        GeoGrid geoGrid(pop.get());

        auto loc = make_shared<Location<Coordinate>>(1, 4, Coordinate(0, 0), "Bavikhove", 2500);

        loc->RefPools(Id::Daycare).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Daycare));
        loc->RefPools(Id::PreSchool).emplace_back(pop->RefPoolSys().CreateContactPool(Id::PreSchool));
        loc->RefPools(Id::K12School).emplace_back(pop->RefPoolSys().CreateContactPool(Id::K12School));
        loc->RefPools(Id::PrimaryCommunity).emplace_back(pop->RefPoolSys().CreateContactPool(Id::PrimaryCommunity));
        loc->RefPools(Id::College).emplace_back(pop->RefPoolSys().CreateContactPool(Id::College));
        loc->RefPools(Id::Household).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Household));
        loc->RefPools(Id::Workplace).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Workplace));

        CompareEpiGrid(geoGrid, "contactPools.proto");
}
} // namespace
