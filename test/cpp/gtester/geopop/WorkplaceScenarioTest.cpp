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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#include "geopop/generators/Generator.h"
#include "geopop/populators/Populator.h"
#include "geopop/io/WorkplaceCSVReader.h"

#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "pop/Population.h"
#include "util/RnMan.h"

#include <gtest/gtest.h>
#include <tuple>
#include <geopop/GeoGridConfig.h>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

namespace {

class WorkplaceScenarioTest : public testing::Test
{
public:
    WorkplaceScenarioTest()
            : m_rn_man(RnInfo()), m_workplace_generator(m_rn_man), m_workplace_populator(m_rn_man)
            , m_gg_config(), m_pop(Population::Create()), m_geo_grid(m_pop.get())
    {
    }

protected:
    RnMan                  m_rn_man;
    WorkplaceGenerator     m_workplace_generator;
    WorkplacePopulator     m_workplace_populator;
    GeoGridConfig          m_gg_config;
    shared_ptr<Population> m_pop;
    GeoGrid                m_geo_grid;
    unsigned int           m_ppwp = m_gg_config.pools[Id::Workplace];
};

TEST_F(WorkplaceScenarioTest, WorkplaceScenarioWithDistribution)
{
    m_gg_config.param.pop_size                     = 5 * 1000 * 1000;
    m_gg_config.regionInfo[0].popcount_workplace         = static_cast<unsigned int>(0.20 * 5 * 1000 * 1000);
    m_gg_config.param.participation_workplace       = 0.20;
    m_gg_config.param.fraction_workplace_commuters = 0.10;

    string csvString =
    R"(ratio,size_min,size_max
    1, 95, 105)";

    GeoGridConfig      geoConfig{};
    auto               instream = make_unique<istringstream>(csvString);
    WorkplaceCSVReader reader(move(instream));

    reader.SetReferenceWorkplaces(m_gg_config.param.workplace_distribution);

    array<unsigned int, 50> sizes{128331, 50784,  191020, 174476, 186595, 105032, 136388, 577,   111380, 171014,
                                  63673,  49438,  45590,  164666, 185249, 141389, 82525,  40397, 123307, 168128,
                                  172937, 15581,  22891,  143505, 178516, 116959, 144659, 20775, 156009, 91951,
                                  49823,  181594, 119075, 27700,  116959, 146583, 102531, 58864, 76946,  91951,
                                  134464, 59248,  10003,  125423, 15004,  8656,   13658,  50784, 61749,  165243};
    for (const auto size : sizes) {
        m_geo_grid.AddLocation(make_shared<Location>(1, 4, Coordinate(0, 0), "Size: " + to_string(size), size));
    }

    array<tuple<size_t, size_t, double>, 6> commuting {make_tuple(0, 10, 0.23),  make_tuple(25, 3, 0.43),
                                                       make_tuple(38, 17, 0.65), make_tuple(10, 17, 0.25),
                                                       make_tuple(15, 17, 0.22), make_tuple(27, 17, 0.47)};

    for (const auto& info : commuting) {
        m_geo_grid[get<0>(info)]->AddOutgoingCommute(m_geo_grid[get<1>(info)], get<2>(info));
        m_geo_grid[get<1>(info)]->AddIncomingCommute(m_geo_grid[get<0>(info)], get<2>(info));
    }

    m_workplace_generator.Apply(m_geo_grid, m_gg_config);
    m_geo_grid.Finalize();
    m_workplace_populator.Apply(m_geo_grid, m_gg_config);

    array<unsigned int, sizes.size()> expected{228, 102, 383, 400, 374, 211, 273, 2, 223, 343,
                                               141, 99, 92, 330, 371, 252, 166, 188, 247, 337,
                                               346, 32, 46, 288, 358, 184, 290, 32, 313, 184,
                                               100, 364, 239, 56, 234, 294, 206, 118, 104, 184,
                                               269, 119, 21, 251, 31, 18, 28, 102, 124, 331};

    for (auto i = 0U; i < sizes.size(); i++) {
        EXPECT_NEAR(expected[i], m_geo_grid[i]->CRefPools(Id::Workplace).size(), expected[i]*0.05);
    }
}

} // namespace
