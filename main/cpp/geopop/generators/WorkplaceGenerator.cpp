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

#include <geopop/GeoGridConfig.h>
#include "Generator.h"

#include "util/Assert.h"
#include "util/RnMan.h"

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

template<>
void Generator<stride::ContactType::Id::Workplace>::Apply(GeoGrid& geoGrid, GeoGridConfig& ggConfig)
{
        // initiate workplaces using given distributions
        if (!ggConfig.param.workplace_distribution.empty())
        {
                // 1. get the active people count for a location
                // 2. loop and generate workplaces
                //      2.1 generate a random number, and derive the workplace size based on this number
                //      2.2 generate a random workplace size that is within the min and max boundaries
                //      2.3 generate a pool and add the correct size to the GeoGridConfig object
                //      2.4 reduce the total number of available workers and continue

                auto uniform01Generator = m_rn_man.GetUniform01Generator(0U);
                for (const auto &loc : geoGrid) {
                        int workers =
                                static_cast<unsigned int>(floor(loc->GetPopCount() *
                                                                ggConfig.param.participation_workplace +
                                                                loc->GetIncomingCommuteCount(
                                                                        ggConfig.param.fraction_workplace_commuters) -
                                                                loc->GetOutgoingCommuteCount(
                                                                        ggConfig.param.fraction_workplace_commuters)));

                        while (workers > 0) {
                                // this variable is used to determine the size of the next workplace pool
                                double workplaceType = uniform01Generator();

                                // these variables represent the boundaries of the chosen pool size
                                unsigned int minSize = 0;
                                unsigned int maxSize = 0;
                                for (const std::vector<double> &workplaceSize: ggConfig.param.workplace_distribution) {
                                        workplaceType -= workplaceSize[0];
                                        if (workplaceType <= 0) {
                                                minSize = static_cast<unsigned int>(workplaceSize[1]);
                                                maxSize = static_cast<unsigned int>(workplaceSize[2]);
                                                break;
                                        }
                                }

                                // if this code is reached, the percentages given in the config file did not add up to 1
                                if (workplaceType > 0) {
                                        minSize = static_cast<unsigned int>(ggConfig.param.workplace_distribution.back()[1]);
                                        maxSize = static_cast<unsigned int>(ggConfig.param.workplace_distribution.back()[2]);
                                }

                                unsigned int boundary = maxSize - minSize;
                                unsigned int workplaceSize =
                                        minSize + static_cast<unsigned int>(floor(uniform01Generator() * boundary));

                                workers -= workplaceSize;
                                AddPools(*loc, geoGrid.GetPopulation(), ggConfig);
                                const auto& pool_id = loc->GetContactPoolId(stride::ContactType::Id::Workplace);
                                ggConfig.wpPoolSizes[pool_id] = workplaceSize;
                        }
                }
        }

        // initiate workplaces without given distributions
        else{
                // store the indices of the locations that are part of the key (which is a region id)
                std::map<unsigned int, vector<unsigned int>> loc_indices;

                // initialise the loc_indices map, so that every specified region has its required vector initialised
                for (const auto& pair : ggConfig.regionInfo){
                        loc_indices[pair.first] = {};
                }

                // store the Location indices in their correct spot in the loc_indices map
                std::vector<unsigned int> provinces = {1, 2, 3, 4, 7};
                for (const unsigned int& region : provinces){
                        if(loc_indices.find(region) != loc_indices.end()){
                                loc_indices[region] = geoGrid.get_L_for_P(region);
                        }
                        else{
                                auto new_indices = geoGrid.get_L_for_P(region);
                                loc_indices[0].insert(loc_indices[0].begin(), new_indices.begin(), new_indices.end());
                        }
                }

                // 1. active people count and the commuting people count are given
                // 2. count the workplaces
                // 3. count the working people at each location = #residents + #incoming commuters - #outgoing commuters
                // 4. use that information for the distribution
                // 5. assign each workplaces to a location
                for (const auto& pair : ggConfig.regionInfo){
                        const auto EmployeeCount = pair.second.popcount_workplace;
                        const auto WorkplacesCount =
                                static_cast<unsigned int>(ceil(EmployeeCount /
                                static_cast<double>(ggConfig.people[Id::Workplace])));

                        // = for each location #residents + #incoming commuting people - #outgoing commuting people
                        vector<double> weights;
                        for (const auto& index : loc_indices[pair.first]) {
                                const auto& loc = geoGrid[index];
                                const double ActivePeopleCount =
                                        (loc->GetPopCount() +
                                         loc->GetIncomingCommuteCount(ggConfig.param.fraction_workplace_commuters) -
                                         loc->GetOutgoingCommuteCount(ggConfig.param.fraction_workplace_commuters) *
                                         ggConfig.param.participation_workplace);

                                const double weight = ActivePeopleCount / EmployeeCount;
                                AssertThrow(weight >= 0 && weight <= 1 && !std::isnan(weight),
                                            "Invalid weight: " + to_string(weight),
                                            m_logger);
                                weights.push_back(weight);
                        }

                        if (weights.empty()) {
                                // trng can't handle empty vectors
                                return;
                        }

                        const auto dist = m_rn_man.GetDiscreteGenerator(weights, 0U);
                        auto       pop  = geoGrid.GetPopulation();
                        const auto& indices = loc_indices[pair.first];

                        for (auto i = 0U; i < WorkplacesCount; i++) {
                                const auto loc = geoGrid[indices[dist()]];
                                AddPools(*loc, pop, ggConfig);
                                const auto& pool_id = loc->GetContactPoolId(stride::ContactType::Id::Workplace);
                                ggConfig.wpPoolSizes[pool_id] = 0;
                        }
                }
        }
}

} // namespace geopop
