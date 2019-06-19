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
using namespace proto;

shared_ptr<EpiGrid> EpiProtoReader::Read()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

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

    ParseHistory();
//    try{
//        ParseHistory();
//    }
//    catch(exception& e){
//        cout << e.what() << endl;
//        throw Exception("Problem parsing History of Protobuf file, check whether empty or invalid protobuf.");
//    }

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

shared_ptr<EpiLocation<Coordinate>> EpiProtoReader::ParseLocation(const proto::EpiGeoGrid_Location &location)
{
    int id = location.id();
    string name = location.name();
    uint32_t province = 0;
    int population = location.population();
    Coordinate coordinate = ParseCoordinate(location.coordinates());

    return make_shared<EpiLocation<Coordinate>>(id, province, coordinate, name, population);
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
            const EpiGeoGrid_History_PoolsForLocation& locPools = history.poolsforlocations(idx2);
            shared_ptr<EpiLocation<Coordinate>> epiLocation = m_grid->GetById(static_cast<uint>(locPools.id()));

            for (int idx3 = 0; idx3 < locPools.pools_size(); idx3 ++)
            {
                shared_ptr<PoolStatus> status = ParseLocationPools(locPools.pools(idx3));
                epiLocation->AddPoolStatus(status);
            }
        }
    }
}

void EpiProtoReader::ParseHistoryLocation(const proto::EpiGeoGrid_History_PoolsForLocation &loc)
{
    shared_ptr<EpiLocation<Coordinate>> epiLocation = m_grid->GetById(static_cast<uint>(loc.id()));

    for (int idx = 0; idx < loc.pools_size(); idx ++)
    {
        shared_ptr<PoolStatus> status = ParseLocationPools(loc.pools(idx));
        epiLocation->AddPoolStatus(status);
    }

}

shared_ptr<PoolStatus> EpiProtoReader::ParseLocationPools(const proto::EpiGeoGrid_History_PoolsForLocation_Pool &pool)
{
    static const map<int, AgeBrackets::AgeBracket> AgeBracketTypes = {
            {0, AgeBrackets::AgeBracket::Daycare},
            {1, AgeBrackets::AgeBracket::PreSchool},
            {2, AgeBrackets::AgeBracket::K12School},
            {3, AgeBrackets::AgeBracket::College},
            {4, AgeBrackets::AgeBracket::Workplace},
            {5, AgeBrackets::AgeBracket::Retired}};

    static const map<int, HealthStatus> HealthPoolTypes = {
            {0, HealthStatus::Exposed},
            {1, HealthStatus::Immune},
            {2, HealthStatus::Infectious},
            {3, HealthStatus::InfectiousAndSymptomatic},
            {4, HealthStatus::Recovered},
            {5, HealthStatus::Susceptible},
            {6, HealthStatus::Symptomatic}
    };

    shared_ptr<PoolStatus> status = make_shared<PoolStatus>();
    for (auto idx = 0; idx < 6; idx ++)
    {
        AgeBrackets::AgeBracket ageBracket = AgeBracketTypes.at(idx);
        shared_ptr<HealthPool> h = make_shared<HealthPool>();
        for (auto idx2 = 0; idx2 < 7; idx2 ++)
        {
            double percentage = pool.agebracketpercentage(idx2);
            h->setHealth(HealthPoolTypes.at(idx2), percentage);
        }
        status->addStatus(ageBracket, h);
    }

    return status;
}

void EpiProtoReader::Print() {
    for (unsigned int i = 0; i < m_grid->size(); ++i) {
        std::cout << "Location: \n";
        std::cout << "\tid: " << i << "\n";
        std::cout << "\tName: " << m_grid->operator[](i)->GetName() << "\n";
        std::cout << "\tpop count: " << m_grid->operator[](i)->GetPopCount() << "\n";
        std::cout << "\t(Coorindates: " << m_grid->operator[](i)->GetCoordinate().get<0>() << ", " << m_grid->operator[](i)->GetCoordinate().get<1>() << ")\n";
        for (size_t day = 0; day < m_grid->operator[](i)->maxDays(); day ++){
            std::cout << "\tday: " << day << "\n";
            std::cout << "\t\t" << m_grid->operator[](i)->GetPoolStatus(day)->operator[](stride::HealthStatus::Susceptible)[0] << "\n";
            std::cout << "\t\t" << m_grid->operator[](i)->GetPoolStatus(day)->operator[](stride::HealthStatus::Exposed)[0] << "\n";
            std::cout << "\t\t" << m_grid->operator[](i)->GetPoolStatus(day)->operator[](stride::HealthStatus::Infectious)[0] << "\n";
            std::cout << "\t\t" << m_grid->operator[](i)->GetPoolStatus(day)->operator[](stride::HealthStatus::Symptomatic)[0] << "\n";
            std::cout << "\t\t" << m_grid->operator[](i)->GetPoolStatus(day)->operator[](stride::HealthStatus::InfectiousAndSymptomatic)[0] << "\n";
            std::cout << "\t\t" << m_grid->operator[](i)->GetPoolStatus(day)->operator[](stride::HealthStatus::Recovered)[0] << "\n";
            std::cout << "\t\t" << m_grid->operator[](i)->GetPoolStatus(day)->operator[](stride::HealthStatus::Immune)[0] << "\n";
        }
        std::cout << std::endl;
    }
}

} // namespace geopop
