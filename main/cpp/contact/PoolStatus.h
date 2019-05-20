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
 * Header for PoolStatus
 */

#pragma once

#include "ContactType.h"
#include "disease/Health.h"

#include <vector>
#include <map>
#include <memory>

namespace stride {

class HealthPool;

/// Stores the % of people of each health status for every ContactType
class PoolStatus {
public:
    PoolStatus() = default;

//    bool operator==(const PoolStatus& other);

    /// Adds a status (vector of percentages) for a ContactType
    void addStatus(stride::ContactType::Id ID, std::shared_ptr<std::vector<double>> status);

    /// Returns the status of a ContactType
    std::shared_ptr<std::vector<double>> getStatus(stride::ContactType::Id ID) { return m_status[ID]; }

    using iterator          = std::vector<std::shared_ptr<std::vector<double>>>::iterator;
    using const_iterator    = std::vector<std::shared_ptr<std::vector<double>>>::const_iterator;

    iterator begin() { return m_status.begin(); }

    iterator end() { return m_status.end(); }

    const_iterator cbegin() { return m_status.cbegin(); }

    const_iterator cend() { return m_status.cend(); }

private:
    std::vector<std::shared_ptr<std::vector<double>>> m_status;   ///< matrix of the health status of a ContactType m_status[ContactType::Id][HealthStatus]
};

class HealthPool {
private:
    double m_susceptible;
    double m_exposed;
    double m_infectious;
    double m_symptomatic;
    double m_infectiousAndSympomatic;
    double m_recovered;
    double m_immune;

public:
    HealthPool() : m_susceptible(0),
                   m_exposed(0), m_infectious(0),
                   m_symptomatic(0), m_infectiousAndSympomatic(0),
                   m_recovered(0), m_immune(0) {};

    void setHealth(HealthStatus ID, double fraction);

    double getHealth(HealthStatus ID);

    double sum(const std::vector<HealthStatus>& ID);
};

}   // namespace stride