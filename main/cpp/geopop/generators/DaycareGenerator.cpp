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

#include "Generator.h"

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;

template<>
void Generator<stride::ContactType::Id::Daycare>::Apply(GeoGrid& geoGrid, GeoGridConfig& ggConfig)
{
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

    // 1. given the number of persons of school age, calculate number of schools
    // 2. assign schools to a location by using a discrete distribution which reflects the
    //    relative number of pupils for that location; the relative number of pupils is set
    //    to the relative population w.r.t the total population.
    for (const auto& pair : ggConfig.regionInfo) {
        const auto toddlerCount = pair.second.popcount_daycare;
        const auto daycareCount =
                static_cast<unsigned int>(ceil(toddlerCount / static_cast<double>(ggConfig.people[Id::Daycare])));

        vector<double> weights;
        for (const auto &index : loc_indices[pair.first]) {
            weights.push_back(geoGrid[index]->GetPopFraction());
        }

        if (weights.empty()) {
            // trng can't handle empty vectors
            return;
        }

        const auto dist = m_rn_man.GetDiscreteGenerator(weights, 0U);
        auto pop = geoGrid.GetPopulation();
        const auto &indices = loc_indices[pair.first];

        for (auto i = 0U; i < daycareCount; i++) {
            const auto loc = geoGrid[indices[dist()]];
            AddPools(*loc, pop, ggConfig);
        }
    }
}

} // namespace geopop
