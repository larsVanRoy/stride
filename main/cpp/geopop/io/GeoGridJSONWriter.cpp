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

#include "GeoGridJSONWriter.h"

#include "contact/ContactPool.h"
#include "geopop/GeoGrid.h"
#include "pop/Person.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <omp.h>

using json = nlohmann::json;

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;

GeoGridJSONWriter::GeoGridJSONWriter() : m_persons_found() {}

void GeoGridJSONWriter::Write(GeoGrid& geoGrid, ostream& stream)
{
        json jsonfile = json::object();

        json locations_array = json::array();

        for (unsigned i = 0; i < geoGrid.size(); ++i) {
                json location_json = json::object();
                location_json = WriteLocation(*geoGrid[i]);
                locations_array.push_back(location_json);
        }

        jsonfile["locations"] = locations_array;

        json persons_array = json::array();

        for (const auto& person : m_persons_found) {
                json person_json = json::object();
                person_json = WritePerson(person);
                persons_array.push_back(person_json);
        }

        jsonfile["persons"] = persons_array;

        m_persons_found.clear();
        stream << setw(4) << jsonfile << std::endl;
        stream.flush();
}

void GeoGridJSONWriter::Write(GeoGrid &geoGrid, const std::string &filename)
{
        std::ofstream stream(filename);
        Write(geoGrid, stream);
        stream.close();
}

json GeoGridJSONWriter::WriteContactPools(stride::ContactType::Id typeId,
                                          const stride::util::SegmentedVector<stride::ContactPool *>& pools)
{
        json contactPool_json = json::object();
        contactPool_json["class"] = ToString(typeId);
        contactPool_json["pools"] = json::array();

        for (const auto& pool : pools) {
                contactPool_json["pools"].push_back(WriteContactPool(pool));
        }

        return contactPool_json;
}

json GeoGridJSONWriter::WriteContactPool(stride::ContactPool* contactPool)
{
        json pool = json::object();
        pool["id"] = contactPool->GetId();
        pool["people"] = json::array();

        for (auto person : contactPool->GetPool()) {
                m_persons_found.insert(person);
                pool["people"].push_back(person->GetId());
        }

        return pool;
}

json GeoGridJSONWriter::WriteCoordinate(const Coordinate& coordinate)
{
        json coordinate_object = json::object();

        coordinate_object["longitude"] = coordinate.get<0>();
        coordinate_object["latitude"] = coordinate.get<1>();

        return coordinate_object;
}

json GeoGridJSONWriter::WriteLocation(const Location<Coordinate>& location)
{
        json location_object = json::object();

        location_object["id"] = location.GetID();
        location_object["name"] = location.GetName();
        location_object["province"] = location.GetProvince();
        location_object["population"] = location.GetPopCount();
        location_object["coordinate"] = WriteCoordinate(location.GetCoordinate());

        location_object["contactPools"] = json::array();

        auto commutes = location.CRefOutgoingCommutes();
        if (!commutes.empty()) {
            json commutes_list = json::array();
            for (auto commute_pair : commutes) {
                json com_obj = json::object();
                const auto temp_to = std::to_string(commute_pair.first->GetID());
                com_obj[temp_to] = commute_pair.second;
                commutes_list.push_back(com_obj);
            }
            location_object["commutes"] = commutes_list;
        }
        json contactPools_array = json::array();

        for (Id typ : IdList) {
            json temp_obj = WriteContactPools(typ, location.CRefPools(typ));
            if (temp_obj["pools"].size() != 0) {
                location_object["contactPools"].push_back(temp_obj);
            }
        }

        return location_object;
}

json GeoGridJSONWriter::WritePerson(stride::Person* person) {
        using namespace ContactType;

        json person_json = json::object();

        person_json["id"] = person->GetId();
        person_json["age"] = (unsigned int)person->GetAge();
        person_json["k12School"] = person->GetPoolId(Id::K12School);
        person_json["college"] = person->GetPoolId(Id::College);
        person_json["household"] = person->GetPoolId(Id::Household);
        person_json["workplace"] = person->GetPoolId(Id::Workplace);
        person_json["primaryCommunity"] = person->GetPoolId(Id::PrimaryCommunity);
        person_json["secondaryCommunity"] = person->GetPoolId(Id::SecondaryCommunity);
        person_json["daycare"] = person->GetPoolId(Id::Daycare);
        person_json["preSchool"] = person->GetPoolId(Id::PreSchool);

        return person_json;
}
} // namespace geopop
