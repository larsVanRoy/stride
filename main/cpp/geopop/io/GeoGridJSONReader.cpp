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

#include "GeoGridJSONReader.h"

#include "geopop/GeoGrid.h"
#include "pop/Population.h"
#include "util/Exception.h"

#include <memory>
#include <boost/lexical_cast.hpp>

using json = nlohmann::json;

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

GeoGridJSONReader::GeoGridJSONReader(unique_ptr<istream> inputStream, Population* pop)
    : GeoGridReader(move(inputStream), pop)
{
}

void GeoGridJSONReader::Read()
{
        json json_file;

        try {
                json_file = json::parse(*m_inputStream);

        } catch (json::parse_error&) {
                throw Exception("Problem parsing JSON file, check whether empty or invalid JSON.");
        }

        auto& geoGrid = m_population->RefGeoGrid();
        auto people = json_file["persons"];

        for (auto it = people.begin(); it != people.end(); it++) {
                auto person = ParsePerson(*it);
                m_people[person->GetId()] = person;
        }
        auto locations = json_file["locations"];

        for (auto it = locations.begin(); it != locations.end(); it++) {
                shared_ptr<Location> loc;
                loc = ParseLocation(*it);
                geoGrid.AddLocation(move(loc));
        }

        AddCommutes(geoGrid);
        m_commutes.clear();
        m_people.clear();

}

template <typename T>
T GeoGridJSONReader::JSONCast(json& json_object)
{
        // If the JSON object is given in a type that was not expected cast it to the properly type.
        if (json_object.is_string()){
                return boost::lexical_cast<T>(json_object.get<string>());
        } else {
                return json_object.get<T>();
        }
}

template <>
string GeoGridJSONReader::JSONCast(json& json_object)
{
        // TEMPLATE SPECIFICATION
        // If the expected type was a string then no casting is needed and the template can be assigned.
        return json_object.get<string>();
}

shared_ptr<Location> GeoGridJSONReader::ParseLocation(json& location)
{
        const auto id         = JSONCast<unsigned int>(location["id"]);
        const auto name       = JSONCast<string>(location["name"]);
        const auto province   = JSONCast<unsigned int>(location["province"]);
        const auto population = JSONCast<unsigned int>(location["population"]);

        const auto coordinate = ParseCoordinate(location["coordinate"]);

        auto result         = make_shared<Location>(id, province, coordinate, name, population);
        auto contactPools = location["contactPools"];
        for (auto it = contactPools.begin(); it != contactPools.end(); it++) {
               ParseContactPools(result, *it);
        }

        if (location.find("commutes") != location.end()) {
                json commutes = location["commutes"];
                for (auto json_obj : commutes) {
                    for(json::iterator it2 = json_obj.begin(); it2!=json_obj.end(); ++it2){
                        const auto to = it2.key();
                        m_commutes.emplace_back(id, std::stoul(to, nullptr,0), JSONCast<double>(json_obj[to]));
                    }
                }
        }

        return result;
}

Coordinate GeoGridJSONReader::ParseCoordinate(json& coordinate)
{

        const auto longitude = JSONCast<double>(coordinate["longitude"]);
        const auto latitude  = JSONCast<double>(coordinate["latitude"]);

        return {longitude, latitude};
}

void GeoGridJSONReader::ParseContactPools(std::shared_ptr<geopop::Location> loc, nlohmann::json &contactCenter)
{
        const auto type = JSONCast<string>(contactCenter["class"]);
        ContactType::Id typeId;

        if (type == ToString(Id::K12School)) {
                typeId = Id::K12School;
        } else if (type == ToString(Id::College)) {
                typeId = Id::College;
        } else if (type == ToString(Id::Household)) {
                typeId = Id::Household;
        } else if (type == ToString(Id::PrimaryCommunity)) {
                typeId = Id::PrimaryCommunity;
        } else if (type == ToString(Id::SecondaryCommunity)) {
                typeId = Id::SecondaryCommunity;
        } else if (type == ToString(Id::Workplace)) {
                typeId = Id::Workplace;
        } else {
                throw Exception("No such ContactPool type: " + type);
        }

        auto contactPools = contactCenter["pools"];

        for(auto it = contactPools.begin(); it != contactPools.end(); it++){
                ParseContactPool(loc, *it, typeId);
        }
}

void GeoGridJSONReader::ParseContactPool(std::shared_ptr<Location> loc, json& contactPool, ContactType::Id typeId)
{
        // Don't use the id of the ContactPool but the let the Population create an id.
        auto result = m_population->RefPoolSys().CreateContactPool(typeId);
        loc->RefPools(typeId).emplace_back(result);
        auto people = contactPool["people"];

        for (auto it = people.begin(); it != people.end(); it++) {
                auto person_id = JSONCast<unsigned int>(*it);
                if(m_people[person_id]){
                        result->AddMember(m_people[person_id]);
                        m_people[person_id]->SetPoolId(typeId, result->GetId());
                } else {
                    throw Exception("Person does not exist!");
                }
        }
}

Person* GeoGridJSONReader::ParsePerson(json& person)
{

        const auto id = JSONCast<unsigned int>(person["id"]);
        const auto age = JSONCast<unsigned int>(person["age"]);
        const auto hhId = JSONCast<unsigned int>(person["household"]);
        const auto ksId = JSONCast<unsigned int>(person["k12School"]);
        const auto coId = JSONCast<unsigned int>(person["college"]);
        const auto wpId = JSONCast<unsigned int>(person["workplace"]);
        const auto pcId = JSONCast<unsigned int>(person["primaryCommunity"]);
        const auto scId = JSONCast<unsigned int>(person["secondaryCommunity"]);

        return m_population->CreatePerson(id, age, hhId, ksId, coId, wpId, pcId, scId);
}

} // namespace geopop
