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

#include "Location.h"
#include "contact/ContactPool.h"
#include "contact/ContactType.h"
#include "disease/Health.h"
#include "pop/Person.h"
#include "util/Exception.h"

#include <boost/geometry/core/access.hpp>
#include <algorithm>
#include <cmath>

namespace geopop {

using namespace std;
using namespace stride::ContactType;

template <class CoordinateLike>
Location<CoordinateLike>::Location(unsigned int id, unsigned int province, CoordinateLike coordinate, string name, unsigned int popCount)
    : GeoLocation<CoordinateLike>(id, province, coordinate, name, popCount), m_inCommutes(), m_outCommutes(), m_pool_index()
{
}

template <class CoordinateLike>
bool Location<CoordinateLike>::operator==(const Location<CoordinateLike>& other) const
{
        using boost::geometry::get;

        auto temp = true;
        for (Id typ : IdList) {
                temp = temp && (CRefPools(typ) == other.CRefPools(typ));
        }
        return temp && this->GetID() == other.GetID() && get<0>(this->GetCoordinate()) == get<0>(other.GetCoordinate()) &&
               get<1>(this->GetCoordinate()) == get<1>(other.GetCoordinate()) && this->GetName() == other.GetName() &&
               this->GetProvince() == other.GetProvince() && this->GetPopCount() == other.GetPopCount() &&
               CRefIncomingCommutes() == other.CRefIncomingCommutes() &&
               CRefOutgoingCommutes() == other.CRefOutgoingCommutes();
}

template <class CoordinateLike>
void Location<CoordinateLike>::AddIncomingCommute(shared_ptr<Location<CoordinateLike>> otherLocation, double fraction)
{
        m_inCommutes.emplace_back(otherLocation.get(), fraction);
}

template <class CoordinateLike>
void Location<CoordinateLike>::AddOutgoingCommute(shared_ptr<Location<CoordinateLike>> otherLocation, double fraction)
{
        m_outCommutes.emplace_back(otherLocation.get(), fraction);
}

template <class CoordinateLike>
int Location<CoordinateLike>::GetIncomingCommuteCount(double fractionCommuters) const
{
        double value = 0;
        for (const auto& locProportion : m_inCommutes) {
                // locProportion.second of the people in locProportion.first are commuting to this
                value += locProportion.second * (fractionCommuters * (double)locProportion.first->GetPopCount());
        }
        return static_cast<int>(floor(value));
}

template <class CoordinateLike>
unsigned int Location<CoordinateLike>::GetInfectedCount() const
{
        unsigned int total{0U};
        for (const auto& pool : CRefPools<Id::Household>()) {
                for (const auto& person : *pool) {
                        const auto& h = person->GetHealth();
                        total += h.IsInfected() || h.IsRecovered();
                }
        }
        return total;
}

template <class CoordinateLike>
unsigned int Location<CoordinateLike>::GetOutgoingCommuteCount(double fractionCommuters) const
{
        double totalProportion = 0;
        for (const auto& locProportion : m_outCommutes) {
                // locProportion.second of the people in this are commuting to locProportion.first
                totalProportion += locProportion.second;
        }
        return static_cast<unsigned int>(floor(totalProportion * (fractionCommuters * this->GetPopCount())));
}

template <class CoordinateLike>
unsigned int Location<CoordinateLike>::GetContactPoolId(stride::ContactType::Id id) {
        return m_pool_index[id].back()->GetId();
}

template class Location<Coordinate>;
} // namespace geopop
