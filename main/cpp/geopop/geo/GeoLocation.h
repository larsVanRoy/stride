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

#include "Coordinate.h"

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace stride {
class ContactPool;
}

namespace geopop {

/**
 * GeoLocation for use within the GeoGrid, contains Coordinate and index to ContactPools.
 */
template <class Data>
class GeoLocation
{
public:
        /// Parametrized constructor with population count.
        GeoLocation(unsigned int id, unsigned int province, Data coordinate = Coordinate(0.0, 0.0),
                 std::string name = "", unsigned int popCount = 0U);

        /// Perform a full comparison with the other location.
        bool operator==(const GeoLocation& other) const;

        /// Gets the Coordinate of this GeoLocation.
        const Data GetCoordinate() const { return m_coordinate; }

        /// Gets ID of this GeoLocation.
        unsigned int GetID() const { return m_id; }

        /// Gets the name.
        std::string GetName() const { return m_name; }

        /// Gets the absolute population.
        unsigned int GetPopCount() const { return m_pop_count; }

        /// Gets the province.
        unsigned int GetProvince() const { return m_province; }

        /// Get GeoLocation's population fraction (of the total population count).
        double GetPopFraction() const;

        /// Sets the Coordinate of this GeoLocation.
        void SetCoordinate(const Data& coordinate) { m_coordinate = coordinate; }

        /// Set GeoLocation's population count using its population fraction and the total population count.
        void SetPopCount(unsigned int totalPopCount);

        /// Set GeoLocation's population fraction (of the total populaion count).
        void SetPopFraction(double relativePopulation);

private:
        Data   m_coordinate;   ///< Coordinate of the GeoLocation.
        unsigned int m_id = 0U;      ///< Id.
        std::string  m_name;         ///< Name.
        unsigned int m_pop_count;    ///< Population count (number of individuals) at this GeoLocation.
        double       m_pop_fraction; ///< Fraction of whole population at this GeoLocation.
        unsigned int m_province;     ///< Province id.
};

} // namespace geopop
