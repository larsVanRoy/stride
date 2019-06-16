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

#include "geopop/io/EpiReader.h"
#include "geopop/EpiLocation.h"
#include "geopop/EpiGrid.h"
#include "util/Exception.h"

#include <nlohmann/json.hpp>

#include <boost/property_tree/ptree_fwd.hpp>
#include <memory>

namespace geopop {

/**
 * An implementation of the EpiReader using JSON.
 * This class is used to read a Epi from a JSON file.
 */
class EpiJSONReader : public EpiReader
{
public:
    /// Construct the EpiJSONReader with the istream which contains the JSON.
    explicit EpiJSONReader(std::unique_ptr<std::ifstream> inputStream) : EpiReader(std::move(inputStream))
    {
        m_grid = std::make_shared<EpiGrid>();
    };

    ~EpiJSONReader() = default;

    /// No copy constructor.
    EpiJSONReader(const EpiJSONReader&) = delete;

    /// No copy assignement.
    EpiJSONReader operator=(const EpiJSONReader&) = delete;

    /// Actually perform the read and return the Epi.
    void Read() override;

    void Print();
private:
    /// Create a Coordinate based on the information stored in the provided JSON object.
    Coordinate ParseCoordinate(const nlohmann::json& coordinate);

    /// Create EpiGrid and fills it with EpiLocations based on the information stored in the provided JSON object.
    void ParseLocations(const nlohmann::json& location);

    /// Create an EpiLocation based on the information stored in the provided JSON object.
    std::shared_ptr<EpiLocation<Coordinate>> ParseLocation(const nlohmann::json& location);


    void ParseHistory(const nlohmann::json& history);

    void ParseHistoryLocation(const nlohmann::json &location);

    /// Adds HealthPool to EpiLocation based on the information stored in the provided JSON object.
    void ParseLocationPools(const nlohmann::json& pools, std::shared_ptr<EpiLocation<Coordinate>> loc);

    /// Adds to HealthPool based on the information stored in the provided JSON object.
    std::shared_ptr<stride::HealthPool> ParsePool(const nlohmann::json& pool);

    /// Take a JSON object and cast wrongly provided types to the expected type (if possible).
    template <typename T>
    T JSONCast(const nlohmann::json& json_object);

private:
    std::shared_ptr<EpiGrid> m_grid;
};

} // namespace geopop
