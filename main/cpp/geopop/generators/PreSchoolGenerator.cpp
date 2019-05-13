#include "Generator.h"

namespace geopop {

    using namespace std;
    using namespace stride;
    using namespace stride::ContactType;

    template<>
    void Generator<stride::ContactType::Id::PreSchool>::Apply(GeoGrid& geoGrid, GeoGridConfig& ggConfig)
    {
        // 1. given the number of persons of school age, calculate number of schools
        // 2. assign schools to a location by using a discrete distribution which reflects the
        //    relative number of pupils for that location; the relative number of pupils is set
        //    to the relative population w.r.t the total population.

        const auto pupilCount = ggConfig.info.popcount_daycare;
        const auto schoolCount =
                static_cast<unsigned int>(ceil(pupilCount / static_cast<double>(ggConfig.people[Id::PreSchool])));

        vector<double> weights;
        for (const auto& loc : geoGrid) {
            weights.push_back(loc->GetPopFraction());
        }

        if (weights.empty()) {
            // trng can't handle empty vectors
            return;
        }

        const auto dist = m_rn_man.GetDiscreteGenerator(weights, 0U);
        auto       pop  = geoGrid.GetPopulation();

        for (auto i = 0U; i < schoolCount; i++) {
            const auto loc = geoGrid[dist()];
            AddPools(*loc, pop, ggConfig);
        }
    }

} // namespace geopop
