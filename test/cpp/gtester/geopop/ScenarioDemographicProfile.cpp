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

#include "geopop/GeoGridConfig.h"
#include "geopop/generators/Generator.h"
#include "geopop/io/HouseholdCSVReader.h"
#include "geopop/io/ReaderFactory.h"
#include "pop/Population.h"
#include "util/RnMan.h"

#include <gtest/gtest.h>
#include <memory>
#include <iostream>
#include <geopop/GeoGridConfig.h>
#include <math.h>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

class ScenarioDemographicProfileTest : public testing::Test
{
public:
    ScenarioDemographicProfileTest()
            : m_rn_man(RnInfo()), m_gg_config(), m_pop(Population::Create()), m_geo_grid(m_pop.get())
    {
    }

protected:
    RnMan                  m_rn_man;
    GeoGridConfig          m_gg_config;
    shared_ptr<Population> m_pop;
    GeoGrid                m_geo_grid;
};

TEST_F(ScenarioDemographicProfileTest, correctUseOfProvinces)
{
    m_gg_config.param.pop_size = 60000;
    m_gg_config.param.participation_college = 0.8;
    m_gg_config.param.participation_workplace = 0.5;

    // make it so that the default configuration is general and includes all kinds of ages ranges
    m_gg_config.SetData("households_flanders.csv");

    // make it so that the provinces only have elderly who are not able to work or go to college etc
    std::map<unsigned int, std::string> elderly_households = {
            {1, "households_elderly.csv"},
            {2, "households_elderly.csv"},
            {3, "households_elderly.csv"},
            {4, "households_elderly.csv"},
            {7, "households_elderly.csv"}
    };
    m_gg_config.SetData(elderly_households, m_geo_grid);

    // assure that this data was properly read into our geogrid configuration
    std::string expected_configuration =
    R"(Input:
Fraction college commuters:                          0
Fraction workplace commuters:                        0
Participation fraction of daycare:                   0.45
Participation fraction of preschool:                 0.99
Participation fraction of college:                   0.8
Participation fraction of workplace:                 0.5
Number of major cities                               0
Target population size                               60.000

Calculated:
for region:                                          Default
    Daycare toddler count:                           1.095
    PreSchool student count:                         2.448
    K12School student count:                         10.454
    College student count:                           5.164
    Workplace person count:                          17.137

for region:                                          Antwerp
    Daycare toddler count:                           0
    PreSchool student count:                         0
    K12School student count:                         0
    College student count:                           0
    Workplace person count:                          0

for region:                                          Flemish Brabant
    Daycare toddler count:                           0
    PreSchool student count:                         0
    K12School student count:                         0
    College student count:                           0
    Workplace person count:                          0

for region:                                          West-Flanders
    Daycare toddler count:                           0
    PreSchool student count:                         0
    K12School student count:                         0
    College student count:                           0
    Workplace person count:                          0

for region:                                          East-Flanders
    Daycare toddler count:                           0
    PreSchool student count:                         0
    K12School student count:                         0
    College student count:                           0
    Workplace person count:                          0

for region:                                          Limburg
    Daycare toddler count:                           0
    PreSchool student count:                         0
    K12School student count:                         0
    College student count:                           0
    Workplace person count:                          0

)";

    stringstream ss;
    ss << m_gg_config;

    EXPECT_EQ(expected_configuration, ss.str());

    // create all locations

    const string cities_name = "flanders_cities.csv";
    const auto locationsReader = ReaderFactory::CreateLocationsReader(cities_name);
    locationsReader->FillGeoGrid(m_geo_grid);

    for (unsigned int i = 0; i < m_geo_grid.size(); ++i) {
        m_geo_grid[i]->SetPopCount(m_gg_config.param.pop_size);
    }
    m_geo_grid.Finalize();

    // create all pools

    DaycareGenerator(m_rn_man).Apply(m_geo_grid, m_gg_config);

    PreSchoolGenerator(m_rn_man).Apply(m_geo_grid, m_gg_config);

    K12SchoolGenerator(m_rn_man).Apply(m_geo_grid, m_gg_config);

    CollegeGenerator(m_rn_man).Apply(m_geo_grid, m_gg_config);

    WorkplaceGenerator(m_rn_man).Apply(m_geo_grid, m_gg_config);

    PrimaryCommunityGenerator(m_rn_man).Apply(m_geo_grid, m_gg_config);

    SecondaryCommunityGenerator(m_rn_man).Apply(m_geo_grid, m_gg_config);

    HouseholdGenerator(m_rn_man).Apply(m_geo_grid, m_gg_config);

    // check how many pools were generated,
    // since we gave a specific household configuration for each province, these should always be used over the general
    // and since all population within the specific configurations consist purely out of elder people, no pools should
    // be generated for k12school, preschool, daycare

    unsigned int collegeCount = 0;
    unsigned int primaryCount = 0;
    unsigned int secondaryCount = 0;

    for (unsigned int i = 0; i < m_geo_grid.size(); ++i){
        const auto& loc = m_geo_grid[i];
        EXPECT_EQ(loc->CRefPools<Id::PreSchool>().size(), 0);
        EXPECT_EQ(loc->CRefPools<Id::Daycare>().size(), 0);
        EXPECT_EQ(loc->CRefPools<Id::K12School>().size(), 0);
        EXPECT_EQ(loc->CRefPools<Id::Workplace>().size(), 0);
        collegeCount += loc->CRefPools<Id::College>().size();
        primaryCount += loc->CRefPools<Id::PrimaryCommunity>().size();
        secondaryCount += loc->CRefPools<Id::SecondaryCommunity>().size();
    }

    // ceil(5.164 college students / 3.000 students per pool) = 2
    EXPECT_EQ(collegeCount, 2 * m_gg_config.pools[Id::College]);

    // ceil(60.000 persons / 2.000 persons per pool) = 30
    EXPECT_EQ(primaryCount, 30 * m_gg_config.pools[Id::PrimaryCommunity]);

    // ceil(60.000 persons / 2.000 persons per pool) = 30
    EXPECT_EQ(secondaryCount, 30 * m_gg_config.pools[Id::SecondaryCommunity]);
}

TEST_F(ScenarioDemographicProfileTest, correctUseInGenerator)
{
    m_gg_config.param.pop_size = 120000;
    m_gg_config.param.participation_college = 0.8;
    m_gg_config.param.participation_workplace = 0.5;

    auto loc1 = make_shared<Location>(1, 1, Coordinate(0, 0), "D", 30000);
    m_geo_grid.AddLocation(loc1);

    auto loc2 = make_shared<Location>(2, 2, Coordinate(0, 0), "A", 30000);
    m_geo_grid.AddLocation(loc2);

    auto loc3 = make_shared<Location>(3, 3, Coordinate(0, 0), "B", 30000);
    m_geo_grid.AddLocation(loc3);

    auto loc4 = make_shared<Location>(4, 4, Coordinate(0, 0), "C", 30000);
    m_geo_grid.AddLocation(loc4);

    auto loc5 = make_shared<Location>(5, 5, Coordinate(0, 0), "ANTWERPEN", 30000);
    m_geo_grid.AddLocation(loc5);

    // make it so that the default configuration is general and includes all kinds of ages ranges
    m_gg_config.SetData("households_flanders.csv");

    const string filename = "major_cities.csv";
    auto majorCitiesReader = ReaderFactory::CreateMajorCitiesReader(filename);
    majorCitiesReader->SetReferenceCities(m_gg_config.majorCities);

    // make it so that the provinces only have elderly who are not able to work or go to college etc
    std::map<unsigned int, std::string> elderly_households = {
            {1, "households_daycarestudents.csv"},
            {2, "households_preschoolstudents.csv"},
            {3, "households_collegestudents.csv"},
            {4, "households_workers.csv"},
            {7, "households_elderly.csv"},
            {11, "households_k12students.csv"}
    };
    m_gg_config.SetData(elderly_households, m_geo_grid);

    // assure that this data was properly read into our geogrid configuration
    std::string expected_configuration =
            R"(Input:
Fraction college commuters:                          0
Fraction workplace commuters:                        0
Participation fraction of daycare:                   0.45
Participation fraction of preschool:                 0.99
Participation fraction of college:                   0.8
Participation fraction of workplace:                 0.5
Number of major cities                               13
Target population size                               120.000

Calculated:
for region:                                          Default
    Daycare toddler count:                           2.190
    PreSchool student count:                         4.896
    K12School student count:                         20.908
    College student count:                           10.328
    Workplace person count:                          34.274

for region:                                          Antwerp
    Daycare toddler count:                           13.500
    PreSchool student count:                         0
    K12School student count:                         0
    College student count:                           0
    Workplace person count:                          0

for region:                                          Flemish Brabant
    Daycare toddler count:                           0
    PreSchool student count:                         29.700
    K12School student count:                         0
    College student count:                           0
    Workplace person count:                          0

for region:                                          West-Flanders
    Daycare toddler count:                           0
    PreSchool student count:                         0
    K12School student count:                         0
    College student count:                           24.000
    Workplace person count:                          3.000

for region:                                          East-Flanders
    Daycare toddler count:                           0
    PreSchool student count:                         0
    K12School student count:                         0
    College student count:                           0
    Workplace person count:                          15.000

for region:                                          Limburg
    Daycare toddler count:                           0
    PreSchool student count:                         0
    K12School student count:                         0
    College student count:                           0
    Workplace person count:                          0

for region:                                          Major Cities
    Daycare toddler count:                           0
    PreSchool student count:                         0
    K12School student count:                         30.000
    College student count:                           0
    Workplace person count:                          0

)";

    stringstream ss;
    ss << m_gg_config;

    EXPECT_EQ(expected_configuration, ss.str());

    // create all locations

    const string cities_name = "flanders_cities.csv";
    const auto locationsReader = ReaderFactory::CreateLocationsReader(cities_name);
    locationsReader->FillGeoGrid(m_geo_grid);

    for (unsigned int i = 0; i < m_geo_grid.size(); ++i){
        m_geo_grid[i]->SetPopCount(m_gg_config.param.pop_size);
    }
    m_geo_grid.Finalize();

    // create all pools

    DaycareGenerator(m_rn_man).Apply(m_geo_grid, m_gg_config);

    PreSchoolGenerator(m_rn_man).Apply(m_geo_grid, m_gg_config);

    K12SchoolGenerator(m_rn_man).Apply(m_geo_grid, m_gg_config);

    CollegeGenerator(m_rn_man).Apply(m_geo_grid, m_gg_config);

    WorkplaceGenerator(m_rn_man).Apply(m_geo_grid, m_gg_config);

    PrimaryCommunityGenerator(m_rn_man).Apply(m_geo_grid, m_gg_config);

    SecondaryCommunityGenerator(m_rn_man).Apply(m_geo_grid, m_gg_config);

    HouseholdGenerator(m_rn_man).Apply(m_geo_grid, m_gg_config);

    // check how many pools were generated,
    // since we gave a specific household configuration for each province, these should always be used over the general
    // and since all population within the specific configurations consist purely out of elder people, no pools should
    // be generated for k12school, preschool, daycare

    std::map<unsigned int, size_t> householdCount{
            {1, 0},
            {2, 0},
            {3, 0},
            {4, 0},
            {7, 0},
            {11, 0}
    };
    unsigned int preschoolCount = 0;
    unsigned int daycareCount = 0;
    unsigned int k12Count = 0;
    unsigned int collegeCount = 0;
    unsigned int primaryCount = 0;
    unsigned int secondaryCount = 0;
    unsigned int workplaceCount = 0;

    for (unsigned int i = 0; i < m_geo_grid.size(); ++i){
        const auto& loc = m_geo_grid[i];
        if(loc->GetProvince() != 1){
            EXPECT_EQ(loc->CRefPools<Id::Daycare>().size(), 0);
        }
        else{
            daycareCount += loc->CRefPools<Id::Daycare>().size();
        }

        if(loc->GetProvince() != 2){
            EXPECT_EQ(loc->CRefPools<Id::PreSchool>().size(), 0);
        }
        else{
            preschoolCount += loc->CRefPools<Id::PreSchool>().size();
        }

        if(loc->GetProvince() != 4 and loc->GetProvince() != 3){
            EXPECT_EQ(loc->CRefPools<Id::Workplace>().size(), 0);
        }
        else{
            workplaceCount += loc->CRefPools<Id::Workplace>().size();
        }

        if(find(m_gg_config.majorCities.begin(), m_gg_config.majorCities.end(), loc->GetName()) !=
                m_gg_config.majorCities.end()){
            k12Count += loc->CRefPools<Id::K12School>().size();
        }
        else{
            EXPECT_EQ(loc->CRefPools<Id::K12School>().size(), 0);
        }

        if(find(m_gg_config.majorCities.begin(), m_gg_config.majorCities.end(), loc->GetName()) !=
           m_gg_config.majorCities.end()) {
            householdCount[11] += loc->CRefPools<Id::Household>().size();
        }
        else{
            const auto& prov = loc->GetProvince();
            householdCount[prov] += loc->CRefPools<Id::Household>().size();
        }

        collegeCount += loc->CRefPools<Id::College>().size();
        primaryCount += loc->CRefPools<Id::PrimaryCommunity>().size();
        secondaryCount += loc->CRefPools<Id::SecondaryCommunity>().size();
    }

    // each region's number of households should be equal to the average number of households in that region
    for (const auto& el : householdCount){
        EXPECT_EQ(el.second, m_gg_config.regionInfo[el.first].count_households);
    }

    double expect = ceil(static_cast<double>(13500) / m_gg_config.people[Id::Daycare]) * m_gg_config.pools[Id::Daycare];
    EXPECT_EQ(daycareCount, expect);
    expect = ceil(static_cast<double>(29700) / m_gg_config.people[Id::PreSchool]) * m_gg_config.pools[Id::PreSchool];
    EXPECT_EQ(preschoolCount, expect);
    expect = ceil(static_cast<double>(30000) / m_gg_config.people[Id::K12School]) * m_gg_config.pools[Id::K12School];
    EXPECT_EQ(k12Count, expect);
    expect = ceil(static_cast<double>(18000) / m_gg_config.people[Id::Workplace]) * m_gg_config.pools[Id::Workplace];
    EXPECT_EQ(workplaceCount, expect);
    expect = ceil(static_cast<double>(10328) / m_gg_config.people[Id::College]) * m_gg_config.pools[Id::College];
    EXPECT_EQ(collegeCount, expect);
    expect = ceil(static_cast<double>(120000) / m_gg_config.people[Id::PrimaryCommunity]) *
            m_gg_config.pools[Id::PrimaryCommunity];
    EXPECT_EQ(primaryCount, expect);
    expect = ceil(static_cast<double>(120000) / m_gg_config.people[Id::SecondaryCommunity]) *
             m_gg_config.pools[Id::SecondaryCommunity];
    EXPECT_EQ(secondaryCount, expect);
}
