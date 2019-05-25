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

#include "EpiReader.h"
#include "EpiJSONReader.h"

#include <memory>
#include <string>


namespace geopop {

/**
 * Factory creates the correct implementation of an EpiWriter based on the filename extension.
 */
class EpiReaderFactory
{
public:
    /// Create an EpiWriter based on the provided extension in the filename.
    static std::shared_ptr<EpiReader> CreateEpiReader(const std::string& filename);
};

} // namespace geopop
