//
// Created by lars on 28-3-19.
//

/**
 * @file
 * Unit tests of Demographic Profiles.
 */

#include "geopop/GeoGridConfig.h"
#include "geopop/GeoGrid.h"
#include "util/FileSys.h"
#include "pop/Population.h"

#include <gtest/gtest.h>
#include <sstream>
#include <geopop/GeoGridConfig.h>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::util;

TEST(UnitDemographicProfile, onlyDefaultConfiguration){
    string expectedData =
    R"(Input:
Fraction college commuters:                          0
Fraction workplace commuters:                        0
Participation fraction of daycare:                   0.45
Participation fraction of preschool:                 0.99
Participation fraction of college:                   0.7
Participation fraction of workplace:                 0.7
Number of major cities                               0
Target population size                               60.000

Calculated:
for region:                                          Default
    Daycare toddler count:                           846
    PreSchool student count:                         1.871
    K12School student count:                         7.721
    College student count:                           3.314
    Workplace person count:                          23.065

)";

    auto ggConfig = GeoGridConfig();
    ggConfig.param.pop_size = 60000;
    ggConfig.param.participation_workplace = 0.7;
    ggConfig.param.participation_college = 0.7;
    ggConfig.SetData("households_general.csv");
    stringstream ss;
    ss << ggConfig;

    EXPECT_EQ(expectedData, ss.str());
}

TEST(UnitDemographicProfile, oneAdditionalProvince){
    string expectedData =
            R"(Input:
Fraction college commuters:                          0
Fraction workplace commuters:                        0
Participation fraction of daycare:                   0.45
Participation fraction of preschool:                 0.99
Participation fraction of college:                   0.7
Participation fraction of workplace:                 0.7
Number of major cities                               0
Target population size                               60.000

Calculated:
for region:                                          Default
    Daycare toddler count:                           846
    PreSchool student count:                         1.871
    K12School student count:                         7.721
    College student count:                           3.314
    Workplace person count:                          23.065

for region:                                          Antwerp
    Daycare toddler count:                           416
    PreSchool student count:                         895
    K12School student count:                         3.849
    College student count:                           1.728
    Workplace person count:                          11.611

)";
    shared_ptr<Population> m_pop(Population::Create());
    GeoGrid m_geo_grid(m_pop.get());

    auto loc1 = make_shared<Location<Coordinate>>(1, 1, Coordinate(0, 0), "Antwerpen", 30000);
    m_geo_grid.AddLocation(loc1);

    auto ggConfig = GeoGridConfig();
    ggConfig.param.pop_size = 60000;
    ggConfig.param.participation_workplace = 0.7;
    ggConfig.param.participation_college = 0.7;
    ggConfig.SetData("households_general.csv");
    ggConfig.SetData(std::map<unsigned int, string>{{1, "households_Antwerp.csv"}}, m_geo_grid);
    stringstream ss;
    ss << ggConfig;

    EXPECT_EQ(expectedData, ss.str());
}

TEST(UnitDemographicProfile, additionalMajorCities){
    string expectedData =
            R"(Input:
Fraction college commuters:                          0
Fraction workplace commuters:                        0
Participation fraction of daycare:                   0.45
Participation fraction of preschool:                 0.99
Participation fraction of college:                   0.7
Participation fraction of workplace:                 0.7
Number of major cities                               13
Target population size                               60.000

Calculated:
for region:                                          Default
    Daycare toddler count:                           846
    PreSchool student count:                         1.871
    K12School student count:                         7.721
    College student count:                           3.314
    Workplace person count:                          23.065

for region:                                          Major Cities
    Daycare toddler count:                           407
    PreSchool student count:                         990
    K12School student count:                         3.914
    College student count:                           2.125
    Workplace person count:                          11.223

)";
    shared_ptr<Population> m_pop(Population::Create());
    GeoGrid m_geo_grid(m_pop.get());

    auto loc1 = make_shared<Location<Coordinate>>(1, 1, Coordinate(0, 0), "ANTWERPEN", 30000);
    m_geo_grid.AddLocation(loc1);

    auto ggConfig = geopop::GeoGridConfig();
    ggConfig.param.pop_size = 60000;
    ggConfig.param.participation_workplace = 0.7;
    ggConfig.param.participation_college = 0.7;
    ggConfig.SetMajorCitiesData("major_cities.csv");
    ggConfig.SetData("households_general.csv");
    ggConfig.SetData(std::map<unsigned int, string>{{11, "households_Major_Cities.csv"}}, m_geo_grid);
    stringstream ss;
    ss << ggConfig;

    EXPECT_EQ(expectedData, ss.str());
}
