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

#include "Region.h"

#include "geopop/geo/GeoLocation.h"
#include "geopop/geo/GeoAggregator.h"
#include "geopop/geo/GeoGridKdTree.h"

#include <queue>
#include <stdexcept>
#include <utility>

namespace geopop {

using namespace std;

template <class LocationLike>
Region<LocationLike>::Region()
        : m_locations(), m_id_to_index(), p_id_to_index(), m_finalized(false), m_tree()
{
}

template <class LocationLike>
void Region<LocationLike>::AddLocation(shared_ptr<LocationLike> location)
{
    if (m_finalized) {
        throw std::runtime_error("Calling addGeoLocation while Region is finalized not supported!");
    }
    m_locations.emplace_back(location);
    m_id_to_index[location->GetID()] = static_cast<unsigned int>(m_locations.size() - 1);
    if (p_id_to_index.find(location->GetProvince()) != p_id_to_index.end()){
        p_id_to_index[location->GetProvince()].emplace_back(static_cast<unsigned int>(m_locations.size() - 1));
    }
    else{
        p_id_to_index[location->GetProvince()] = {static_cast<unsigned int>(m_locations.size() - 1)};
    }
}

template <class LocationLike>
template <typename Policy, typename F>
GeoAggregator<LocationLike, Policy, F> Region<LocationLike>::BuildAggregator(F functor, typename Policy::Args&& args) const
{
    return GeoAggregator<LocationLike, Policy, F>(m_tree, functor, std::forward<typename Policy::Args>(args));
}

template <class LocationLike>
template <typename Policy>
GeoAggregator<LocationLike, Policy> Region<LocationLike>::BuildAggregator(typename Policy::Args&& args) const
{
    return GeoAggregator<LocationLike, Policy>(m_tree, std::forward<typename Policy::Args>(args));
}

template <class LocationLike>
void Region<LocationLike>::CheckFinalized(const string& functionName) const
{
    if (!m_finalized) {
        throw std::runtime_error("Calling \"" + functionName + "\" with Region not finalized not supported!");
    }
}

template <class LocationLike>
void Region<LocationLike>::Finalize()
{
    using namespace geogrid_detail;
    vector<KdTree2DPoint<LocationLike>> points;
    for (const auto& loc : m_locations) {
        points.emplace_back(KdTree2DPoint(loc.get()));
    }

    m_tree      = KdTree<KdTree2DPoint<LocationLike>>::Build(points);
    m_finalized = true;
}

template <class LocationLike>
set<const LocationLike*> Region<LocationLike>::LocationsInBox(double long1, double lat1, double long2, double lat2) const
{
    CheckFinalized(__func__);

    set<const LocationLike*> result;

    auto agg = BuildAggregator<BoxPolicy<LocationLike>>(
            MakeCollector(inserter(result, result.begin())),
            make_tuple(min(long1, long2), min(lat1, lat2), max(long1, long2), max(lat1, lat2)));
    agg();

    return result;
}

template <class LocationLike>
set<const LocationLike*> Region<LocationLike>::LocationsInBox(LocationLike* loc1, LocationLike* loc2) const
{
    using boost::geometry::get;
    return LocationsInBox(get<0>(loc1->GetCoordinate()), get<1>(loc1->GetCoordinate()),
                          get<0>(loc2->GetCoordinate()), get<1>(loc2->GetCoordinate()));
}

template <class LocationLike>
vector<LocationLike*> Region<LocationLike>::TopK(size_t k) const
{
    auto cmp = [](LocationLike* rhs, LocationLike* lhs) { return rhs->GetPopCount() > lhs->GetPopCount(); };

    priority_queue<LocationLike*, vector<LocationLike*>, decltype(cmp)> queue(cmp);
    for (const auto& loc : m_locations) {
        queue.push(loc.get());
        if (queue.size() > k) {
            queue.pop();
        }
    }

    vector<LocationLike*> topLocations;
    while (!queue.empty()) {
        auto loc = queue.top();
        topLocations.push_back(loc);
        queue.pop();
    }

    return topLocations;
}

template <class LocationLike>
vector<const LocationLike*> Region<LocationLike>::LocationsInRadius(const LocationLike& start, double radius) const
{
    CheckFinalized(__func__);

    geogrid_detail::KdTree2DPoint startPt(&start);
    vector<const LocationLike*>       result;

    auto agg = BuildAggregator<RadiusPolicy<LocationLike>>(MakeCollector(back_inserter(result)), make_tuple(startPt, radius));
    agg();

    return result;
}

template class Region<EpiLocation<Coordinate>>;
template class Region<Location<Coordinate>>;
} // namespace geopop
