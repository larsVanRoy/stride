//
// Created by lars on 28-3-19.
//

/**
 * @file
 * Unit tests of Demographic Profiles.
 */

#include "geopop/GeoGridConfig.h"
#include "util/FileSys.h"

#include <gtest/gtest.h>
#include <sstream>
#include <geopop/GeoGridConfig.h>

using namespace std;
using namespace stride;
using namespace stride::util;

TEST(DemographicProfile, onlyDefaultConfiguration){
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

    auto ggConfig = geopop::GeoGridConfig();
    ggConfig.param.pop_size = 60000;
    ggConfig.param.participation_workplace = 0.7;
    ggConfig.param.participation_college = 0.7;
    ggConfig.SetData("households_general.csv");
    stringstream ss;
    ss << ggConfig;

    EXPECT_EQ(expectedData, ss.str());
}

TEST(DemographicProfile, oneAdditionalProvince){
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
    Daycare toddler count:                           815
    PreSchool student count:                         1.969
    K12School student count:                         10.348
    College student count:                           7.563
    Workplace person count:                          18.279

)";

    auto ggConfig = geopop::GeoGridConfig();
    ggConfig.param.pop_size = 60000;
    ggConfig.param.participation_workplace = 0.7;
    ggConfig.param.participation_college = 0.7;
    ggConfig.SetData("households_general.csv");
    ggConfig.SetData(std::map<unsigned int, string>{{1, "households_Antwerp.csv"}});
    stringstream ss;
    ss << ggConfig;

    EXPECT_EQ(expectedData, ss.str());
}

TEST(DemographicProfile, additionalMajorCities){
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
    Daycare toddler count:                           795
    PreSchool student count:                         2.029
    K12School student count:                         10.212
    College student count:                           7.676
    Workplace person count:                          18.453

)";

    auto ggConfig = geopop::GeoGridConfig();
    ggConfig.param.pop_size = 60000;
    ggConfig.param.participation_workplace = 0.7;
    ggConfig.param.participation_college = 0.7;
    ggConfig.SetData("households_general.csv");
    ggConfig.SetData(std::map<unsigned int, string>{{11, "households_Major_Cities.csv"}});
    ggConfig.SetMajorCitiesData("major_cities.csv");
    stringstream ss;
    ss << ggConfig;

    EXPECT_EQ(expectedData, ss.str());
}
