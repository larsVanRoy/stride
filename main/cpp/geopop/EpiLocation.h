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

#include "disease/Health.h"
#include "geopop/geo/GeoLocation.h"
#include "contact/PoolStatus.h"

#include <geopop/geo/Coordinate.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace geopop {

/**
 * EpiLocation for use within the GeoGrid, contains GeoLocation and index to ContactPools.
 */
template <class CoordinateLike>
class EpiLocation : public GeoLocation<CoordinateLike>
{
public:
        /// Parametrized constructor with population count.
        EpiLocation(unsigned int id, unsigned int province, CoordinateLike coordinate = CoordinateLike(0.0, 0.0),
                 std::string name = "", unsigned int popCount = 0U);

        /// Perform a full comparison with the other location.
        bool operator==(const EpiLocation<CoordinateLike>& other) const;

        size_t maxDays() { return m_history.size(); }

        std::shared_ptr<stride::PoolStatus> GetPoolStatus(unsigned int day) { return m_history.at(day); };

        const std::shared_ptr<const stride::PoolStatus> GetPoolStatus(unsigned int day) const { return m_history.at(day); };

        void AddPoolStatus(std::shared_ptr<stride::PoolStatus> status) { m_history.push_back(status); };

private:
        /// Stores the history of the location for every step
        std::vector<std::shared_ptr<stride::PoolStatus>> m_history;

public:
        typedef CoordinateLike coordinate_type;

};

extern template class EpiLocation<Coordinate>;

} // namespace geopop
