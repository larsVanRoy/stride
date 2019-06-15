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

#include "GeoGrid.h"

#include "contact/ContactPool.h"
#include "geopop/Location.h"
#include "pop/Population.h"

#include <queue>
#include <stdexcept>
#include <utility>

namespace geopop {

using namespace std;
using stride::ContactPool;
using stride::ContactType::Id;

GeoGrid::GeoGrid(stride::Population* population)
    : Region(), m_population(population)
{
}

void GeoGrid::AddLocation(shared_ptr<Location> location)
{
        Region::AddGeoLocation(location);
}

template <typename Policy, typename F>
GeoAggregator<Policy, F> GeoGrid::BuildAggregator(F functor, typename Policy::Args&& args) const
{
        return Region::BuildAggregator(functor, args);
}

template <typename Policy>
GeoAggregator<Policy> GeoGrid::BuildAggregator(typename Policy::Args&& args) const
{
        return Region::BuildAggregator(args);
}

void GeoGrid::Finalize()
{
        Region::Finalize();
}

set<const Location*> GeoGrid::LocationsInBox(double long1, double lat1, double long2, double lat2) const
{
        set<const GeoLocation<Coordinate>*> s = Region::GeoLocationsInBox(long1, lat1, long2, lat2);
        set<const Location*> result;
        for(const GeoLocation<Coordinate>* g : s){
                result.insert(static_cast<const Location*>(g));
        }
        return result;
}

set<const Location*> GeoGrid::LocationsInBox(Location* loc1, Location* loc2) const
{
        using boost::geometry::get;
        return LocationsInBox(get<0>(loc1->GetCoordinate()), get<1>(loc1->GetCoordinate()),
                              get<0>(loc2->GetCoordinate()), get<1>(loc2->GetCoordinate()));
}

vector<const Location*> GeoGrid::LocationsInRadius(const Location& start, double radius) const
{
        vector<const GeoLocation<Coordinate>*> s = Region::GeoLocationsInRadius(start, radius);
        vector<const Location*> result;
        result.reserve(s.size());
        for(const GeoLocation<Coordinate>* g : s){
                result.push_back(static_cast<const Location*>(g));
        }
        return result;
}

vector<ContactPool*> GeoGrid::GetNearbyPools(Id id, const Location& start, double startRadius) const
{
        double               currentRadius = startRadius;
        vector<ContactPool*> pools;

        while (pools.empty()) {
                for (const Location* nearLoc : LocationsInRadius(start, currentRadius)) {
                        const auto& locPool = nearLoc->CRefPools(id);
                        pools.insert(pools.end(), locPool.begin(), locPool.end());
                }
                currentRadius *= 2;
                if (currentRadius == numeric_limits<double>::infinity()) {
                        break;
                }
        }
        return pools;
}

vector<Location*> GeoGrid::TopK(size_t k) const
{
        vector<GeoLocation<Coordinate>*> s = Region::TopK(k);
        vector<Location*> result;
        result.reserve(s.size());
        for(GeoLocation<Coordinate>* g : s){
                result.push_back(static_cast<Location*>(g));
        }
        return result;
}

} // namespace geopop
