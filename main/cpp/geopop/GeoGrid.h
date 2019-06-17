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
#include "Location.h"

#include <set>
#include <unordered_map>
#include <vector>
#include <memory>

namespace stride {
class ContactPool;
class Population;
} // namespace stride

namespace geopop {

//class Location;

template <typename Policy, typename... F>
class GeoAggregator;

/**
 * A Geographic grid of simulation region contains Locations that in turn contain
 * an index to the ContactPools situated at that Location.
 */
class GeoGrid : public Region<Location<Coordinate>>
{
public:
        /// GeoGrid and associated Population.
        explicit GeoGrid(stride::Population* population);

        /// No copy constructor.
        GeoGrid(const GeoGrid&) = delete;

        /// No copy assignment.
        GeoGrid operator=(const GeoGrid&) = delete;

        /// Get the Population associated with this GeoGrid
        stride::Population* GetPopulation() const { return m_population; }


        /// Find contactpools in startRadius (in km) around start and, if none are found, double
        /// the radius and search again until the radius gets infinite. May return an empty vector
        /// when there are really no pools to be found (empty grid).
        std::vector<stride::ContactPool*> GetNearbyPools(stride::ContactType::Id id, const Location<Coordinate>& start,
                                                         double startRadius = 10.0) const;

private:
        ///< Stores pointer to Popluation, but does not take ownership.
        stride::Population* m_population;
};

} // namespace geopop
