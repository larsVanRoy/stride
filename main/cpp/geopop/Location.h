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

#include "contact/ContactType.h"
#include "contact/IdSubscriptArray.h"
#include "geopop/geo/GeoLocation.h"
#include "util/SegmentedVector.h"

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
enum class HealthStatus : unsigned short int;
}

namespace geopop {

/**
 * Location for use within the GeoGrid, contains GeoLocation and index to ContactPools.
 */
template <class CoordinateLike>
class Location : public GeoLocation<CoordinateLike>
{
public:
        /// Parametrized constructor with population count.
        Location(unsigned int id, unsigned int province, CoordinateLike coordinate = CoordinateLike(0.0, 0.0),
                 std::string name = "", unsigned int popCount = 0U);

        /// Perform a full comparison with the other location.
        bool operator==(const Location& other) const;

        /// Adds a Location and a proportion to the incoming commute vector.
        /// I.e. fraction of commuting population at otherLocation commuting to this Location.
        void AddIncomingCommute(std::shared_ptr<Location> otherLocation, double fraction);

        /// Adds a Location and a fraction to the outgoing commute vector.
        /// I.e. fraction of commuting population at this Location commuting to otherLocation.
        void AddOutgoingCommute(std::shared_ptr<Location> otherLocation, double fraction);

        /// Calculates number of incomming commuters, given the fraction of the population that commutes.
        int GetIncomingCommuteCount(double fractionCommuters) const;

        /// Gets the number of people infected in the contactpools at this location.
        unsigned int GetInfectedCount() const;

        /// Calculates number of outgoing commuters, given the fraction of the population that commutes.
        unsigned int GetOutgoingCommuteCount(double fractionCommuters) const;

        /// Get the last contactPool ID for a given contactType
        unsigned int GetContactPoolId(stride::ContactType::Id id);

public:
        /// Access through const reference to ContactPools of type 'id'.
        /// \param id   ContactType::Id of pools container you want to access.
        /// \return     The requested reference.
        const stride::util::SegmentedVector<stride::ContactPool*>& CRefPools(stride::ContactType::Id id) const
        {
                return m_pool_index[id];
        }

        /// Templated version of @CRefPools for use when the type id is fixed
        /// \tparam T   ContactType::Id of pools container you want to access.
        /// \return     The requested reference.
        template <stride::ContactType::Id T>
        const stride::util::SegmentedVector<stride::ContactPool*>& CRefPools() const
        {
                return m_pool_index[T];
        }

        /// Access through reference to ContactPools of type 'id'.
        /// \param id   ContactType::Id of pools container you want to access.
        /// \return     The requested reference.
        stride::util::SegmentedVector<stride::ContactPool*>& RefPools(stride::ContactType::Id id)
        {
                return m_pool_index[id];
        }

        /// Templated version of @RefPools for use when the type id is fixed
        /// \tparam T   ContactType::Id of pools container you want to access.
        /// \return     The requested reference.
        template <stride::ContactType::Id T>
        stride::util::SegmentedVector<stride::ContactPool*>& RefPools()
        {
                return m_pool_index[T];
        }

        /// Register a ContactPool pointer in this Location's pool system.
        /// Prior to this the pool should have been created in Population's pool system.
        void RegisterPool(stride::ContactPool* p, stride::ContactType::Id typeId)
        {
                m_pool_index[typeId].emplace_back(p);
        }

        /// Templated version of @RegisterPool
        template <stride::ContactType::Id T>
        void RegisterPool(stride::ContactPool* p)
        {
                m_pool_index[T].emplace_back(p);
        }

public:
        /// References incoming commute Locations + fraction of commutes to that Location.
        const std::vector<std::pair<Location<CoordinateLike>*, double>>& CRefIncomingCommutes() const { return m_inCommutes; }

        /// References outgoing commute Locations + fraction of commutes to that Location.
        const std::vector<std::pair<Location<CoordinateLike>*, double>>& CRefOutgoingCommutes() const { return m_outCommutes; }

private:
        /// Incomming commutes stored as pair of Location and fraction of population at that Location.
        std::vector<std::pair<Location<CoordinateLike>*, double>> m_inCommutes;

        ///< Outgoing commutes stored as pair of Location and fraction of population to this this Location.
        std::vector<std::pair<Location<CoordinateLike>*, double>> m_outCommutes;

        ///< The system holding pointers to the contactpools (for each type id) at this Location.
        stride::ContactType::IdSubscriptArray<stride::util::SegmentedVector<stride::ContactPool*>> m_pool_index;

public:
        typedef CoordinateLike coordinate_type;
};
extern template class Location<Coordinate>;

} // namespace geopop
