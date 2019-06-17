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

#include "geopop/io/EpiJSONReader.h"
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include <string>

namespace geopop {
using nlohmann::json;

/// Actually perform the read and return the Epi.
std::shared_ptr<EpiGrid> EpiJSONReader::Read() {

    json json_file;

    try {
        json_file = json::parse(*m_stream);

    } catch (json::parse_error&) {
        throw stride::util::Exception("Problem parsing JSON file, check whether empty or invalid JSON.");
    }
    try {
        auto location = json_file["locations"];
        ParseLocations(location);

        auto history = json_file["history"];
        ParseHistory(history);
    }
    catch(std::exception& e){
        std::cout << e.what() << std::endl;
        throw stride::util::Exception("Problem parsing JSON file, check whether empty or invalid JSON. 2");
    }
    m_stream->close();

    m_grid->Finalize();

    return m_grid;
}

template <typename T>
T EpiJSONReader::JSONCast(const json& json_object)
{
    // If the JSON object is given in a type that was not expected cast it to the properly type.
    if (json_object.is_string()){
        return boost::lexical_cast<T>(json_object.get<std::string>());
    } else {
        return json_object.get<T>();
    }
}

Coordinate EpiJSONReader::ParseCoordinate(const nlohmann::json &coordinate) {
    double latitude = coordinate["lat"];
    double longitude = coordinate["long"];

    return Coordinate(longitude, latitude);
}

void EpiJSONReader::ParseLocations(const nlohmann::json &location_array) {

    for(unsigned int i = 0; i < location_array.size(); ++i){
        auto t = ParseLocation(location_array[i]);
        m_grid->AddLocation(t);
    }
}

std::shared_ptr<EpiLocation> EpiJSONReader::ParseLocation(const nlohmann::json& location) {

    unsigned int id         =   location["id"];
    unsigned int province   =   location["province"];
    Coordinate coordinate   =   ParseCoordinate(location["coordinates"]);
    std::string name        =   location["name"];
    unsigned int population =   location["population"];

    std::shared_ptr<EpiLocation> result = std::make_shared<EpiLocation>(id, province, coordinate, name, population);

    return result;
}

void EpiJSONReader::ParseHistory(const nlohmann::json &history) {
    for (unsigned int i = 0; i < history.size(); ++i) {
        nlohmann::json locations;
        try {
            const nlohmann::json &step = history[i];
            locations = step["locations"];
            const nlohmann::json &day = step["timestep"];
        }
        catch(std::exception& e){
            std::cout << "Error: ParseHistory" << e.what() << std::endl;
            throw e;
        }
        ParseHistoryLocation(locations);
    }
}

void EpiJSONReader::ParseLocationPools(const nlohmann::json &pools, std::shared_ptr<EpiLocation> loc) {
    std::shared_ptr<stride::PoolStatus> status = std::make_shared<stride::PoolStatus>();
    try {
        for (stride::ContactType::Id i : stride::ContactType::IdList) {
            std::shared_ptr<stride::HealthPool> h = ParsePool(pools[stride::ContactType::ToString(i)]);
            status->addStatus(i, h);
        }
    }
    catch(std::exception& e){
        std::cout << "Error in ParseLocationPools: " << e.what() << std::endl;
        throw e;
    }

    loc->AddPoolStatus(status);
}

void EpiJSONReader::ParseHistoryLocation(const nlohmann::json &location) {

    try {
        for(unsigned int i = 0; i < location.size(); ++i){
            const auto& j = location[i];
            unsigned int locationID = j["id"];
            std::shared_ptr<EpiLocation> loc = m_grid->GetById(locationID);
            ParseLocationPools(j["pools"], loc);
        }
    }
    catch(std::exception& e){
        std::cout << "Error in ParseHistoryLocation: " << e.what() << std::endl;
        throw e;
    }
}

std::shared_ptr<stride::HealthPool> EpiJSONReader::ParsePool(const nlohmann::json &pool) {
    std::shared_ptr<stride::HealthPool> result = std::make_shared<stride::HealthPool>();
    try {
        for (unsigned short i = 0; i < pool.size(); ++i) {
            double fraction = pool[i];
            result->setHealth(static_cast<stride::HealthStatus>(i), fraction);

        }
    }
    catch(std::exception& e){
        std::cout << "Error in ParsePool: " << e.what() << std::endl;
        throw e;
    }
    return result;
}

void EpiJSONReader::Print() {
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

}