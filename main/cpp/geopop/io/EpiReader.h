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
#include <fstream>

namespace geopop {

class EpiGrid;
class EpiJSONReader;
/**
 * An interface for reading the epidemiological status to a file, can be implemented with multiple file types.
 * json is currently implemented.
 */
class EpiReader
{
public:
    explicit EpiReader(std::unique_ptr<std::ifstream> input_stream) : m_stream(std::move(input_stream)) {};
    /// Construct the Writer.

    virtual ~EpiReader() {m_stream->close(); };

    /// Write the epidemiological status to ostream.
    virtual std::shared_ptr<EpiGrid> Read() = 0;

protected:
    std::unique_ptr<std::ifstream> m_stream;
};

} // namespace geopop
