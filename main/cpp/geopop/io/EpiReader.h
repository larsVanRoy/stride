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

#include "geopop/GeoGrid.h"

#include <memory>
#include <ostream>

namespace geopop {

class EpiJSONReader;
/**
 * An interface for reading the epidemiological status to a file, can be implemented with multiple file types.
 * json is currently implemented.
 */
class EpiReader
{
public:
    EpiReader() {};
    /// Construct the Writer.
    virtual ~EpiReader();

    /// Write the epidemiological status to ostream.
    virtual void Read(const geopop::GeoGrid& geoGrid, const std::string& filename) = 0;
};

} // namespace geopop
