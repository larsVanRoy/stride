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

# pragma once

#include <memory>
#include <vector>
#include <iostream>

namespace stride {

    class ContactPool;
    class Person;

} // namespace stride

namespace  geopop {

using namespace std;

/**
* Creates a Reader that retrieves the different major city profiles from a given CSV file.
*/

class MajorCitiesReader {
public:
        /// Construct the HouseholdCSVReader with an istream containing the CSV data.
        MajorCitiesReader(unique_ptr<istream> inputStream);

        /// Add the locations to the GeoGrid.
        void SetReferenceCities(vector<std::string> &major_cities);

private:
        /// Input stream connected to input data file.
        std::unique_ptr<std::istream> m_input_stream;
};

} // namespace geopop