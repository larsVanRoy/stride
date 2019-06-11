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

#include "MajorCitiesReader.h"

#include "util/CSV.h"

namespace geopop {

using namespace std;
using namespace stride::util;

MajorCitiesReader::MajorCitiesReader(unique_ptr<istream> inputStream)
        : m_input_stream(move(inputStream))
{
}

void MajorCitiesReader::SetReferenceCities(vector<std::string> &major_cities)
{
    CSV reader(*(m_input_stream.get()));

    for (const CSVRow& row : reader) {
        try {
            string city = row.GetValue<string>(0);
            major_cities.emplace_back(city);
        } catch (const std::bad_cast& e) {
            // incorrect format/data types
            throw(e);
        }
    }
}

} // namespace geopop