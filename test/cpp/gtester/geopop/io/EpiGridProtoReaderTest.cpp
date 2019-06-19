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
#include "geopop/io/EpiProtoReader.h"

#include <gtest/gtest.h>

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace geopop;
using namespace proto;
using boost::geometry::get;

namespace {

void fillLocation(int id, const string& name, int population, double latitude, double longitude,
        EpiGeoGrid_Location* location)
{
        location->set_id(id);
        location->set_name(name);
        location->set_population(population);
        const auto protoCoordinate = new EpiGeoGrid_Location_Coordinates();
        protoCoordinate->set_latitude(latitude);
        protoCoordinate->set_longitude(longitude);
        location->set_allocated_coordinates(protoCoordinate);
}

void fillHistory(int timestep, EpiGeoGrid_History* history)
{
        history->set_timestep(timestep);
}

void fillPoolsForLocation(int id, EpiGeoGrid_History_PoolsForLocation* pool)
{
        pool->set_id(id);
}

void fillPool(vector<double> percentages, int typeId, EpiGeoGrid_History_PoolsForLocation_Pool* pool)
{
        static const map<int, EpiGeoGrid_History_PoolsForLocation_Pool_AgeBracket> AgeBracketTypes = {
                {0, EpiGeoGrid_History_PoolsForLocation_Pool_AgeBracket_Daycare},
                {1, EpiGeoGrid_History_PoolsForLocation_Pool_AgeBracket_PreSchool},
                {2, EpiGeoGrid_History_PoolsForLocation_Pool_AgeBracket_K12School},
                {3, EpiGeoGrid_History_PoolsForLocation_Pool_AgeBracket_College},
                {4, EpiGeoGrid_History_PoolsForLocation_Pool_AgeBracket_Workplace},
                {5, EpiGeoGrid_History_PoolsForLocation_Pool_AgeBracket_Retired}};

        for(const auto& percentage : percentages)
        {
                pool->add_agebracketpercentage(percentage);
        }

        pool->set_type(AgeBracketTypes.at(typeId));
}

void CompareCoordinate(const Coordinate& coordinate, const EpiGeoGrid_Location_Coordinates& epiCoordinate)
{
        using boost::geometry::get;
        EXPECT_EQ(get<0>(coordinate), epiCoordinate.longitude());
        EXPECT_EQ(get<1>(coordinate), epiCoordinate.latitude());
}

void CompareLocation(const EpiLocation<Coordinate>& location, const EpiGeoGrid_Location& protoLocation)
{
        EXPECT_EQ(location.GetName(), protoLocation.name());
        EXPECT_EQ(location.GetPopCount(), protoLocation.population());
        CompareCoordinate(location.GetCoordinate(), protoLocation.coordinates());
}

void ComparePools(EpiLocation<Coordinate> location, const EpiGeoGrid_History_PoolsForLocation& pools)
{
        static const map<int, HealthStatus> HealthPoolTypes = {
                {0, HealthStatus::Exposed},
                {1, HealthStatus::Immune},
                {2, HealthStatus::Infectious},
                {3, HealthStatus::InfectiousAndSymptomatic},
                {4, HealthStatus::Recovered},
                {5, HealthStatus::Susceptible},
                {6, HealthStatus::Symptomatic}
        };

        for (size_t day = 0; day < location.maxDays(); day ++)
        {
                auto dayStatus = location.GetPoolStatus(static_cast<uint>(day));
                for(int healthType = 0; healthType < 7; healthType ++)
                {
                        vector<double> location_percentage = dayStatus->operator[](HealthPoolTypes.at(healthType));
                        for(int poolType = 0; poolType < pools.pools_size(); poolType ++)
                        {
                                double pool_percentage = pools.pools(poolType).agebracketpercentage(healthType);
                                EXPECT_EQ(location_percentage[poolType], pool_percentage);
                        }
                }
        }
}

void CompareEpiGrid(const shared_ptr<EpiGrid>& epiGrid, const EpiGeoGrid& protoGrid)
{
        EXPECT_EQ(epiGrid->size(), protoGrid.locations_size());

        for (int idx = 0; idx < protoGrid.locations_size(); idx ++)
        {
                const auto& protoLocation = protoGrid.locations(idx);
                auto        location      = epiGrid->GetById(static_cast<unsigned int>(protoLocation.id()));
                CompareLocation(*location, protoLocation);
        }

        for (int idx = 0; idx < protoGrid.history_size(); idx ++)
        {
                const auto& history = protoGrid.history(idx);
                for (int idx2 = 0; idx2 < history.poolsforlocations_size(); idx2 ++)
                {
                    const auto& pools         = history.poolsforlocations(idx2);
                    auto        location      = epiGrid->GetById(static_cast<unsigned int>(pools.id()));
                    ComparePools(*location, pools);
                }
        }
}

void CompareEpiGrid(const EpiGeoGrid& protoGrid, const char* filename)
{
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        auto ss = ofstream(filename);
        protoGrid.SerializeToOstream(&ss);
        ss.close();
        unique_ptr<ifstream> is = make_unique<ifstream>(filename);
        EpiProtoReader       reader(move(is));
        shared_ptr<EpiGrid> epiGrid = reader.Read();
        CompareEpiGrid(epiGrid, protoGrid);
        remove(filename);
}

TEST(EpiGridProtoReaderTest, locationTest)
{
        EpiGeoGrid epiGrid;

        fillLocation(1, "Bavikhove", 2500, 0, 0, epiGrid.add_locations());
        fillLocation(2, "Gent", 5000, 0, 0, epiGrid.add_locations());
        fillLocation(3, "Mons", 2500, 0, 0, epiGrid.add_locations());

        CompareEpiGrid(epiGrid, "locationTest.proto");
}

TEST(EpiGridProtoReaderTest, historyTest)
{
        EpiGeoGrid epiGrid;

        fillLocation(1, "Bavikhove", 2500, 0, 0, epiGrid.add_locations());
        fillLocation(2, "Gent", 5000, 0, 0, epiGrid.add_locations());
        fillLocation(3, "Mons", 2500, 0, 0, epiGrid.add_locations());

        fillHistory(0, epiGrid.add_history());
        fillHistory(1, epiGrid.add_history());
        fillHistory(2, epiGrid.add_history());

        CompareEpiGrid(epiGrid, "historyTest.proto");
}

TEST(EpiGridProtoReaderTest, Poolstest)
{
        EpiGeoGrid epiGrid;

        fillLocation(1, "Bavikhove", 2500, 0, 0, epiGrid.add_locations());
        fillLocation(2, "Gent", 5000, 0, 0, epiGrid.add_locations());
        fillLocation(3, "Mons", 2500, 0, 0, epiGrid.add_locations());

        EpiGeoGrid_History* history = epiGrid.add_history();
        fillHistory(0, history);

        EpiGeoGrid_History_PoolsForLocation* poolsForLocations = history->add_poolsforlocations();
        fillPoolsForLocation(1, poolsForLocations);

        vector<double> percentages = {0.2, 0.2, 0.2, 0.2, 0.2, 0, 0};
        fillPool(percentages, 0, poolsForLocations->add_pools());
        fillPool(percentages, 1, poolsForLocations->add_pools());
        fillPool(percentages, 2, poolsForLocations->add_pools());
        fillPool(percentages, 3, poolsForLocations->add_pools());
        fillPool(percentages, 4, poolsForLocations->add_pools());
        fillPool(percentages, 5, poolsForLocations->add_pools());

        CompareEpiGrid(epiGrid, "poolsTest.proto");

}
} // namespace
