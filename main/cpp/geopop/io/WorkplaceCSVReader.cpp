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

#include <iostream>
#include "WorkplaceCSVReader.h"

#include "util/CSV.h"

namespace geopop {

using namespace std;
using namespace stride::util;

WorkplaceCSVReader::WorkplaceCSVReader(unique_ptr<istream> inputStream)
    : m_input_stream(move(inputStream))
{
}

void WorkplaceCSVReader::SetReferenceWorkplaces(vector<vector<double>>& ref_workplaces)
{
        CSV reader(*(m_input_stream.get()));

        for (const CSVRow& row : reader) {

                vector<double> temp;
                double ratio;
                int min_size;
                int max_size;
                try {
                        ratio = row.GetValue<double>(0);
                        min_size = row.GetValue<unsigned int>(1);
                        max_size = row.GetValue<unsigned int>(2);
                } catch (const std::bad_cast& e) {
                        // incorrect format/data types
                        throw(e);
                }

                temp = {ratio, double(min_size), double(max_size)};
                ref_workplaces.emplace_back(temp);
        }
}

} // namespace geopop
