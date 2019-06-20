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
#include "geopop/GeoGrid.h"
#include "geopop/geo/Coordinate.h"
#include <nlohmann/json.hpp>

#include <ostream>
#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <set>
#include <string>

namespace stride {
    class ContactPool;
    class Person;
//namespace AgeBrackets {
//    enum class AgeBracket : unsigned short;
//}
} // namespace stride

namespace geopop {

/**
 * Writes the epidemiological status to a JSON file.
 */
class EpiJSONWriter : public EpiWriter
{
public:
    /// Construct the EpiJSONWriter.
    EpiJSONWriter(const std::string& filename) : EpiWriter(), m_json(), m_filename(filename)
    {
    };

    ~EpiJSONWriter() = default;

    /// Convert the provided GeoGrid to an the epidemiological status and write the status to the proved ostream in JSON format.
    void Write(const geopop::GeoGrid& geoGrid, unsigned timeStep) override;

//    /// Convert the provided GeoGrid to an the epidemiological status and write the status to the proved file in JSON format.
//    void Write(const geopop::GeoGrid& geoGrid, unsigned timeStep);
    /// Generate file name and open the file stream.
    void Initialize(const geopop::GeoGrid& geoPopGrid) override;

    void Finalize() override;

private:
    /// Create json object for coordinate
    nlohmann::json WriteCoordinate(const geopop::Coordinate& coordinate);

    /// Create json object for location
    nlohmann::json WriteLocation(const std::shared_ptr<geopop::Location<Coordinate>> location);

    /// Create json object for every location with id, coordinates and name
    nlohmann::json WriteLocations(const geopop::GeoGrid &geoGrid);

    /// Create json object for percentage of the population per health category for the proved location
    nlohmann::json WriteHealthStatus(const std::shared_ptr<geopop::Location<Coordinate>> location);

//    nlohmann::json WriteHealthStatusBracket(const std::shared_ptr<geopop::Location<Coordinate>> location,
//            stride::AgeBrackets::AgeBracket bracket);

private:
    nlohmann::json m_json;      ///< object to store data to write

    std::string m_filename;     ///< name of the file to write to
};

} // namespace geopop
