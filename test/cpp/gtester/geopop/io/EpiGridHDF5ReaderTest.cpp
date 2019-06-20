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


#include "pop/Population.h"
#include "geopop/EpiGrid.h"
#include "geopop/io/EpiHDF5Reader.h"
#include "util/FileSys.h"

#include <gtest/gtest.h>

using namespace std;
using namespace stride;
using namespace stride::util;
using namespace stride::ContactType;
using namespace geopop;
using boost::geometry::get;

namespace {

std::shared_ptr<EpiGrid> getEpiGridFromFile(const string& filename)
{
        auto              file = FileSys::GetTestsDir().string() + "/testdata/EpiGridHDF5/" + filename;
        EpiHDF5Reader epiHdf5Reader(file);
        return epiHdf5Reader.Read();
}


TEST(EpiGridHDF5ReaderTest, locationsTest)
{
        EpiGrid expected_grid;

        expected_grid.AddLocation( std::make_shared<EpiLocation<Coordinate>>(1, 4, Coordinate{0, 0}, "Bavikhove", 2500));
        expected_grid.AddLocation( std::make_shared<EpiLocation<Coordinate>>(2, 3, Coordinate{0, 0}, "Gent", 5000));
        expected_grid.AddLocation( std::make_shared<EpiLocation<Coordinate>>(3, 2, Coordinate{0, 0}, "Mons", 2500));

        auto actual_grid = getEpiGridFromFile("locationsTest.h5");

        EXPECT_EQ(expected_grid.size(), actual_grid->size());

        for (size_t i = 0; i < expected_grid.size(); ++i){
                auto expected_loc = expected_grid[i];
                auto actual_loc = actual_grid->operator[](i);

                EXPECT_EQ(expected_loc->GetName(), actual_loc->GetName());
                EXPECT_EQ(expected_loc->GetProvince(), actual_loc->GetProvince());
                EXPECT_EQ(expected_loc->GetPopCount(), actual_loc->GetPopCount());
                EXPECT_EQ(expected_loc->GetID(), actual_loc->GetID());
                EXPECT_EQ(get<0>(expected_loc->GetCoordinate()), get<0>(actual_loc->GetCoordinate()));
                EXPECT_EQ(get<1>(expected_loc->GetCoordinate()), get<1>(actual_loc->GetCoordinate()));
        }
}

TEST(EpiGridHDF5ReaderTest, historyTest)
{
        auto actual_grid = getEpiGridFromFile("historyTest.h5");
        for (const auto& loc : *actual_grid){
                EXPECT_EQ(3, loc->maxDays());
                for(unsigned int i = 0; i<3; ++i) {
                        auto status = loc->GetPoolStatus(i);
                        for(const auto& bracket : AgeBrackets::AgeBracketList){
                                for(const auto& percent : status->operator[](bracket)->toVector()){
                                        EXPECT_EQ(0, percent);
                                }
                        }
                }
        }

}
} // namespace
