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

#include "EpiGrid.h"

#include "contact/ContactPool.h"
#include "geopop/EpiLocation.h"
#include "pop/Population.h"

#include <queue>
#include <stdexcept>
#include <utility>

namespace geopop {

using namespace std;
using stride::ContactPool;
using stride::ContactType::Id;

EpiGrid::EpiGrid()
{
}

void EpiGrid::AddLocation(shared_ptr<EpiLocation> loc)
{
    this->AddGeoLocation(std::dynamic_pointer_cast<GeoLocation>(loc));
}

template <typename Policy, typename F>
GeoAggregator<Policy, F> EpiGrid::BuildAggregator(F functor, typename Policy::Args&& args) const
{
        return Region::BuildAggregator(functor, args);
}

template <typename Policy>
GeoAggregator<Policy> EpiGrid::BuildAggregator(typename Policy::Args&& args) const
{
        return Region::BuildAggregator(args);
}

void EpiGrid::Finalize()
{
        Region::Finalize();
}

set<const EpiLocation*> EpiGrid::LocationsInBox(double long1, double lat1, double long2, double lat2) const
{
        set<const GeoLocation*> s = Region::GeoLocationsInBox(long1, lat1, long2, lat2);
        set<const EpiLocation*> result;
        for(const GeoLocation* g : s){
                result.insert(static_cast<const EpiLocation*>(g));
        }
        return result;
}

set<const EpiLocation*> EpiGrid::LocationsInBox(EpiLocation* loc1, EpiLocation* loc2) const
{
        using boost::geometry::get;
        return LocationsInBox(get<0>(loc1->GetCoordinate()), get<1>(loc1->GetCoordinate()),
                              get<0>(loc2->GetCoordinate()), get<1>(loc2->GetCoordinate()));
}

vector<const EpiLocation*> EpiGrid::LocationsInRadius(const EpiLocation& start, double radius) const
{
        vector<const GeoLocation*> s = Region::GeoLocationsInRadius(start, radius);
        vector<const EpiLocation*> result;
        result.reserve(s.size());
        for(const GeoLocation* g : s){
                result.push_back(static_cast<const EpiLocation*>(g));
        }
        return result;
}

vector<EpiLocation*> EpiGrid::TopK(size_t k) const
{
        vector<GeoLocation*> s = Region::TopK(k);
        vector<EpiLocation*> result;
        result.reserve(s.size());
        for(GeoLocation* g : s){
                result.push_back(static_cast<EpiLocation*>(g));
        }
        return result;
}

} // namespace geopop
