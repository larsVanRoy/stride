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

#include "K12SchoolPopulator.h"

#include "pool/PoolConfig.h"
#include "gengeopop/GeoGrid.h"
#include "gengeopop/Household.h"
#include "gengeopop/K12School.h"
#include "gengeopop/Location.h"
#include "pop/Person.h"

#include <trng/uniform_int_dist.hpp>

namespace gengeopop {

using namespace std;
using namespace stride;

void K12SchoolPopulator::Apply(shared_ptr<GeoGrid> geoGrid, const GeoGridConfig&)
{
        m_logger->info("Starting to populate Schools");

        set<ContactPool*> found;
        unsigned int      pupils = 0;

        // for every location
        for (const shared_ptr<Location>& loc : *geoGrid) {
                if (loc->GetPopCount() == 0) {
                        continue;
                }

                // 1. find all schools in an area of 10-k*10 km
                const vector<ContactPool*>& classes = GetPoolInIncreasingRadius<K12School>(geoGrid, loc);

                auto dist = m_rnManager[0].variate_generator(
                    trng::uniform_int_dist(0, static_cast<trng::uniform_int_dist::result_type>(classes.size())));

                // 2. for every student assign a class
                for (const shared_ptr<ContactCenter>& household : loc->GetContactCentersOfType<Household>()) {
                        ContactPool* contactPool = household->GetPools()[0];
                        found.insert(contactPool);
                        for (Person* p : *contactPool) {
                                if (PoolConfig::K12School::IsOfAge(p->GetAge())) {
                                        auto& c = classes[dist()];
                                        c->AddMember(p);
                                        p->SetK12SchoolId(static_cast<unsigned int>(c->GetId()));
                                        pupils++;
                                }
                        }
                }
        }

        m_logger->info("Number of pupils in schools: {}", pupils);
        m_logger->info("Number of different classes: {}", found.size());
}

} // namespace gengeopop