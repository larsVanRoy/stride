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

    // 1. given the number of persons of school age, calculate number of schools
    // 2. assign schools to a location by using a discrete distribution which reflects the
    //    relative number of pupils for that location; the relative number of pupils is set
    //    to the relative population w.r.t the total population.
    for (const auto& pair : ggConfig.regionInfo) {
        const auto pupilCount = pair.second.popcount_preschool;
        const auto schoolCount =
                static_cast<unsigned int>(ceil(pupilCount / static_cast<double>(ggConfig.people[Id::PreSchool])));

        vector<double> weights;
        for (const auto &index : loc_indices[pair.first]) {
            weights.push_back(geoGrid[index]->GetPopFraction());
        }

        if (weights.empty()) {
            // trng can't handle empty vectors
            continue;
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
