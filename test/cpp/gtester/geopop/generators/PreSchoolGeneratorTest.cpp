///*
// *  This is free software: you can redistribute it and/or modify it
// *  under the terms of the GNU General Public License as published by
// *  the Free Software Foundation, either version 3 of the License, or
// *  any later version.
// *  The software is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// *  GNU General Public License for more details.
// *  You should have received a copy of the GNU General Public License
// *  along with the software. If not, see <http://www.gnu.org/licenses/>.
// *
// *  Copyright 2019, Jan Broeckhove.
// */
//
//#include "geopop/generators/Generator.h"
//
//#include "geopop/GeoGrid.h"
//#include "geopop/GeoGridConfig.h"
//#include "geopop/Location.h"
//#include "pop/Population.h"
//#include "util/RnMan.h"
//
//#include <gtest/gtest.h>
//
//using namespace std;
//using namespace geopop;
//using namespace stride;
//using namespace stride::ContactType;
//using namespace stride::util;
//
//namespace {
//
//class PreSchoolGeneratorTest : public testing::Test
//{
//public:
//        PreSchoolGeneratorTest()
//            : m_rn_man(RnInfo()), m_preschool_generator(m_rn_man), m_gg_config(), m_pop(Population::Create()),
//              m_geo_grid(m_pop.get())
//        {
//        }
//
//protected:
//        RnMan                  m_rn_man;
//        PreSchoolGenerator     m_preschool_generator;
//        GeoGridConfig          m_gg_config;
//        shared_ptr<Population> m_pop;
//        GeoGrid                m_geo_grid;
//        unsigned int           m_ppps = m_gg_config.pools[Id::PreSchool];
//};
//
//// Checks whether generator can handle a single location.
//TEST_F(PreSchoolGeneratorTest, OneLocationTest)
//{
//        m_gg_config.param.pop_size          = 45000;
//        m_gg_config.info.popcount_preschool = 9000;
//
//        auto loc1 = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", m_gg_config.param.pop_size);
//        m_geo_grid.AddLocation(loc1);
//
//        m_preschool_generator.Apply(m_geo_grid, m_gg_config);
//
//        const auto& poolsOfLoc1 = loc1->CRefPools<Id::PreSchool>();
//        EXPECT_EQ(poolsOfLoc1.size(), 3 * m_ppps);
//}
//
//// Checks whether Generator can handle zero locations in GeoGrid.
//TEST_F(PreSchoolGeneratorTest, ZeroLocationTest)
//{
//        m_gg_config.param.pop_size        = 10000;
//        m_gg_config.info.popcount_college = 2000;
//
//        m_preschool_generator.Apply(m_geo_grid, m_gg_config);
//
//        EXPECT_EQ(m_geo_grid.size(), 0);
//}
//
//// Checks whether generator can handle multiple locations.
//TEST_F(PreSchoolGeneratorTest, MultipleLocationsTest)
//{
//        m_gg_config.param.pop_size          = 399992;
//        m_gg_config.info.popcount_preschool = 79998;
//
//        vector<int> sizes{28559, 33319, 39323, 37755, 35050, 10060, 13468, 8384,
//                          9033,  31426, 33860, 4110,  50412, 25098, 40135};
//        for (int size : sizes) {
//                const auto loc = make_shared<Location>(1, 4, Coordinate(0, 0), "Size: " + to_string(size), size);
//                m_geo_grid.AddLocation(loc);
//        }
//        m_preschool_generator.Apply(m_geo_grid, m_gg_config);
//
//        vector<int> expected{2, 2, 5, 2, 3, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3};
//        for (size_t i = 0; i < sizes.size(); i++) {
//                EXPECT_EQ(expected[i] * m_ppps, m_geo_grid[i]->CRefPools<Id::PreSchool>().size());
//        }
//}
//
//} // namespace