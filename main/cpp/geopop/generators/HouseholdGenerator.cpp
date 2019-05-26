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
void Generator<stride::ContactType::Id::Household>::Apply(GeoGrid& geoGrid, GeoGridConfig& ggConfig)
{
        // store the indices of the locations that are part of the key (which is a region id)
        std::map<unsigned int, vector<unsigned int>> loc_indices;

        bool majorCitySpecified = false;

        // initialise the loc_indices map, so that every specified region has its required vector initialised
        for (const auto& pair : ggConfig.regionInfo){
                loc_indices[pair.first] = {};
                if (pair.first == 11){
                        majorCitySpecified = true;
                }
        }

        // store the Location indices in their correct spot in the loc_indices map
        for (size_t i = 0; i < geoGrid.size(); i++){
                const auto& region = geoGrid[i]->GetProvince();
                if(majorCitySpecified and
                   find(ggConfig.majorCities.begin(), ggConfig.majorCities.end(), geoGrid[i]->GetName()) !=
                   ggConfig.majorCities.end()){
                        loc_indices[11].emplace_back(i);
                }
                else if(loc_indices.find(region) != loc_indices.end()){
                        loc_indices[region].emplace_back(i);
                }
                else{
                        loc_indices[0].emplace_back(i);
                }
        }

        // iterate over the different region in the weights map
        for(const auto& pair : loc_indices){
                vector<double> weights;
                for (const auto& index : loc_indices[pair.first]) {
                        weights.push_back(geoGrid[index]->GetPopFraction());
                }

                // trng can't handle empty vectors
                if (weights.empty()) {
                        continue;
                }

                const auto  dist = m_rn_man.GetDiscreteGenerator(weights, 0U);
                auto        pop  = geoGrid.GetPopulation();
                const auto& indices = loc_indices[pair.first];

                // create pools, the amount is derived from the numbers generated in GeoGridConfig::SetData
                for (auto i = 0U; i < ggConfig.regionInfo[pair.first].count_households; i++) {
                        const auto loc = geoGrid[indices[dist()]];
                        AddPools(*loc, pop, ggConfig);
                }
        }
}

} // namespace geopop
