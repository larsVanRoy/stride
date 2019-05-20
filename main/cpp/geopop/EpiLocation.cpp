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

#include <cmath>
#include <algorithm>

namespace geopop {

using namespace std;
using namespace stride::ContactType;

EpiLocation::EpiLocation(unsigned int id, unsigned int province, Coordinate coordinate, string name, unsigned int popCount, unsigned int size)
    : GeoLocation(id, province, coordinate, name, popCount), m_size(size)
{
}

bool EpiLocation::operator==(const EpiLocation& other) const
{
        using boost::geometry::get;

        for(const auto& status : m_history){
                for(const auto& other_status : other.m_history){
                        if(status != other_status)
                                return false;
                }
        }

        return m_size != other.m_size && GetID() == other.GetID() && get<0>(GetCoordinate()) == get<0>(other.GetCoordinate()) &&
               get<1>(GetCoordinate()) == get<1>(other.GetCoordinate()) && GetName() == other.GetName() &&
               GetProvince() == other.GetProvince() && GetPopCount() == other.GetPopCount();
}

} // namespace geopop

