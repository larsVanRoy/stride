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
#include "AgeBrackets.h"

#include <iostream>
#include <vector>
#include <map>
#include <memory>

namespace stride {

PoolStatus::PoolStatus() : m_status({}){
    m_status.resize(AgeBrackets::NumOfAgeBrackets());
}

bool PoolStatus::operator==(const PoolStatus &other) const{
    for (const AgeBrackets::AgeBracket& ageBracket : AgeBrackets::AgeBracketList) {
        const auto& v1 = m_status[AgeBrackets::ToSize(ageBracket)];
        const auto& v2 = other.m_status[AgeBrackets::ToSize(ageBracket)];
        if(v1 != v2)
            return false;
    }
    return true;
}

std::shared_ptr<HealthPool> PoolStatus::getStatus(const AgeBrackets::AgeBracket& ageBracket){
    return m_status[AgeBrackets::ToSize(ageBracket)];
}

std::shared_ptr<HealthPool> PoolStatus::operator[](const AgeBrackets::AgeBracket& ageBracket){
    return m_status[AgeBrackets::ToSize(ageBracket)];
}

const std::shared_ptr<const HealthPool> PoolStatus::getStatus(const AgeBrackets::AgeBracket& ageBracket) const{
    return m_status[AgeBrackets::ToSize(ageBracket)];
}

const std::shared_ptr<const HealthPool> PoolStatus::operator[](const AgeBrackets::AgeBracket& ageBracket) const{
    return m_status[AgeBrackets::ToSize(ageBracket)];
}

std::vector<double> PoolStatus::operator[](HealthStatus h) const {
    std::vector<double> result;
    for(const auto status : m_status){
        result.push_back(status->getHealth(h));
    }
    return result;
}

void PoolStatus::addStatus(const AgeBrackets::AgeBracket& ageBracket, std::shared_ptr<HealthPool> status) {
    m_status[AgeBrackets::ToSize(ageBracket)] = status;
}

double PoolStatus::getPercentage(const AgeBrackets::AgeBracket& ageBracket) const {
    double scale = 0;
    for(const AgeBrackets::AgeBracket& a : AgeBrackets::AgeBracketList){
        scale += getStatus(a)->sum(stride::HealthStatusList);
    }
    if(scale == 0)
        return 0;
    return this->getStatus(ageBracket)->sum(stride::HealthStatusList)/scale;
}

double PoolStatus::getPercentage(const AgeBrackets::AgeBracket& ageBracket, const std::set<HealthStatus>& ID) const {
    double scale = 0;
    for(const AgeBrackets::AgeBracket& a : AgeBrackets::AgeBracketList){
        scale += getStatus(a)->sum(stride::HealthStatusList);
    }
    if(scale == 0)
        return 0;
    return this->getStatus(ageBracket)->sum(ID)/scale;
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

double HealthPool::getHealth(const HealthStatus& ID) const {
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

bool HealthPool::operator==(const HealthPool& other) const {
    return m_immune == other.m_immune && m_recovered == other.m_recovered &&
    m_infectiousAndSympomatic == other.m_infectiousAndSympomatic &&
    m_infectious == other.m_infectious && m_symptomatic == other.m_symptomatic &&
    m_exposed == other.m_exposed && m_susceptible == other.m_susceptible;
}

double HealthPool::sum(const std::set<HealthStatus>& ID) const {
    double result = 0;
    for (const HealthStatus& i : ID){
        result += getHealth(i);
    }
    return result;
}


} //namespace stride