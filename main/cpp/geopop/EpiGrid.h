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
class EpiGrid : private Region
{
public:
        /// EpiGrid and associated Population.
        explicit EpiGrid();

        /// No copy constructor.
        EpiGrid(const EpiGrid&) = delete;

        /// No copy assignment.
        EpiGrid operator=(const EpiGrid&) = delete;

        /// Adds a location to this EpiGrid.
        void AddLocation(std::shared_ptr<EpiLocation> location);

        /// Disables the addLocation method and builds the kdtree.
        void Finalize();

        /// Gets a EpiLocation by Id and check if the Id exists.
        std::shared_ptr<EpiLocation> GetById(unsigned int id) const { return std::static_pointer_cast<EpiLocation>(Region::GetById(id)); }

        /// Search for locations in \p radius (in km) around \p start.
        std::vector<const EpiLocation*> LocationsInRadius(const EpiLocation& start, double radius) const;

        /**
         * Gets the locations in a rectangle determined by the two coordinates (long1, lat1) and (long2, lat2).
         * The coordinates must be positioned on the diagonal, i.e:
         *
         *  p1 -----+     +-------p1
         *  |       |     |       |
         *  |       |  or |       |
         *  |       |     |       |
         *  +-------p2    p2------+
         */
        std::set<const EpiLocation*> LocationsInBox(double long1, double lat1, double long2, double lat2) const;

        /// Gets the location in a rectangle defined by the two EpiLocations.
        std::set<const EpiLocation*> LocationsInBox(EpiLocation* loc1, EpiLocation* loc2) const;

        /// Gets the K biggest (in population count) locations of this EpiGrid
        std::vector<EpiLocation*> TopK(size_t k) const;

public:
        /// Build a GeoAggregator with a predefined functor and given args for the Policy.
        template <typename Policy, typename F>
        GeoAggregator<Coordinate, Policy, F> BuildAggregator(F functor, typename Policy::Args&& args) const;

        /// Build a GeoAggregator that gets its functor when calling, with given args for the Policy.
        template <typename Policy>
        GeoAggregator<Coordinate, Policy> BuildAggregator(typename Policy::Args&& args) const;

public:
        using iterator       = std::vector<std::shared_ptr<EpiLocation>>::iterator;
        using const_iterator = std::vector<std::shared_ptr<EpiLocation>>::const_iterator;

        /// Gets an EpiLocation by index, doesn't performs a range check.
        std::shared_ptr<EpiLocation> operator[](size_t index) { return std::static_pointer_cast<EpiLocation>(Region::operator[](index)); }

        /// Gets an EpiLocation by index, doesn't performs a range check.
        const std::shared_ptr<EpiLocation> operator[](size_t index) const { return std::static_pointer_cast<EpiLocation>(Region::operator[](index)); }

        /// Gets a range of EpiLocation indices by province ID
        const std::vector<unsigned int> get_L_for_P(const unsigned int& province){ return Region::get_L_for_P(province); }

        /// Gets current size of EpiLocation storage.
        size_t size() const { return Region::size(); }
};

} // namespace geopop
