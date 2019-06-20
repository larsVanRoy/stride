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

#include "H5Cpp.h"
#include "geopop/EpiGrid.h"
#include "geopop/EpiLocation.h"
#include "geopop/io/EpiReader.h"
#include "util/Exception.h"

#include <memory>
namespace geopop {
class EpiHDF5Reader : public EpiReader
{
public:
        /// Construct the EpiJSONReader with the istream which contains the JSON.
        explicit EpiHDF5Reader(std::unique_ptr<std::ifstream> inputStream) = delete;

        explicit EpiHDF5Reader(const std::string& filename)
            : EpiReader(filename), m_grid(std::make_shared<EpiGrid>()), m_file(H5::H5File(filename, H5F_ACC_RDONLY))
        {
        }

        ~EpiHDF5Reader() = default;

        /// No copy constructor.
        EpiHDF5Reader(const EpiHDF5Reader&) = delete;

        /// No copy assignement.
        EpiHDF5Reader operator=(const EpiHDF5Reader&) = delete;

        /// Actually perform the read and return the Epi.
        std::shared_ptr<EpiGrid> Read() override;

        void Print();

private:
        /// Create EpiGrid and fills it with EpiLocations based on the information stored in the HDF5 File.
        void ReadLocations();

        void ReadHistory();

        void ReadTimeStep(unsigned int timestep);

        void ReadHistoryLocation(const H5::Group& location_group);

        void ReadLocationPools(const H5::Group& location_group, std::shared_ptr<EpiLocation<Coordinate>> location);

        template <typename T>
        void ReadAttribute(const std::string& name, T* data, const H5::H5Object& object);

        template <typename T>
        void ReadDataset(const std::string& name, T* data, const H5::H5Object& object);

private:
        std::shared_ptr<EpiGrid> m_grid;
        H5::H5File               m_file;
};
} // namespace geopop
