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

#include "GeoGridHDF5Reader.h"
#include "GeoGridHDF5Types.h"

#include "geopop/GeoGrid.h"
#include "pop/Population.h"
#include "util/Exception.h"

#include <memory>
namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;
using namespace H5;

GeoGridHDF5Reader::GeoGridHDF5Reader(const std::string& filename, Population* pop)
    : GeoGridReader(filename, pop), m_filename(filename)
{
}

void GeoGridHDF5Reader::Read()
{
        H5File file;
        try {
                file = H5File(m_filename, H5F_ACC_RDONLY);

        } catch (H5::Exception&) {
                throw stride::util::Exception("Problem parsing HDF5 file, check whether empty.");
        }
        auto& geoGrid = m_population->RefGeoGrid();

        ReadPeople(file);

        ReadLocations(file, geoGrid);

        AddCommutes(geoGrid);
        m_commutes.clear();
        m_people.clear();
        file.close();
}
void GeoGridHDF5Reader::ReadPeople(const H5::H5File& file)
{
        size_t nrPeople;

        ReadAttribute("size", &nrPeople, file);

        if (nrPeople == 0)
                return;

        std::vector<H5Person> people(nrPeople);
        auto                  person_t = GetPersonType();
        ReadDataset("People", people.data(), file);

        for (auto person : people) {
                auto stride_person  = m_population->CreatePerson(person.id, person.age, person.houseHold, 0, 0,
                                                                person.k12School, person.college, person.workplace,
                                                                person.primaryCommunity, person.secondaryCommunity);
                m_people[person.id] = stride_person;
        }
}
void GeoGridHDF5Reader::ReadLocations(const H5::H5File& file, GeoGrid& geoGrid)
{
        auto   locations_group = file.openGroup("Locations");
        size_t locations_size;
        ReadAttribute("size", &locations_size, locations_group);
        if (locations_size == 0)
                return;
        for (auto i = 0; i < locations_size; ++i) {
                auto group = locations_group.openGroup("Location" + to_string(i + 1));
                ReadLocation(group, geoGrid);
        }
}

void GeoGridHDF5Reader::ReadLocation(const Group& object, GeoGrid& geoGrid)
{
        unsigned int id, province, population, size;
        size_t       commutes_size;
        double       longitude, latitude;
        string       name;

        ReadAttribute("id", &id, object);
        ReadAttribute("province", &province, object);
        ReadAttribute("population", &population, object);
        ReadAttribute("size", &size, object);
        ReadAttribute("name", &name, object);
        ReadAttribute("longitude", &longitude, object);
        ReadAttribute("latitude", &latitude, object);

        auto location_ptr = make_shared<Location>(id, province, Coordinate{longitude, latitude}, name, population);

        for (auto i = 0; i < size; ++i) {
                auto pool_group = object.openGroup("ContactPools").openDataSet("Pool" + to_string(i + 1));
                ReadContactPool(pool_group, location_ptr);
        }

        auto commutes_set = object.openDataSet("Commutes");
        ReadAttribute("size", &commutes_size, commutes_set);
        std::vector<H5Commute> commutes(commutes_size);
        auto                   commute_t = GetCommuteType();

        ReadDataset("Commutes", commutes.data(), object);

        for (auto commute : commutes) {
                m_commutes.emplace_back(id, commute.to, commute.proportion);
        }

        geoGrid.AddLocation(move(location_ptr));
}

void GeoGridHDF5Reader::ReadContactPool(const DataSet& object, std::shared_ptr<Location> location_ptr)
{
        unsigned int size;
        string       type;

        ReadAttribute("size", &size, object);
        ReadAttribute("type", &type, object);

        auto type_id = ToId(type);

        vector<unsigned int> people(size);
        object.read(people.data(), PredType::NATIVE_UINT);

        auto contact_pool_ptr = m_population->RefPoolSys().CreateContactPool(type_id);
        location_ptr->RefPools(type_id).emplace_back(contact_pool_ptr);

        for (const auto& p : people) {
                auto person = m_people[p];
                contact_pool_ptr->AddMember(person);
                person->SetPoolId(type_id, contact_pool_ptr->GetId());
        }
}

template <typename T>
void GeoGridHDF5Reader::ReadAttribute(const std::string& name, T* data, const H5::H5Object& object)
{
        auto attr = object.openAttribute(name);
        switch (attr.getTypeClass()) {
        default: attr.read(attr.getDataType(), data); break;
        case H5T_COMPOUND: attr.read(attr.getCompType(), data); break;
        }
}
template <>
void GeoGridHDF5Reader::ReadAttribute(const std::string& name, std::string* data, const H5::H5Object& object)
{
        auto attr = object.openAttribute(name);
        attr.read(attr.getStrType(), *data);
}
template <typename T>
void GeoGridHDF5Reader::ReadDataset(const std::string& name, T* data, const H5::H5Object& object)
{
        auto dataSet = object.openDataSet(name);
        switch (dataSet.getTypeClass()) {
        default: dataSet.read(data, dataSet.getDataType()); break;
        case H5T_STRING: dataSet.read(data, dataSet.getStrType()); break;
        case H5T_COMPOUND: dataSet.read(data, dataSet.getCompType()); break;
        }
}

} // namespace geopop
