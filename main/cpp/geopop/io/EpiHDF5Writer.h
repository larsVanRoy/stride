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

#include "geopop/EpiGrid.h"
#include "geopop/EpiLocation.h"
#include "geopop/io/EpiWriter.h"
#include "H5Cpp.h"

namespace geopop {
class EpiHDF5Writer : public EpiWriter
{
public:
        /// Construct the EpiJSONWriter.
        EpiHDF5Writer(const std::string& filename) : EpiWriter(), m_filename(filename), m_file(H5::H5File(filename, H5F_ACC_TRUNC)){};

        ~EpiHDF5Writer() = default;

        /// Convert the provided GeoGrid to an the epidemiological status and write the status to the provided file
        void Write(const geopop::GeoGrid& geoGrid, unsigned timeStep) override;

        /// Generate file name and open the file stream.
        void Initialize(const geopop::GeoGrid& geoPopGrid) override;

        void Finalize() override;
private:
        /// Write location in HDF5 file
        void WriteLocation(H5::Group& locations_group, const std::shared_ptr<geopop::Location<Coordinate>> location);

        /// Write locations in HDF5 file
        void WriteLocations(const geopop::GeoGrid& geoGrid);

        /// Write HealthStatus in HDF5 file
        void WriteHealthStatus(H5::Group& group, const std::shared_ptr<geopop::Location<geopop::Coordinate>> location);

        template<typename T>
        void WriteAttribute(const std::string& name, const T& value, H5::H5Object& object);

        template<typename T>
        void WriteDataset(const std::string& name, std::vector<T> values, H5::H5Object& object);

private:
        std::string m_filename; ///< name of the file to write to
        std::vector<unsigned int> m_timesteps = {};
        unsigned int m_location_counter = 0;
        H5::H5File m_file;
};
} // namespace geopop
