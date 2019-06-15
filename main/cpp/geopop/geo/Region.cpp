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

Region::Region()
        : m_locations(), m_id_to_index(), p_id_to_index(), m_finalized(false), m_tree()
{
}

void Region::AddGeoLocation(shared_ptr<GeoLocation<Coordinate>> location)
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

template <typename Policy, typename F>
GeoAggregator<Coordinate, Policy, F> Region::BuildAggregator(F functor, typename Policy::Args&& args) const
{
    return GeoAggregator<Coordinate, Policy, F>(m_tree, functor, std::forward<typename Policy::Args>(args));
}

template <typename Policy>
GeoAggregator<Coordinate, Policy> Region::BuildAggregator(typename Policy::Args&& args) const
{
    return GeoAggregator<Coordinate, Policy>(m_tree, std::forward<typename Policy::Args>(args));
}

void Region::CheckFinalized(const string& functionName) const
{
    if (!m_finalized) {
        throw std::runtime_error("Calling \"" + functionName + "\" with Region not finalized not supported!");
    }
}

void Region::Finalize()
{
    vector<geogrid_detail::KdTree2DPoint<Coordinate>> points;
    for (const auto& loc : m_locations) {
        points.emplace_back(geogrid_detail::KdTree2DPoint(loc.get()));
    }
    m_tree      = GeoGridKdTree::Build(points);
    m_finalized = true;
}

set<const GeoLocation<Coordinate>*> Region::GeoLocationsInBox(double long1, double lat1, double long2, double lat2) const
{
    CheckFinalized(__func__);

    set<const GeoLocation<Coordinate>*> result;

    auto agg = BuildAggregator<BoxPolicy<Coordinate>>(
            MakeCollector(inserter(result, result.begin())),
            make_tuple(min(long1, long2), min(lat1, lat2), max(long1, long2), max(lat1, lat2)));
    agg();

    return result;
}

set<const GeoLocation<Coordinate>*> Region::GeoLocationsInBox(GeoLocation<Coordinate>* loc1, GeoLocation<Coordinate>* loc2) const
{
    using boost::geometry::get;
    return GeoLocationsInBox(get<0>(loc1->GetCoordinate()), get<1>(loc1->GetCoordinate()),
                          get<0>(loc2->GetCoordinate()), get<1>(loc2->GetCoordinate()));
}

vector<GeoLocation<Coordinate>*> Region::TopK(size_t k) const
{
    auto cmp = [](GeoLocation<Coordinate>* rhs, GeoLocation<Coordinate>* lhs) { return rhs->GetPopCount() > lhs->GetPopCount(); };

    priority_queue<GeoLocation<Coordinate>*, vector<GeoLocation<Coordinate>*>, decltype(cmp)> queue(cmp);
    for (const auto& loc : m_locations) {
        queue.push(loc.get());
        if (queue.size() > k) {
            queue.pop();
        }
    }

    vector<GeoLocation<Coordinate>*> topLocations;
    while (!queue.empty()) {
        auto loc = queue.top();
        topLocations.push_back(loc);
        queue.pop();
    }

    return topLocations;
}

vector<const GeoLocation<Coordinate>*> Region::GeoLocationsInRadius(const GeoLocation<Coordinate>& start, double radius) const
{
    CheckFinalized(__func__);

    geogrid_detail::KdTree2DPoint startPt(&start);
    vector<const GeoLocation<Coordinate>*>       result;

    auto agg = BuildAggregator<RadiusPolicy<Coordinate>>(MakeCollector(back_inserter(result)), make_tuple(startPt, radius));
    agg();

    return result;
}
} // namespace geopop
