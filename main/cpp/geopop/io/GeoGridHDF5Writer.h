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

#include "GeoGridWriter.h"
#include "H5Cpp.h"
#include "geopop/Location.h"

#include <H5Location.h>
#include <set>

namespace stride {
class ContactPool;
class Person;
} // namespace stride

namespace geopop {

/**
 * Writes a GeoGrid to a HDF5 file.
 */
class GeoGridHDF5Writer : public GeoGridWriter
{
public:
        /// Construct the GeoGridHDF5Writer.
        GeoGridHDF5Writer();

        /// Write the provided GeoGrid to the proved ostream in HDF5 format.
        void Write(GeoGrid& geoGrid, std::ostream& stream) override;

        /// Write the provided GeoGrid to the provided file in HDF format.
        void Write(GeoGrid& geoGrid, const std::string& filename) override;

private:

        /// Create a Boost Property Tree containing all info needed to reconstruct a ContactPool.
        void WriteContactPool(H5::Group& group, stride::ContactPool* contactPool);

        /// Create a Boost Property Tree containing all info needed to reconstruct a Location.
        void WriteLocation(H5::H5File& file, std::shared_ptr<Location> location);

        /// Create a Boost Property Tree containing all info needed to reconstruct a Person.
        void WritePeople(H5::H5File& file);

        void WriteCommutes(H5::H5File& file, std::shared_ptr<Location> location, H5::Group& group);

        template<typename T>
        void WriteAttribute(const std::string& name, const T& value, H5::H5Object& object);

        template<typename T>
        void WriteDataset(const std::string& name, std::vector<T> values, H5::H5Object& object);

private:
        std::set<stride::Person*> m_persons_found; ///< The persons found when looping over the ContactPools.
        unsigned int m_location_counter = 0U;
        unsigned int m_pool_counter = 0U;
};

template<>
void GeoGridHDF5Writer::WriteAttribute(const std::string& name, const std::string& value, H5::H5Object& object);

} // namespace geopop
