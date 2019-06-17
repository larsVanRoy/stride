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

#include "EpiGridHDF5Types.h"

namespace H5{
    CompType getLocationType(){
        CompType location_t(sizeof(H5GeoLocation));
        location_t.insertMember("id", HOFFSET(H5GeoLocation, id), PredType::NATIVE_UINT);
        location_t.insertMember("population", HOFFSET(H5GeoLocation, population), PredType::NATIVE_UINT);
        location_t.insertMember("name", HOFFSET(H5GeoLocation, name), StrType(H5T_C_S1, H5T_VARIABLE));
        location_t.insertMember("province", HOFFSET(H5GeoLocation, province), PredType::NATIVE_UINT);
        location_t.insertMember("longitude", HOFFSET(H5GeoLocation, longitude), PredType::NATIVE_DOUBLE);
        location_t.insertMember("latitude", HOFFSET(H5GeoLocation, latitude), PredType::NATIVE_DOUBLE);
        return location_t;
    }
    const CompType GetH5Type(H5GeoLocation){
        return getLocationType();
    }
}