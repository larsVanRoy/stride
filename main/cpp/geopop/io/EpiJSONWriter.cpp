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

#include "EpiJSONWriter.h"

#include "contact/ContactPool.h"
#include "contact/AgeBrackets.h"
#include "geopop/Location.h"
#include "geopop/GeoGrid.h"
#include "geopop/geo/Coordinate.h"
#include "pop/Person.h"
#include "disease/Health.h"

#include <string>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <fstream>
#include <memory>
#include <omp.h>

using json = nlohmann::json;

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace boost::property_tree;


void EpiJSONWriter::Initialize(const geopop::GeoGrid& geoGrid) {
    m_json = json::object();
    m_json["locations"]     = WriteLocations(geoGrid);
    m_json["history"]       = json::array();
}

void EpiJSONWriter::Finalize() {
    ofstream stream;
    stream.open(m_filename);
    stream << setw(4) << m_json << endl;
    stream.close();
}

void EpiJSONWriter::Finalize(std::ostream& stream) {
    stream << setw(4) << m_json << endl;
}

void EpiJSONWriter::Write(const geopop::GeoGrid& geoGrid, unsigned timestep) {
    json locations_array = json::array();

    for (unsigned i = 0; i < geoGrid.size(); ++i) {
        const auto& location = geoGrid[i];
        json location_json = json::object();
        location_json = WriteLocation(location);
        locations_array.push_back(location_json);
    }

    json step = json::object();
    step["timestep"] = timestep;
    step["locations"] = locations_array;
    m_json["history"].push_back(step);
}

void EpiJSONWriter::Write(const geopop::GeoGrid& geoGrid, std::ostream& stream, unsigned timestep) {
    json locations_array = json::array();

    for (unsigned i = 0; i < geoGrid.size(); ++i) {
        const auto& location = geoGrid[i];
        json location_json = json::object();
        location_json = WriteLocation(location);
        locations_array.push_back(location_json);
    }

    json step = json::object();
    step["timestep"] = timestep;
    step["locations"] = locations_array;
    m_json["history"].push_back(step);

    stream << setw(4) << m_json << std::endl;
    stream.flush();
}

json EpiJSONWriter::WriteLocations(const geopop::GeoGrid &geoGrid)
{
    json location_array = json::array();

    for(unsigned i = 0; i < geoGrid.size(); ++i){
        const auto& location = geoGrid[i];
        json location_object = json::object();
        location_object["id"]           = location->GetID();
        location_object["coordinates"]  = WriteCoordinate(location->GetCoordinate());
        location_object["population"]   = location->GetPopCount();
        location_object["name"]         = location->GetName();
        location_array.push_back(location_object);
    }

    return location_array;
}

json EpiJSONWriter::WriteCoordinate(const geopop::Coordinate& coordinate)
{
    json coordinate_object = json::object();

    coordinate_object["long"] = boost::geometry::get<0>(coordinate);
    coordinate_object["lat"] = boost::geometry::get<1>(coordinate);

    return coordinate_object;
}

json EpiJSONWriter::WriteLocation(const std::shared_ptr<geopop::Location<Coordinate>> location)
{
    json location_object = json::object();

    location_object["id"] = location->GetID();
    location_object["agebrackets"] = WriteHealthStatus(location);

    return location_object;
}

json EpiJSONWriter::WriteHealthStatus(const std::shared_ptr<geopop::Location<Coordinate>> location) {
    json health_status = json::object();

    std::map<AgeBrackets::AgeBracket, std::map<string, double>> status;

    for(const AgeBrackets::AgeBracket& bracket : AgeBrackets::AgeBracketList) {
        std::map<string, double> health;
        for(const HealthStatus& h : stride::HealthStatusList){
            health[stride::HealthToString(h)] = 0;
        }
        status[bracket] = health;
    }

    for(const auto &pool : location->CRefPools(stride::ContactType::Id::Household)) {
        for (const Person* person : *pool) {
            status[AgeBrackets::ToAgeBracket(person->GetAge())][stride::HealthToString(person->GetHealth().GetStatus())]++;
        }
    }

    for(const AgeBrackets::AgeBracket& bracket : AgeBrackets::AgeBracketList) {
        json Jhealth = json::object();
        for(const HealthStatus& h : stride::HealthStatusList) {
            if(location->GetPopCount() == 0){
                Jhealth[stride::HealthToString(h)] = 0;
            }
            else {
                Jhealth[stride::HealthToString(h)] =
                        (double) status[bracket][stride::HealthToString(h)] / location->GetPopCount();
            }
        }
        health_status[AgeBrackets::AgeBracketToString(bracket)] =Jhealth;
    }

    return health_status;
}
} // namespace geopop
