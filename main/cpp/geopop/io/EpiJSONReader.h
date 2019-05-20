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

#include "EpiJSONReader.h"
#include "geopop/EpiLocation.h"

#include <nlohmann/json.hpp>

#include <boost/property_tree/ptree_fwd.hpp>

namespace geopop {


/**
 * An implementation of the EpiReader using JSON.
 * This class is used to read a Epi from a JSON file.
 */
class EpiJSONReader : public EpiReader
{
public:
    /// Construct the EpiJSONReader with the istream which contains the JSON.
    EpiJSONReader(std::unique_ptr<std::istream> inputStream, stride::Population* pop);

    /// No copy constructor.
    EpiJSONReader(const EpiJSONReader&) = delete;

    /// No copy assignement.
    EpiJSONReader operator=(const EpiJSONReader&) = delete;

    /// Actually perform the read and return the Epi.
    void Read() override;

private:
    /// Create a Coordinate based on the information stored in the provided JSON object.
    Coordinate ParseCoordinate(nlohmann::json& coordinate);

    /// Create an EpiLocation based on the information stored in the provided JSON object.
    std::shared_ptr<Location> ParseLocation(nlohmann::json& location);


    /// Take a JSON object and cast wrongly provided types to the expected type (if possible).
    template <typename T>
    T JSONCast(nlohmann::json& json_object);
};

} // namespace geopop
