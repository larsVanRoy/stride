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

#pragma once

#include "contact/ContactType.h"
#include "geopop/geo/Region.h"
#include "EpiLocation.h"

#include <set>
#include <unordered_map>
#include <vector>
#include <memory>


namespace geopop {

//class EpiLocation;

template <typename Policy, typename... F>
class GeoAggregator;

/**
 * A Geographic grid of simulation region contains EpiLocations that in turn contain
 * an index to the HealthPool situated at that EpiLocation.
 */
class EpiGrid : public Region<EpiLocation<Coordinate>>
{
public:
        /// EpiGrid and associated Population.
        explicit EpiGrid();

        /// No copy constructor.
        EpiGrid(const EpiGrid&) = delete;

        /// No copy assignment.
        EpiGrid operator=(const EpiGrid&) = delete;
};

} // namespace geopop
