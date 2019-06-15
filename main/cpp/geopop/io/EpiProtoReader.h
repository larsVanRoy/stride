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

#include "EpiReader.h"
#include "proto_pb/epigeogrid.pb.h"
#include "geopop/EpiLocation.h"
#include "geopop/EpiGrid.h"

#include <memory>
#include <set>

namespace geopop {

/**
 * An implementation of the EpiGridReader using Protocol Buffers.
 * This class is used to read a EpiGrid from a Proto file.
 */

class EpiProtoReader : public EpiReader
{
public:
    /// Construt the EpiProtoReader with the ifstream which contains the protobuf info.
    EpiProtoReader(std::unique_ptr<std::ifstream> inputStream) : EpiReader(std::move(inputStream)), m_grid(), m_protogrid()
    {
        m_grid = std::make_shared<EpiGrid>();
    }

    /// Default destructor
    ~EpiProtoReader() override = default;

    /// No copy constructor.
    EpiProtoReader(const EpiProtoReader&) = delete;

    /// Perform the read and return the Epi Grid
    std::shared_ptr<EpiGrid> Read() override;

    void Print();

private:
    /// Create EpiGrid and fill it with EpiLocations based on the information stored in the Protobuf object
    void ParseLocations();

    /// Create a Coordinate based on the information stored in the Protobuf object
    Coordinate ParseCoordinate(const proto::EpiGeoGrid_Location_Coordinates& coordinates);

    /// Create an EpiLocation based on the information stored in the Protobuf object.
    std::shared_ptr<EpiLocation> ParseLocation(const proto::EpiGeoGrid_Location& location);

    /// Update the EpiGrid and add the correct information related to the history.
    void ParseHistory();

    /// Update the information in the EpiGrid related to the history of a location.
    void ParseHistoryLocation(const proto::EpiGeoGrid_History_PoolsForLocation& loc);

    /// Update the information in the EpiGrid related to the history of a pool of a location.
    std::shared_ptr<stride::HealthPool> ParseLocationPools(const proto::EpiGeoGrid_History_PoolsForLocation_Pool& pool);

private:
    std::shared_ptr<EpiGrid> m_grid;    ///< final EpiGrid object
    proto::EpiGeoGrid m_protogrid;      ///< protoBuf object
};


} // namespace geopop
