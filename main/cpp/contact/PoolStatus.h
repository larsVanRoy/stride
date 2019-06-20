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

//#include "ContactType.h"
#include "AgeBrackets.h"
#include "disease/Health.h"

#include <vector>
#include <map>
#include <set>
#include <memory>

namespace stride {

class HealthPool;

/// Stores the % of people of each health status for every ContactType
class PoolStatus {
public:
    PoolStatus();

    bool operator==(const PoolStatus& other) const;

    /// Adds a status (vector of percentages) for an agebracket
    void addStatus(const AgeBrackets::AgeBracket&, std::shared_ptr<HealthPool> status);

    /// Returns the status of a ContactType
    std::shared_ptr<HealthPool> getStatus(const AgeBrackets::AgeBracket& ageBracket);

    std::shared_ptr<HealthPool> operator[](const AgeBrackets::AgeBracket& ageBracket);

    const std::shared_ptr<const HealthPool> getStatus(const AgeBrackets::AgeBracket& ageBracket) const;

    const std::shared_ptr<const HealthPool> operator[](const AgeBrackets::AgeBracket& ageBracket) const;

    std::vector<double> operator[](HealthStatus h) const;

    using iterator          = std::vector<std::shared_ptr<HealthPool>>::iterator;
    using const_iterator    = std::vector<std::shared_ptr<HealthPool>>::const_iterator;

    iterator begin() { return m_status.begin(); }

    iterator end() { return m_status.end(); }

    const_iterator cbegin() const { return m_status.cbegin(); }

    const_iterator cend() const { return m_status.cend(); }

    double getPercentage(const AgeBrackets::AgeBracket& ageBracket) const;

    double getPercentage(const AgeBrackets::AgeBracket& ageBracket, const std::set<HealthStatus>& ID) const;

private:
    std::vector<std::shared_ptr<HealthPool>> m_status;   ///< matrix of the health status of a ContactType m_status[ContactType::Id][HealthStatus]
};


/// container to hold the fractions of health per HealthStatus
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

    double getHealth(const HealthStatus& ID) const;

    double operator[](const HealthStatus& ID) const { return getHealth(ID); };

    bool operator==(const HealthPool& other) const;

    /// Sum of fractions of all given ID
    double sum(const std::set<HealthStatus>& ID) const;

    /// returns size of HealthPool
    static unsigned size() { return 7; };

    std::vector<double> toVector() { return {m_susceptible, m_exposed, m_infectious, m_symptomatic,
                                            m_infectiousAndSympomatic, m_recovered, m_immune}; }
};
}   // namespace stride