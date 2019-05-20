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

#include "PoolStatus.h"

#include <vector>
#include <map>
#include <memory>

namespace stride {

//bool PoolStatus::operator==(const PoolStatus &other) {
//    for (stride::ContactType::Id ID : stride::ContactType::IdList) {
//        const auto v1 = m_status[ID];
//        const auto v2 = other.m_status[ID];
//        if(v1 != v2)
//            return false;
//    }
//    return true;
//}

void PoolStatus::addStatus(stride::ContactType::Id ID, std::shared_ptr<HealthPool> status) {
    m_status[ContactType::ToSizeT(ID)] = status;
}

void HealthPool::setHealth(HealthStatus ID, double fraction) {

    switch (ID) {
        case HealthStatus::Susceptible: {
            m_susceptible = fraction;
            break;
        }
        case HealthStatus::Exposed: {
            m_exposed = fraction;
            break;
        }
        case HealthStatus::Infectious: {
            m_infectious = fraction;
            break;
        }
        case HealthStatus::Symptomatic: {
            m_symptomatic = fraction;
            break;
        }
        case HealthStatus::InfectiousAndSymptomatic: {
            m_infectiousAndSympomatic = fraction;
            break;
        }
        case HealthStatus::Recovered: {
            m_recovered = fraction;
            break;
        }
        case HealthStatus::Immune: {
            m_immune = fraction;
            break;
        }
    }
}

double HealthPool::getHealth(HealthStatus ID) {
    switch (ID) {
        case HealthStatus::Susceptible:
            return m_susceptible;
        case HealthStatus::Exposed:
            return m_exposed;
        case HealthStatus::Infectious:
            return m_infectious;
        case HealthStatus::Symptomatic:
            return m_symptomatic;
        case HealthStatus::InfectiousAndSymptomatic:
            return m_infectiousAndSympomatic;
        case HealthStatus::Recovered:
            return m_recovered;
        case HealthStatus::Immune:
            return m_immune;
        default:
            return 0;
    }
}

double HealthPool::sum(const std::vector<HealthStatus>& ID){
    double result = 0;
    for (HealthStatus i : ID){
        result += getHealth(i);
    }
    return result;
}


} //namespace stride