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
 *  Copyright 2019, Jan Broeckhove.
 */

#include "geopop/generators/Generator.h"

#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "pop/Population.h"
#include "util/RnMan.h"

#include <gtest/gtest.h>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

namespace {

class DaycareGeneratorTest : public testing::Test
{
public:
        DaycareGeneratorTest()
            : m_rn_man(RnInfo()), m_daycare_generator(m_rn_man), m_gg_config(), m_pop(Population::Create()),
              m_geo_grid(m_pop.get())
        {
        }

protected:
        RnMan                  m_rn_man;
        DaycareGenerator       m_daycare_generator;
        GeoGridConfig          m_gg_config;
        shared_ptr<Population> m_pop;
        GeoGrid                m_geo_grid;
        unsigned int           m_ppd = m_gg_config.pools[Id::Daycare];
};

// Checks whether generator can handle a single location.
TEST_F(DaycareGeneratorTest, OneLocationTest)
{
        m_gg_config.param.pop_size                   = 10000;
        m_gg_config.regionInfo[0].popcount_daycare = 2000;

        auto loc1 = make_shared<Location<Coordinate>>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);
        m_geo_grid.AddLocation(loc1);

        m_daycare_generator.Apply(m_geo_grid, m_gg_config);

        const auto& poolsOfLoc1 = loc1->CRefPools(Id::Daycare);
        EXPECT_EQ(poolsOfLoc1.size(), 223 * m_ppd);
}

// Checks whether Generator can handle zero locations in GeoGrid.
TEST_F(DaycareGeneratorTest, ZeroLocationTest)
{
        m_gg_config.param.pop_size                   = 10000;
        m_gg_config.regionInfo[0].popcount_daycare = 2000;

        m_daycare_generator.Apply(m_geo_grid, m_gg_config);

        EXPECT_EQ(m_geo_grid.size(), 0);
}

// Checks whether generator can handle multiple locations.
TEST_F(DaycareGeneratorTest, MultipleLocationsTest)
{
        m_gg_config.param.pop_size                   = 37542 * 100;
        m_gg_config.regionInfo[0].popcount_daycare = 750840;

        auto loc1 = make_shared<Location<Coordinate>>(1, 4, Coordinate(0, 0), "Antwerpen", 10150 * 100);
        auto loc2 = make_shared<Location<Coordinate>>(1, 4, Coordinate(0, 0), "Vlaams-Brabant", 10040 * 100);
        auto loc3 = make_shared<Location<Coordinate>>(1, 4, Coordinate(0, 0), "Henegouwen", 7460 * 100);
        auto loc4 = make_shared<Location<Coordinate>>(1, 4, Coordinate(0, 0), "Limburg", 3269 * 100);
        auto loc5 = make_shared<Location<Coordinate>>(1, 4, Coordinate(0, 0), "Luxemburg", 4123 * 100);

        m_geo_grid.AddLocation(loc1);
        m_geo_grid.AddLocation(loc2);
        m_geo_grid.AddLocation(loc3);
        m_geo_grid.AddLocation(loc4);
        m_geo_grid.AddLocation(loc5);

        for (unsigned i = 0; i < m_geo_grid.size(); ++i) {
                const auto& loc = m_geo_grid[i];
                loc->SetPopFraction(static_cast<double>(loc->GetPopCount()) /
                                    static_cast<double>(m_gg_config.param.pop_size));
        }

        m_daycare_generator.Apply(m_geo_grid, m_gg_config);

        array<unsigned int, 5> sizes{24254, 23844, 17652, 7778, 9899};
        for (auto i = 0U; i < sizes.size(); i++) {
                EXPECT_EQ(sizes[i] * m_ppd, m_geo_grid[i]->CRefPools(Id::Daycare).size());
        }
}

} // namespace
