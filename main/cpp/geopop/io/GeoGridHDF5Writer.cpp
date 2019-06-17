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

#include "GeoGridHDF5Writer.h"
#include "GeoGridHDF5Types.h"

#include "contact/ContactPool.h"
#include "geopop/GeoGrid.h"
#include "pop/Person.h"

#include <iostream>
#include <omp.h>
namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace H5;

GeoGridHDF5Writer::GeoGridHDF5Writer() : m_persons_found() {}

void GeoGridHDF5Writer::Write(GeoGrid& geoGrid, ostream& stream)
{
        // TODO: throw error, HDF5 doesn't support streams
}

void GeoGridHDF5Writer::Write(GeoGrid& geoGrid, const std::string& filename)
{
        m_location_counter = 0U;
        m_pool_counter = 0U;
        std::cout << "reached 5.0" << std::endl;
        H5File file(filename, H5F_ACC_TRUNC);
        std::cout << "reached 5.1" << std::endl;
        auto   locations = file.createGroup("Locations");
        std::cout << "reached 5.2" << std::endl;

        for (unsigned i = 0; i < geoGrid.size(); ++i) {
                std::cout << "reached 5.2.3" << std::endl;
                WriteLocation(file, geoGrid[i]);
                std::cout << "reached 5.2.6" << std::endl;
        }
        std::cout << "reached 5.3" << std::endl;
        WriteAttribute("size", geoGrid.size(), locations);
        std::cout << "reached 5.4" << std::endl;
        WritePeople(file);
        std::cout << "reached 5.5" << std::endl;
        file.close();
}
void GeoGridHDF5Writer::WriteContactPool(H5::Group& group, stride::ContactPool* contactPool)
{
        std::vector<unsigned int> people;
        for (auto person : *contactPool) {
                people.push_back(person->GetId());
                m_persons_found.insert(person);
        }
        auto label = "Pool" + to_string(++m_pool_counter);
        WriteDataset(label, people, group);

        auto dataset = group.openDataSet(label);
        WriteAttribute("id", contactPool->GetId(), dataset);
        WriteAttribute("size", contactPool->size(), dataset);
        auto type = ToString(contactPool->GetType());
        WriteAttribute("type", type, dataset);
}
void GeoGridHDF5Writer::WriteLocation(H5::H5File& file, std::shared_ptr<Location> location)
{
        std::cout << "reached 1" << std::endl;
        auto group = file.openGroup("Locations");
    std::cout << "reached 1" << std::endl;
        group      = group.createGroup("Location" + to_string(++m_location_counter));
    std::cout << "reached 1" << std::endl;

        WriteAttribute("id", location->GetID(), group);
    std::cout << "reached 1" << std::endl;
        WriteAttribute("name", location->GetName(), group);
    std::cout << "reached 1" << std::endl;
        WriteAttribute("province", location->GetProvince(), group);
    std::cout << "reached 1" << std::endl;
        WriteAttribute("population", location->GetPopCount(), group);
    std::cout << "reached 1" << std::endl;

        auto coordinate = location->GetCoordinate();
    std::cout << "reached 1" << std::endl;
        auto longitude  = boost::geometry::get<0>(coordinate);
    std::cout << "reached 1" << std::endl;
        auto latitude   = boost::geometry::get<1>(coordinate);
    std::cout << "reached 1" << std::endl;

        WriteAttribute("longitude", longitude, group);
    std::cout << "reached 1" << std::endl;
        WriteAttribute("latitude", latitude, group);
    std::cout << "reached 1" << std::endl;

        WriteCommutes(file, location, group);
    std::cout << "reached 1" << std::endl;
        auto contactPools = group.createGroup("ContactPools");
    std::cout << "reached 2" << std::endl;
        for (auto id : IdList) {
            std::cout << "reached 1" << std::endl;
                for (auto pool : location->RefPools(id)) {
                    std::cout << "reached 1" << std::endl;
                        WriteContactPool(contactPools, pool);
                    std::cout << "reached 1" << std::endl;
                }
        }
    std::cout << "reached 3" << std::endl;
        WriteAttribute("size", m_pool_counter, group);
        m_pool_counter = 0U;
}
void GeoGridHDF5Writer::WriteCommutes(H5::H5File& file, std::shared_ptr<Location> location, H5::Group& group)
{
        auto commutes = location->CRefOutgoingCommutes();

        std::vector<H5Commute> data;
        for (auto pair : commutes) {
                H5Commute commute{pair.first->GetID(), pair.second};
                data.push_back(commute);
        }
        WriteDataset("Commutes", data, group);
        auto dataset = group.openDataSet("Commutes");
        WriteAttribute("size", data.size(), dataset);
}

void GeoGridHDF5Writer::WritePeople(H5::H5File& file)
{
        WriteAttribute("size", m_persons_found.size(), file);
        std::vector<H5Person> people;
        for (auto person : m_persons_found) {
                people.push_back({person->GetId(), person->GetAge(), person->GetPoolId(Id::Daycare),
                                  person->GetPoolId(Id::PreSchool), person->GetPoolId(Id::Household),
                                  person->GetPoolId(Id::K12School), person->GetPoolId(Id::College),
                                  person->GetPoolId(Id::Workplace), person->GetPoolId(Id::PrimaryCommunity),
                                  person->GetPoolId(Id::SecondaryCommunity)});
        }
        WriteDataset("People", people, file);
}
template <typename T>
void GeoGridHDF5Writer::WriteAttribute(const std::string& name, const T& value, H5Object& object)
{
        auto type = GetH5Type(value);
        auto attr = object.createAttribute(name, type, DataSpace());
        attr.write(type, &value);
}
template <typename T>
void GeoGridHDF5Writer::WriteDataset(const std::string& name, std::vector<T> values, H5Object& object)
{
        hsize_t dimensions[1] = {values.size()};
        T       emptyValue;
        auto    type      = GetH5Type(emptyValue);
        auto    dataSpace = DataSpace(1, dimensions);
        auto    dataSet   = object.createDataSet(name, type, dataSpace);
        dataSet.write(values.data(), type);
}
} // namespace geopop
