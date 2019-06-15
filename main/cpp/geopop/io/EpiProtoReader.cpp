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
 *  Copyright 2018, 2019Jan Broeckhove and Bistromatics group.
 */

#include "EpiProtoReader.h"
#include "util/Exception.h"

#include <iostream>
#include <stdexcept>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::util;
using namespace stride::ContactType;

shared_ptr<EpiGrid> EpiProtoReader::Read()
{
    if (!m_protogrid.ParseFromIstream(m_stream.get())){
        throw runtime_error("Failed to parse Proto file");
    }

    try {
        ParseLocations();
    }
    catch(exception& e) {
        cout << e.what() << endl;
        throw Exception("Problem parsing Locations of protobuf file, check whether empty or invalid protobuf.");
    }

    try{
        ParseHistory();
    }
    catch(exception& e){
        cout << e.what() << endl;
        throw Exception("Problem parsing History of Protobuf file, check whether empty or invalid protobuf.");
    }

    return m_grid;
}

void EpiProtoReader::ParseLocations()
{
    for (int idx = 0; idx < m_protogrid.locations_size(); idx++)
    {
        auto location = ParseLocation(m_protogrid.locations(idx));
        m_grid->AddLocation(location);
    }
}

shared_ptr<EpiLocation> EpiProtoReader::ParseLocation(const proto::EpiGeoGrid_Location &location)
{
    int id = location.id();
    string name = location.name();
    uint32_t province = 0;
    int population = location.population();
    Coordinate coordinate = ParseCoordinate(location.coordinates());

    return make_shared<EpiLocation>(id, province, coordinate, name, population);
}

Coordinate EpiProtoReader::ParseCoordinate(const proto::EpiGeoGrid_Location_Coordinates &coordinates)
{
    double latitude = coordinates.latitude();
    double longitude = coordinates.longitude();

    return Coordinate(longitude, latitude);
}

void EpiProtoReader::ParseHistory()
{
    for (int idx = 0; idx < m_protogrid.history_size(); idx ++)
    {
        const proto::EpiGeoGrid_History& history = m_protogrid.history(idx);
        for (int idx2 = 0; idx2 < history.poolsforlocations_size(); idx2 ++)
        {
            ParseHistoryLocation(history.poolsforlocations(idx2));
        }
    }
}

void EpiProtoReader::ParseHistoryLocation(const proto::EpiGeoGrid_History_PoolsForLocation &loc)
{
    static const map<proto::EpiGeoGrid_History_PoolsForLocation_Pool_Type , Id> types = {
            {proto::EpiGeoGrid_History_PoolsForLocation_Pool_Type_K12School, Id::K12School},
            {proto::EpiGeoGrid_History_PoolsForLocation_Pool_Type_PrimaryCommunity, Id::PrimaryCommunity},
            {proto::EpiGeoGrid_History_PoolsForLocation_Pool_Type_SecondaryCommunity, Id::SecondaryCommunity},
            {proto::EpiGeoGrid_History_PoolsForLocation_Pool_Type_College, Id::College},
            {proto::EpiGeoGrid_History_PoolsForLocation_Pool_Type_Household, Id::Household},
            {proto::EpiGeoGrid_History_PoolsForLocation_Pool_Type_Workplace, Id::Workplace},
            {proto::EpiGeoGrid_History_PoolsForLocation_Pool_Type_Daycare, Id::Daycare},
            {proto::EpiGeoGrid_History_PoolsForLocation_Pool_Type_PreSchool, Id::PreSchool}};

    shared_ptr<EpiLocation> epiLocation = m_grid->GetById(static_cast<uint>(loc.id()));
    shared_ptr<PoolStatus> status = make_shared<PoolStatus>();

    for (int idx = 0; idx < loc.pools_size(); idx ++)
    {
        shared_ptr<HealthPool> h = ParseLocationPools(loc.pools(idx));
        Id i = types.at(loc.pools(idx).type());
        status->addStatus(i, h);
    }

    epiLocation->AddPoolStatus(status);

}

shared_ptr<HealthPool> EpiProtoReader::ParseLocationPools(const proto::EpiGeoGrid_History_PoolsForLocation_Pool &pool)
{
    shared_ptr<HealthPool> h = make_shared<HealthPool>();
    for (int idx = 0; idx < pool.percentage_size(); idx ++)
    {
        double fraction = pool.percentage(idx);
        h->setHealth(static_cast<HealthStatus>(idx), fraction);
    }

    return h;
}

void EpiProtoReader::Print() {
    for (unsigned int i = 0; i < m_grid->size(); ++i) {
        std::cout << "Location: " << i << "\n";
        std::cout << "\t" << m_grid->operator[](i)->GetName() << "\n";
        std::cout << "\t" << m_grid->operator[](i)->GetPopCount() << "\n";
        std::cout << "\t(" << m_grid->operator[](i)->GetCoordinate().get<0>() << ", " << m_grid->operator[](i)->GetCoordinate().get<1>() << ")\n";
        std::cout << "\t" << m_grid->operator[](i)->GetPoolStatus(0)->operator[](stride::HealthStatus::Susceptible)[0] << "\n";
        std::cout << "\t" << m_grid->operator[](i)->GetPoolStatus(0)->operator[](stride::HealthStatus::Exposed)[0] << "\n";
        std::cout << "\t" << m_grid->operator[](i)->GetPoolStatus(0)->operator[](stride::HealthStatus::Infectious)[0] << "\n";
        std::cout << "\t" << m_grid->operator[](i)->GetPoolStatus(0)->operator[](stride::HealthStatus::Symptomatic)[0] << "\n";
        std::cout << "\t" << m_grid->operator[](i)->GetPoolStatus(0)->operator[](stride::HealthStatus::InfectiousAndSymptomatic)[0] << "\n";
        std::cout << "\t" << m_grid->operator[](i)->GetPoolStatus(0)->operator[](stride::HealthStatus::Recovered)[0] << "\n";
        std::cout << "\t" << m_grid->operator[](i)->GetPoolStatus(0)->operator[](stride::HealthStatus::Immune)[0] << "\n";
        std::cout << std::endl;
    }
}

} // namespace geopop
