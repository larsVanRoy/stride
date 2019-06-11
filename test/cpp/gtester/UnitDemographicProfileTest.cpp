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
    Daycare toddler count:                           857
    PreSchool student count:                         1.782
    K12School student count:                         10.176
    College student count:                           7.744
    Workplace person count:                          18.440

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
    Daycare toddler count:                           857
    PreSchool student count:                         1.782
    K12School student count:                         10.176
    College student count:                           7.744
    Workplace person count:                          18.440

for region:                                          Antwerp
    Daycare toddler count:                           407
    PreSchool student count:                         984
    K12School student count:                         5.174
    College student count:                           3.781
    Workplace person count:                          9.139

)";
    shared_ptr<Population> m_pop(Population::Create());
    GeoGrid m_geo_grid(m_pop.get());

    auto loc1 = make_shared<Location>(1, 1, Coordinate(0, 0), "Antwerpen", 30000);
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
    Daycare toddler count:                           857
    PreSchool student count:                         1.782
    K12School student count:                         10.176
    College student count:                           7.744
    Workplace person count:                          18.440

for region:                                          Major Cities
    Daycare toddler count:                           397
    PreSchool student count:                         1.014
    K12School student count:                         5.106
    College student count:                           3.838
    Workplace person count:                          9.226

)";
    shared_ptr<Population> m_pop(Population::Create());
    GeoGrid m_geo_grid(m_pop.get());

    auto loc1 = make_shared<Location>(1, 1, Coordinate(0, 0), "ANTWERPEN", 30000);
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
