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

#include "Populator.h"

#include "contact/AgeBrackets.h"
#include "contact/ContactPool.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "util/Assert.h"

#include <utility>
#include <algorithm>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::AgeBrackets;
using namespace util;

template<>
void Populator<stride::ContactType::Id::Workplace>::Apply(GeoGrid& geoGrid, GeoGridConfig& geoGridConfig)
{
        m_logger->trace("Starting to populate Workplaces");

        auto genCommute{function<int()>()};
        auto genNonCommute{function<int()>()};
        vector<ContactPool*> nearbyWp{};
        vector<Location*> commuteLocations{};
        vector<unsigned int> commuteLocationSize{};

        const auto participCollege      = geoGridConfig.param.participation_college;
        const auto participWorkplace    = geoGridConfig.param.participation_workplace;
        auto popCollege                 = 0;
        auto popWorkplace               = 0;
        const auto fracCollegeCommute   = geoGridConfig.param.fraction_college_commuters;
        const auto fracWorkplaceCommute = geoGridConfig.param.fraction_workplace_commuters;

        for (const auto& pair : geoGridConfig.regionInfo){
                popCollege += pair.second.popcount_college;
                popWorkplace += pair.second.popcount_workplace;
        }

        double fracCommuteStudents = 0.0;
        if (static_cast<bool>(fracWorkplaceCommute) && popWorkplace) {
                fracCommuteStudents = (popCollege * fracCollegeCommute) /(popWorkplace * fracWorkplaceCommute);
        }

        // --------------------------------------------------------------------------------
        // For every location, if populated ...
        // --------------------------------------------------------------------------------
        for (unsigned i = 0; i <geoGrid.size(); ++i) {
                const auto& loc = geoGrid[i];
                if (loc->GetPopCount() == 0) {
                        continue;
                }


                // --------------------------------------------------------------------------------
                // Find all Workplaces were employees from this location commute to
                // --------------------------------------------------------------------------------
                commuteLocations.clear();
                genCommute = function<int()>();

                vector<double> commutingWeights;
                for (const pair<Location*, double>& commute : loc->CRefOutgoingCommutes()) {
                        const auto& workplaces = commute.first->RefPools(Id::Workplace);
                        if (!workplaces.empty()) {
                                commuteLocations.push_back(commute.first);
                                const auto weight = commute.second - (commute.second * fracCommuteStudents);
                                commutingWeights.push_back(weight);
                                AssertThrow(weight >= 0.0 && weight <= 1.0 && !isnan(weight),
                                            "Invalid weight: " + to_string(weight), m_logger);
                        }
                }

                if (!commutingWeights.empty()) {
                        genCommute = m_rn_man.GetDiscreteGenerator(commutingWeights, 0U);
                }

                // --------------------------------------------------------------------------------
                // Set NearbyWorkspacePools and associated generator
                // --------------------------------------------------------------------------------
                nearbyWp      = geoGrid.GetNearbyPools(Id::Workplace, *loc);
                genNonCommute = m_rn_man.GetUniformIntGenerator(0, static_cast<int>(nearbyWp.size()), 0U);

                // --------------------------------------------------------------------------------
                // For everyone of working age: decide between work or college (iff of College age)
                // --------------------------------------------------------------------------------
                for (auto& hhPool : loc->RefPools(Id::Household)) {
                        for (auto person : *hhPool) {
                                if (!Workplace::HasAge(person->GetAge())) {
                                        continue;
                                }

                                bool isStudent      = m_rn_man.MakeWeightedCoinFlip(participCollege);
                                bool isActiveWorker = m_rn_man.MakeWeightedCoinFlip(participWorkplace);

                                if ((College::HasAge(person->GetAge()) && !isStudent) || isActiveWorker) {
                                        // ---------------------------------------------
                                        // this person is employed
                                        // ---------------------------------------------
                                        const auto isCommuter = m_rn_man.MakeWeightedCoinFlip(fracWorkplaceCommute);
                                        if (!commuteLocations.empty() && isCommuter) {
                                            // --------------------------------------------------------------
                                            // this person commutes to the Location and in particular to Pool
                                            // --------------------------------------------------------------
                                            auto &pools = commuteLocations[genCommute()]->RefPools(Id::Workplace);
                                            auto s = static_cast<int>(pools.size());
                                            auto gen = m_rn_man.GetUniformIntGenerator(0, s);

                                            bool success = false;
                                            for (int i = 0; i < s; i ++) {
                                                auto pool = pools[gen()];

                                                if (geoGridConfig.wpPoolSizes[pool->GetId()] == 0){
                                                    continue;
                                                }

                                                // so that's it
                                                pool->AddMember(person);
                                                person->SetPoolId(Id::Workplace, pool->GetId());
                                                geoGridConfig.wpPoolSizes[pool->GetId()] -= 1;

                                                success = true;
                                                break;
                                            }

                                            if(not success){
                                                const auto& pool = pools[gen()];
                                                pool->AddMember(person);
                                                person->SetPoolId(Id::Workplace, pool->GetId());
                                            }
                                        } else {
                                            // ----------------------------
                                            // this person does not commute
                                            // ----------------------------

                                            bool success = false;
                                            for (unsigned int i = 0; i < nearbyWp.size(); i ++){
                                                const auto& pool = nearbyWp[genNonCommute()];

                                                if (geoGridConfig.wpPoolSizes[pool->GetId()] == 0){
                                                    continue;
                                                }

                                                pool->AddMember(person);
                                                person->SetPoolId(Id::Workplace, pool->GetId());
                                                geoGridConfig.wpPoolSizes[pool->GetId()] -= 1;

                                                success = true;
                                                break;
                                            }

                                            if(not success){
                                                const auto& pool = nearbyWp[genNonCommute()];
                                                pool->AddMember(person);
                                                person->SetPoolId(Id::Workplace, pool->GetId());
                                            }

                                        }
                                } else {
                                        // -----------------------------
                                        // this person has no employment
                                        // -----------------------------
                                        person->SetPoolId(Id::Workplace, 0);
                                }
                        }
                }
        }

        m_logger->trace("Done populating Workplaces");
}

} // namespace geopop
