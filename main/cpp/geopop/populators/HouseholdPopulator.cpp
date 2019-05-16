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

#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "pop/Population.h"

namespace geopop {

using namespace std;
using namespace stride::ContactType;

template<>
void Populator<stride::ContactType::Id::Household>::Apply(GeoGrid& geoGrid, GeoGridConfig& geoGridConfig)
{
        m_logger->trace("Starting to populate Households");

        auto person_id                    = 0U;
        auto pop                          = geoGrid.GetPopulation();

        GeoGridConfig::refHH HHReference{};

        for (const shared_ptr<Location>& loc : geoGrid) {
                if(geoGridConfig.refHHs.find(loc->GetProvince()) != geoGridConfig.refHHs.end()){
                        HHReference = geoGridConfig.refHHs[loc->GetProvince()];
                } else{
                        HHReference = geoGridConfig.refHHs[0];
                }
                if (loc->GetPopCount() == 0) {
                        continue;
                }
                auto hh_dist   = m_rn_man.GetUniformIntGenerator(0, static_cast<int>(HHReference.ages.size()), 0U);
                for (auto& pool : loc->RefPools(Id::Household)) {
                        const auto hDraw = static_cast<unsigned int>(hh_dist());
                        for (const auto& age : HHReference.ages[hDraw]) {
                                const auto p = pop->CreatePerson(person_id++, age, 0, 0, pool->GetId(), 0, 0, 0, 0, 0);
                                pool->AddMember(p);
                        }
                }
        }

        m_logger->trace("Done populating Households");
}

} // namespace geopop
