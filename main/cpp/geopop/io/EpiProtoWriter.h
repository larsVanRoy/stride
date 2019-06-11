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

#pragma once

#include "EpiWriter.h"
#include "proto_pb/epigeogrid.pb.h"
#include "geopop/GeoGrid.h"
#include "geopop/geo/Coordinate.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <set>
#include <string>

namespace stride {
    class ContactPool;
    class Person;
} // namespace stride

namespace geopop {

/**
 * Writes the epidemiological status to a ProtoBuf file.
 */
class EpiProtoWriter : public EpiWriter
{
public:
    /// Construct the EpiProtoriter.
    explicit EpiProtoWriter(const std::string& filename);

    /// Convert the provided GeoGrid to an the epidemiological status and
    /// write the status to the proved ostream in Protobuf format.
    void Write(const geopop::GeoGrid& geoGrid, unsigned timeStep) override;

private:
    /// Generate file name and open the file stream.
    void Initialize(const geopop::GeoGrid& geoPopGrid) override;

    void Finalize() override;

    /// Create a ProtoBuf Coordinate containing all the info needed to reconstruct a Coordinate..
    void WriteCoordinates(const Coordinate& coordinate, proto::EpiGeoGrid_Location_Coordinates* protoCoordinates);

    /// Create a ProtoBuf Location containing all the info needed to reconstruct a Location.
    void WriteLocation(Location& location, proto::EpiGeoGrid_Location* protoLocation);

    /// Create a ProtoBuf history containing all the info about the health status
    /// in the different pools for a given location
    void WriteHealthStatus(Location& location, proto::EpiGeoGrid_History* protoHistory);

    /// Create a protoBuf pool status containing all the info about the health status
    /// within a given pool
    void WritePoolHealthStatus(Location*                                            location,
                               stride::ContactType::Id                              id,
                               proto::EpiGeoGrid_History_PoolsForLocation_Pool*     protoPool);

private:
    std::ofstream m_stream;            ///< output stream

    proto::EpiGeoGrid m_protogrid;     ///< protoBuf object
};

} // namespace geopop
