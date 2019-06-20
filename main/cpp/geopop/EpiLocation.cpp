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

#include "EpiLocation.h"
#include "contact/ContactType.h"
#include "disease/Health.h"

#include <boost/geometry/core/access.hpp>
#include <algorithm>
#include <cmath>

namespace geopop {

using namespace std;
using namespace stride::ContactType;

template <class CoordinateLike>
EpiLocation<CoordinateLike>::EpiLocation(unsigned int id, unsigned int province, CoordinateLike coordinate, string name, unsigned int popCount)
    : GeoLocation<CoordinateLike>(id, province, coordinate, name, popCount), m_history()
{
}

template <class CoordinateLike>
bool EpiLocation<CoordinateLike>::operator==(const EpiLocation<CoordinateLike>& other) const
{
        using boost::geometry::get;

        bool temp = (m_history == other.m_history);

        return temp && this->GetID() == other.GetID() && get<0>(this->GetCoordinate()) == get<0>(other.GetCoordinate()) &&
               get<1>(this->GetCoordinate()) == get<1>(other.GetCoordinate()) && this->GetName() == other.GetName() &&
               this->GetProvince() == other.GetProvince() && this->GetPopCount() == other.GetPopCount();
}

template class EpiLocation<Coordinate>;

} // namespace geopop

