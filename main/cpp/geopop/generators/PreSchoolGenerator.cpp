#include "Generator.h"

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;

template<>
void Generator<stride::ContactType::Id::PreSchool>::Apply(GeoGrid& geoGrid, GeoGridConfig& ggConfig)
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
        const auto pupilCount = pair.second.popcount_daycare;
        const auto schoolCount =
                static_cast<unsigned int>(ceil(pupilCount / static_cast<double>(ggConfig.people[Id::PreSchool])));

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

        for (auto i = 0U; i < schoolCount; i++) {
            const auto loc = geoGrid[indices[dist()]];
            AddPools(*loc, pop, ggConfig);
        }
    }
}

} // namespace geopop
