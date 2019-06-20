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

#include "geopop/io/EpiProtoWriter.h"
#include "geopop/io/EpiProtoReader.h"
#include "pop/Population.h"

#include "geopop/EpiGrid.h"
#include "pop/Population.h"

#include <gtest/gtest.h>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;

namespace {

void CompareCoordinate(const  Coordinate& geoCoordinate, const  Coordinate& epiCoordinate)
{
    using boost::geometry::get;
    EXPECT_EQ(get<0>(geoCoordinate), get<0>(epiCoordinate));
    EXPECT_EQ(get<1>(geoCoordinate), get<1>(epiCoordinate));
}

void CompareLocation(const  shared_ptr<Location<Coordinate>>&    geoLocation,
                     const  shared_ptr<EpiLocation<Coordinate>>& epiLocation)
{
    EXPECT_EQ(geoLocation->GetName(), epiLocation->GetName());
    EXPECT_EQ(geoLocation->GetPopCount(), epiLocation->GetPopCount());
    CompareCoordinate(geoLocation->GetCoordinate(), epiLocation->GetCoordinate());

    map<AgeBrackets::AgeBracket, map<string, double>> statuses;

    for (const AgeBrackets::AgeBracket& bracket : AgeBrackets::AgeBracketList)
    {
        map<string, double> health;
        for (const HealthStatus& h: HealthStatusList)
        {
            health[HealthToString(h)] = 0;
        }
        statuses[bracket] = health;
    }

    for (const auto &pool : geoLocation->CRefPools(ContactType::Id::Household)) {
        for (const auto &person : *pool) {

            statuses[AgeBrackets::ToAgeBracket(person->GetAge())][HealthToString(person->GetHealth().GetStatus())] ++;
        }
    }
    const auto& epiStatus = epiLocation->GetPoolStatus(0);
    static const map<int, HealthStatus> HealthPoolTypes = {
            {0, HealthStatus::Exposed},
            {1, HealthStatus::Immune},
            {2, HealthStatus::Infectious},
            {3, HealthStatus::InfectiousAndSymptomatic},
            {4, HealthStatus::Recovered},
            {5, HealthStatus::Susceptible},
            {6, HealthStatus::Symptomatic}
    };

    for (const AgeBrackets::AgeBracket& bracket : AgeBrackets::AgeBracketList)
    {
        const auto& epiPoolStatus = epiStatus->getStatus(bracket);
        vector<double> status;
        for (const auto &healthStatus : HealthStatusList) {
            status.push_back(static_cast<double>(statuses[bracket][HealthToString(healthStatus)]) / geoLocation->GetPopCount());
        }

        for (unsigned idx = 0; idx < status.size(); idx ++)
        {
            EXPECT_EQ(status[idx], epiPoolStatus->operator[](HealthPoolTypes.at(idx)));
        }
    }
}

void CompareGrid(const GeoGrid& geoGrid, const  EpiGrid& epiGrid)
{
    EXPECT_EQ(geoGrid.size(), epiGrid.size());

    for (size_t i = 0; i < geoGrid.size(); i ++)
    {
        auto geoLocation = geoGrid[i];
        auto epiLocation = epiGrid[i];
        CompareLocation(geoLocation, epiLocation);
    }
}

TEST(EpiGridProtoReaderWriterScenarioTest, locationTest)
{
    auto pop =  stride::Population::Create();
    GeoGrid geoGrid(pop.get());

    geoGrid.AddLocation(make_shared<Location<Coordinate>>(1, 4, Coordinate(0, 0), "Bavikhove", 2500));
    geoGrid.AddLocation(make_shared<Location<Coordinate>>(2, 3, Coordinate(0, 0), "Gent", 5000));
    geoGrid.AddLocation(make_shared<Location<Coordinate>>(3, 2, Coordinate(0, 0), "Mons", 2500));

    EpiProtoWriter       writer("locationTest.proto");
    writer.Initialize(geoGrid);
    writer.Write(geoGrid, 0);
    writer.Finalize();

    unique_ptr<ifstream> is = make_unique<ifstream>("locationTest.proto");
    EpiProtoReader       reader(move(is));
    shared_ptr< EpiGrid>  grid = reader.Read();

    CompareGrid(geoGrid, *grid);
    remove("locationTest.proto");
}

TEST(EpiGridProtoReaderWriterScenartioTest, contactPoolsTest)
{
    auto pop =  stride::Population::Create();
    GeoGrid geoGrid(pop.get());

    auto loc = make_shared<Location<Coordinate>>(1, 4, Coordinate(0, 0), "Bavikhove", 2500);

    loc->RefPools(Id::Daycare).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Daycare));
    loc->RefPools(Id::PreSchool).emplace_back(pop->RefPoolSys().CreateContactPool(Id::PreSchool));
    loc->RefPools(Id::K12School).emplace_back(pop->RefPoolSys().CreateContactPool(Id::K12School));
    loc->RefPools(Id::PrimaryCommunity).emplace_back(pop->RefPoolSys().CreateContactPool(Id::PrimaryCommunity));
    loc->RefPools(Id::College).emplace_back(pop->RefPoolSys().CreateContactPool(Id::College));
    loc->RefPools(Id::Household).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Household));
    loc->RefPools(Id::Workplace).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Workplace));

    EpiProtoWriter       writer("locationTest.proto");
    writer.Initialize(geoGrid);
    writer.Write(geoGrid, 0);
    writer.Finalize();

    unique_ptr<ifstream> is = make_unique<ifstream>("locationTest.proto");
    EpiProtoReader       reader(move(is));
    shared_ptr< EpiGrid>  grid = reader.Read();

    CompareGrid(geoGrid, *grid);
    remove("locationTest.proto");
}
}
