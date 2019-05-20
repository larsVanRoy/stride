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

#include "geopop/geo/GeoGridKdTree.h"

#include <set>
#include <unordered_map>
#include <vector>

namespace geopop {

class GeoLocation;

template <typename Policy, typename... F>
class GeoAggregator;

/**
 * A Geographic grid of simulation region contains GeoLocations that in turn contain
 * an index to the ContactPools situated at that GeoLocation.
 */
class Region
{
public:
    /// Region and associated Population.
    explicit Region();

    /// No copy constructor.
    Region(const Region&) = delete;

    /// No copy assignment.
    Region operator=(const Region&) = delete;

    /// Adds a location to this Region.
    void AddGeoLocation(std::shared_ptr<GeoLocation> location);

    /// Disables the addLocation method and builds the kdtree.
    void Finalize();

    /// Gets a GeoLocation by Id and check if the Id exists.
    std::shared_ptr<GeoLocation> GetById(unsigned int id) const { return m_locations[m_id_to_index.at(id)]; }

    /// Search for locations in \p radius (in km) around \p start.
    std::vector<const GeoLocation*> GeoLocationsInRadius(const GeoLocation& start, double radius) const;

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
    std::set<const GeoLocation*> GeoLocationsInBox(double long1, double lat1, double long2, double lat2) const;

    /// Gets the location in a rectangle defined by the two GeoLocations.
    std::set<const GeoLocation*> GeoLocationsInBox(GeoLocation* loc1, GeoLocation* loc2) const;

    /// Gets the K biggest (in population count) locations of this Region
    std::vector<GeoLocation*> TopK(size_t k) const;

public:
    /// Build a GeoAggregator with a predefined functor and given args for the Policy.
    template <typename Policy, typename F>
    GeoAggregator<Policy, F> BuildAggregator(F functor, typename Policy::Args&& args) const;

    /// Build a GeoAggregator that gets its functor when calling, with given args for the Policy.
    template <typename Policy>
    GeoAggregator<Policy> BuildAggregator(typename Policy::Args&& args) const;

public:
    using iterator       = std::vector<std::shared_ptr<GeoLocation>>::iterator;
    using const_iterator = std::vector<std::shared_ptr<GeoLocation>>::const_iterator;

    /// Iterator to first GeoLocation.
    iterator begin() { return m_locations.begin(); }

    /// Iterator to the end of the GeoLocation storage.
    iterator end() { return m_locations.end(); }

    /// Const Iterator to first GeoLocation.
    const_iterator cbegin() const { return m_locations.cbegin(); }

    /// Const iterator to the end of the GeoLocation storage.
    const_iterator cend() const { return m_locations.cend(); }

    /// Gets a GeoLocation by index, doesn't performs a range check.
    std::shared_ptr<GeoLocation>& operator[](size_t index) { return m_locations[index]; }

    /// Gets a GeoLocation by index, doesn't performs a range check.
    const std::shared_ptr<GeoLocation>& operator[](size_t index) const { return m_locations[index]; }

    /// Gets a range of GeoLocation indices by province ID
    const std::vector<unsigned int> get_L_for_P(const unsigned int& province){ return p_id_to_index[province]; }

    /// Gets current size of GeoLocation storage.
    size_t size() const { return m_locations.size(); }

private:
    ///< Checks whether the Region is finalized i.e. certain operations can(not) be used.
    void CheckFinalized(const std::string& functionName) const;

private:
    ///< Container for GeoLocations in Region.
    std::vector<std::shared_ptr<GeoLocation>> m_locations;

    ///< Associative container maps GeoLocation Id to index in m_locations.
    std::unordered_map<unsigned int, unsigned int> m_id_to_index;

    ///< Associative container maps Province Ids to indices in m_locations.
    std::unordered_map<unsigned int, std::vector<unsigned int>> p_id_to_index;

    ///< Is the Region finalized (ready for use) yet?
    bool m_finalized;

    ///< Internal KdTree for quick spatial lookup.
    GeoGridKdTree m_tree;
};

} // namespace geopop
