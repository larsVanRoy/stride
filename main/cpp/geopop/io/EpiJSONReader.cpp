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
void EpiJSONReader::Read() {

    json json_file;

    try {
        json_file = json::parse(*m_stream);

    } catch (json::parse_error&) {
        throw stride::util::Exception("Problem parsing JSON file, check whether empty or invalid JSON.");
    }
//    std::cout << std::setw(4) << json_file << "\n";
    try {
        auto location = json_file["locations"];
        ParseLocations(location);
    }
    catch(...){
        throw stride::util::Exception("Problem parsing JSON file, check whether empty or invalid JSON. 2");
    }
    m_stream->close();
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
    return Coordinate(0.0,0.0);
}

void EpiJSONReader::ParseLocations(const nlohmann::json &location_array) {

    try {
        std::cout << location_array.size() << std::endl;
        std::cout << std::setw(4) << location_array[80] << std::endl;
        std::cout << "first:\n";
        m_grid->AddLocation(std::make_shared<EpiLocation>(0,1,Coordinate(0,0),"test", 10000));
        std::cout << "second:\n";
        m_grid->AddLocation(std::make_shared<EpiLocation>(1,1,Coordinate(0,1),"test2", 10000));
        for(unsigned int i = 0; i < location_array.size(); ++i){
            std::cout << "Parse Location: " << i << std::endl;
            std::shared_ptr<EpiLocation> t = ParseLoc(location_array[i]);
            std::cout << "\tname:   " << t->GetName() << std::endl;
            std::cout << "\tprov:   " << t->GetProvince() << std::endl;
            std::cout << "\tpopcount:   " << t->GetPopCount() << std::endl;
//            m_grid->AddLocation(std::make_shared<EpiLocation>(t->GetID(),t->GetProvince(),t->GetCoordinate(),t->GetName(), t->GetPopCount()));
        }
    }
    catch(...)
    {
        printf("Woops");
    }
}

std::shared_ptr<EpiLocation> EpiJSONReader::ParseLoc(const nlohmann::json& location) {


    unsigned int id         =   location["id"];
    unsigned int province   =   location["province"];
    Coordinate coordinate   =   ParseCoordinate(location["coordinates"]);
    std::string name        =   location["name"];
    unsigned int population =   location["population"];
    std::shared_ptr<EpiLocation> result = std::make_shared<EpiLocation>(id, province, coordinate, name, population);

    return result;
}

std::shared_ptr<EpiLocation> EpiJSONReader::ParseLocation(const nlohmann::json &location) {

}
Coordinate ParseCoordinate(const nlohmann::json& coordinate);

std::shared_ptr<EpiLocation> EpiJSONReader::ParsePools(const nlohmann::json &location) {

}
std::shared_ptr<EpiLocation> EpiJSONReader::ParsePool(const nlohmann::json &location) {

}

}