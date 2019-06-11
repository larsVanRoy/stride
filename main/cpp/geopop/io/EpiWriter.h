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

class EpiJSONWriter;
/**
 * An interface for writing the epidemiological status to a file, can be implemented with multiple file types.
 * json is currently implemented.
 */
class EpiWriter
{
public:
    EpiWriter() {};
    /// Construct the Writer.
    virtual ~EpiWriter() {};

    /// Write the epidemiological status to ostream.
    virtual void Write(const geopop::GeoGrid& geoGrid, unsigned timeStep) = 0;

    virtual void Initialize(const geopop::GeoGrid& geoGrid) = 0;

    virtual void Finalize() = 0;
};

} // namespace geopop
