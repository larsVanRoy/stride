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

#include "EpiProtoWriter.h"

#include "contact/ContactPool.h"
#include "geopop/Location.h"
#include "geopop/GeoGrid.h"
#include "geopop/geo/Coordinate.h"
#include "pop/Person.h"
#include "util/Exception.h"
#include "disease/Health.h"

#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <omp.h>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace boost::property_tree;

EpiProtoWriter::EpiProtoWriter(const std::string& filename) : EpiWriter(), m_stream(filename), m_protogrid()
{
}

void EpiProtoWriter::Initialize(const geopop::GeoGrid &geoPopGrid)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    for (unsigned i = 0; i < geoPopGrid.size(); i++) {
        WriteLocation(*geoPopGrid[i], m_protogrid.add_locations());
    }

    google::protobuf::ShutdownProtobufLibrary();
}

void EpiProtoWriter::Write(const geopop::GeoGrid &geoGrid, unsigned timeStep)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    auto history = m_protogrid.add_history();
    history->set_timestep(timeStep);
    for (unsigned i = 0; i < geoGrid.size(); i ++) {
        WriteHealthStatus(*geoGrid[i], history);
    }

    google::protobuf::ShutdownProtobufLibrary();
}

void EpiProtoWriter::Finalize()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if(!m_protogrid.SerializeToOstream(&m_stream)){
        throw stride::util::Exception("There was an error writing the EpiGeoGrid to the file.");
    }

    google::protobuf::ShutdownProtobufLibrary();
    m_stream.close();
}

void EpiProtoWriter::WriteCoordinates(const Coordinate&                         coordinate,
                                      proto::EpiGeoGrid_Location_Coordinates*   protoCoordinates)
{
    protoCoordinates->set_longitude(boost::geometry::get<0>(coordinate));
    protoCoordinates->set_latitude(boost::geometry::get<1>(coordinate));
}

void EpiProtoWriter::WriteLocation(Location<Coordinate>& location, proto::EpiGeoGrid_Location *protoLocation)
{
    protoLocation->set_id(location.GetID());
    protoLocation->set_name(location.GetName());
    protoLocation->set_population(location.GetPopCount());
    auto coordinates = new proto::EpiGeoGrid_Location_Coordinates();
    WriteCoordinates(location.GetCoordinate(), coordinates);
    protoLocation->set_allocated_coordinates(coordinates);
}

void EpiProtoWriter::WriteHealthStatus(geopop::Location<Coordinate> &location, proto::EpiGeoGrid_History* protoHistory)
{
    static const map<AgeBrackets::AgeBracket, proto::EpiGeoGrid_History_PoolsForLocation_Pool_AgeBracket > types = {
            {AgeBrackets::AgeBracket::Daycare, proto::EpiGeoGrid_History_PoolsForLocation_Pool_AgeBracket_Daycare},
            {AgeBrackets::AgeBracket::PreSchool, proto::EpiGeoGrid_History_PoolsForLocation_Pool_AgeBracket_PreSchool},
            {AgeBrackets::AgeBracket::K12School, proto::EpiGeoGrid_History_PoolsForLocation_Pool_AgeBracket_K12School},
            {AgeBrackets::AgeBracket::College, proto::EpiGeoGrid_History_PoolsForLocation_Pool_AgeBracket_College},
            {AgeBrackets::AgeBracket::Retired, proto::EpiGeoGrid_History_PoolsForLocation_Pool_AgeBracket_Retired},
            {AgeBrackets::AgeBracket::Workplace, proto::EpiGeoGrid_History_PoolsForLocation_Pool_AgeBracket_Workplace}};


    auto protoPools = protoHistory->add_poolsforlocations();
    protoPools->set_id(location.GetID());
    for (AgeBrackets::AgeBracket ageBracket : AgeBrackets::AgeBracketList)
    {
        auto protoPool = protoPools->add_pools();
        protoPool->set_type(types.at(ageBracket));
        WritePoolHealthStatus(&location, protoPool);
    }
}

void EpiProtoWriter::WritePoolHealthStatus(geopop::Location<Coordinate>*                        location,
                                           proto::EpiGeoGrid_History_PoolsForLocation_Pool*     protoPool)
{
    vector<unsigned long> statuses(NumOfHealthStatus(), 0);

    for (const auto &pool : location->CRefPools(ContactType::Id::Household)) {
        for (const auto &person : *pool) {
            statuses[ToSize(person->GetHealth().GetStatus())] ++;
        }
    }

    for (const auto& status : statuses)
    {
        protoPool->add_agebracketpercentage(static_cast<double>(status)/location->GetPopCount());
    }
}


} // namespace geopop
