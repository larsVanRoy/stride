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

#include "EpiHDF5Writer.h"
#include "EpiGridHDF5Types.h"
#include "GeoGridHDF5Types.h"
#include "contact/ContactPool.h"
#include "geopop/Location.h"
#include "geopop/GeoGrid.h"
#include "geopop/geo/Coordinate.h"
#include "pop/Person.h"
#include "disease/Health.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <omp.h>
#include <string>

namespace geopop {
using namespace H5;
using namespace std;
using namespace stride;
using namespace stride::ContactType;

void EpiHDF5Writer::Write(const geopop::GeoGrid& geoGrid, unsigned timeStep)
{
        auto group = this->m_file.openGroup("Steps").createGroup("Step" + to_string(timeStep));
        this->m_timesteps.push_back(timeStep);
        for (size_t i = 0; i < geoGrid.size(); ++i) {
                this->WriteLocation(group, geoGrid[i]);
        }
        WriteAttribute("nrLocations", m_location_counter, group);
        this->m_location_counter = 0;
}
void EpiHDF5Writer::Initialize(const geopop::GeoGrid& geoPopGrid)
{
        this->m_file = H5File{m_filename, H5F_ACC_TRUNC};
        this->WriteLocations(geoPopGrid);
        this->m_file.createGroup("Steps");
}
void EpiHDF5Writer::Finalize() {
        WriteDataset("Timesteps", m_timesteps, m_file);
        WriteAttribute("nrTimesteps", m_timesteps.size(), m_file);
        this->m_file.close();
}

void EpiHDF5Writer::WriteLocation(H5::Group& locations_group, const std::shared_ptr<Location<Coordinate>> location)
{
        auto group = locations_group.createGroup("Location" + to_string(m_location_counter));
        m_location_counter++;
        WriteAttribute("id", location->GetID(), group);
        WriteHealthStatus(group, location);
}

void EpiHDF5Writer::WriteLocations(const geopop::GeoGrid& geoGrid)
{
        std::vector<H5GeoLocation> locations;
        using namespace boost::geometry;
        for (size_t i = 0; i < geoGrid.size(); ++i) {
                auto loc = geoGrid[i];
                locations.push_back(H5GeoLocation{loc->GetID(), loc->GetPopCount(), {}, loc->GetProvince(),
                                       get<0>(loc->GetCoordinate()), get<1>(loc->GetCoordinate())});
                strcpy(locations.back().name, loc->GetName().c_str());
        }
        this->WriteDataset("Locations", locations, m_file);
        auto dataset = m_file.openDataSet("Locations");
        this->WriteAttribute("size", locations.size(), dataset);
}

void EpiHDF5Writer::WriteHealthStatus(H5::Group& group, const std::shared_ptr<Location<Coordinate>> location) {
        auto pools_group = group.createGroup("Pools");
        for (unsigned id = 0; id < NumOfTypes(); ++id){
                WritePoolHealthStatus(pools_group, location, static_cast<Id>(id));
        }
}

void EpiHDF5Writer::WritePoolHealthStatus(H5::Group&                                          group,
                                          const std::shared_ptr<geopop::Location<Coordinate>> location,
                                          stride::ContactType::Id                             id)
{
        vector<unsigned long> status(NumOfHealthStatus(), 0);
        for (const auto &pool : location->CRefPools(id)) {
                for (const auto &person : *pool) {
                        status[ToSize(person->GetHealth().GetStatus())]++;
                }
        }
        std::vector<double> health_status;
        for(size_t i = 0; i < status.size(); ++i){
                health_status.push_back((double)status[i]/location->GetPopCount());
        }
        WriteDataset(ToString(id), health_status, group);
}

template <typename T>
void EpiHDF5Writer::WriteAttribute(const std::string& name, const T& value, H5Object& object)
{
        auto type = GetH5Type(value);
        auto attr = object.createAttribute(name, type, DataSpace());
        attr.write(type, &value);
}
template <typename T>
void EpiHDF5Writer::WriteDataset(const std::string& name, std::vector<T> values, H5Object& object)
{
        hsize_t dimensions[1] = {values.size()};
        T       emptyValue;
        auto    type      = GetH5Type(emptyValue);
        auto    dataSpace = DataSpace(1, dimensions);
        auto    dataSet   = object.createDataSet(name, type, dataSpace);
        dataSet.write(values.data(), type);
}

}