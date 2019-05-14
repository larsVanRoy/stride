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

#include "GeoGridReader.h"
#include "H5Cpp.h"
#include "contact/ContactPool.h"
#include "contact/ContactType.h"
#include "geopop/Location.h"

namespace geopop {

class GeoGrid;

/**
 * An implementation of the GeoGridReader using HDF5.
 * This class is used to read a GeoGrid from a HDF5 file.
 */

class GeoGridHDF5Reader : public GeoGridReader
{
public:
        /// Construct the GeoGridHDF5Reader with the filename which contains the HDF5.
        GeoGridHDF5Reader(const std::string& filename, stride::Population* pop);

        /// No copy constructor.
        GeoGridHDF5Reader(const GeoGridHDF5Reader&) = delete;

        /// No copy assignment.
        GeoGridHDF5Reader operator=(const GeoGridHDF5Reader&) = delete;

        /// Actually perform the read and return the GeoGrid.
        void Read() override;

private:
        std::string m_filename;

protected:
        void ReadPeople(const H5::H5File& file);

        void ReadLocations(const H5::H5File& file, GeoGrid& geoGrid);

        void ReadLocation(const H5::Group& object, GeoGrid& geoGrid);

        void ReadContactPool(const H5::DataSet& object, std::shared_ptr<Location> location_ptr);

        template <typename T>
        void ReadAttribute(const std::string& name, const H5::DataType& type, T* data, const H5::H5Location& object);

        template <typename T>
        void ReadDataset(const std::string& name, const H5::DataType& type, T* data, const H5::CommonFG& object);
};
template<>
void GeoGridHDF5Reader::ReadAttribute(const std::string& name, const H5::DataType& type, std::string* data, const H5::H5Location& object);
} // namespace geopop
