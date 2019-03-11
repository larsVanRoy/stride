#pragma once
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
 *  Copyright 2018, Willem L, Kuylen E, Broeckhove J
 */

/**
 * @file
 * Header for the PublicHealthAgency class.
 */

#include "util/RnMan.h"

#include <memory>

namespace stride {

class Population;

class PublicHealthAgency
{
public:
        ///
        explicit PublicHealthAgency() : m_case_detection_probability(0.0){};

        /// Execute
        void Exec(std::shared_ptr<Population> pop, util::RnMan& rnManager, unsigned short int sim_day);

        /// Initialize
        void Initialize(const double detection_probability);

private:
        /// Public Health Strategy: look for symptomatic cases and vaccinate their household
        void PerformCaseFinding(std::shared_ptr<Population> pop, util::RnMan& rnManager, unsigned short int sim_day);

        double m_case_detection_probability; ///< Detection probability of symptomatic cases
};

} // namespace stride
