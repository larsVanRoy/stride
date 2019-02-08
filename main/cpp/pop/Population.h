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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header file for the core Population class
 */

#pragma once

#include "pool/ContactPoolSys.h"
#include "pop/Person.h"
#include "util/RnMan.h"
#include "util/SegmentedVector.h"

#include <boost/property_tree/ptree_fwd.hpp>
#include <memory>
#include <spdlog/spdlog.h>

namespace gengeopop {
class GeoGrid;
}

namespace stride {

/**
 * Key Data structure: container for
 * (a) all individuals in the population
 * (b) the ContactPoolSys wchich is used to loop over ContactPools of each type
 * (c) (if present) geographical grid of Locations with ContactCenters at that location.
 */
class Population : public util::SegmentedVector<Person>
{
public:
        /// Create a population initialized by the configuration in property tree.
        static std::shared_ptr<Population> Create(const boost::property_tree::ptree& configPt, util::RnMan& rnManager);

        /// For use in python environment: create using configuration string i.o ptree.
        static std::shared_ptr<Population> Create(const std::string& configString, util::RnMan& rnManager);

        /// Create an empty Population, used in gengeopop.
        static std::shared_ptr<Population> Create();

        /// Get the cumulative number of cases.
        unsigned int GetInfectedCount() const;

        ///
        std::shared_ptr<spdlog::logger>& GetContactLogger() { return m_contact_logger; }

        /// The ContactPoolSys of the simulator.
        ContactPoolSys& GetContactPoolSys() { return m_pool_sys; }

        /// The ContactPoolSys of the simulator.
        const ContactPoolSys& GetContactPoolSys() const { return m_pool_sys; }

        /// Create Person in the population.
        Person* CreatePerson(unsigned int id, double age, unsigned int householdId, unsigned int k12SchoolId,
                             unsigned int college, unsigned int workId, unsigned int primaryCommunityId,
                             unsigned int secondaryCommunityId);

        /// Add a new contact pool of a given type
        ContactPool* CreateContactPool(ContactPoolType::Id typeId);

private:
        ///
        Population() : m_pool_sys(), m_contact_logger(), m_geoGrid() {}

        friend class DefaultPopBuilder;
        friend class GenPopBuilder;
        friend class ImportPopBuilder;

private:
        ContactPoolSys                  m_pool_sys;       ///< Holds vector of ContactPools of different types.
        std::shared_ptr<spdlog::logger> m_contact_logger; ///< Logger for contact/transmission.
        std::size_t m_currentContactPoolId = 1;           ///< The current contact pool id, assigns in increasing order.
        std::shared_ptr<gengeopop::GeoGrid> m_geoGrid;    ///< Associated geoGrid may be nullptr
};

} // namespace stride
