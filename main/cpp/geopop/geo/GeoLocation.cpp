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

#include "GeoLocation.h"

#include <cmath>
#include <algorithm>

namespace geopop {

using namespace std;

GeoLocation::GeoLocation(unsigned int id, unsigned int province, Coordinate coordinate, string name, unsigned int popCount)
    : m_coordinate(coordinate), m_id(id), m_name(move(name)), m_pop_count(popCount), m_pop_fraction(0.0),
      m_province(province)
{
}

bool GeoLocation::operator==(const GeoLocation& other) const
{
        using boost::geometry::get;

        return GetID() == other.GetID() && get<0>(GetCoordinate()) == get<0>(other.GetCoordinate()) &&
               get<1>(GetCoordinate()) == get<1>(other.GetCoordinate()) && GetName() == other.GetName() &&
               GetProvince() == other.GetProvince() && GetPopCount() == other.GetPopCount();
}

double GeoLocation::GetPopFraction() const { return m_pop_fraction; }

void GeoLocation::SetPopCount(unsigned int totalPopCount)
{
        m_pop_count = static_cast<unsigned int>(floor(m_pop_fraction * totalPopCount));
}
void GeoLocation::SetPopFraction(double relativePopulation) { m_pop_fraction = relativePopulation; }

} // namespace geopop
