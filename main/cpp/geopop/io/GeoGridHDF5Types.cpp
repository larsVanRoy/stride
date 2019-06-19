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
#include "GeoGridHDF5Types.h"

namespace H5 {
CompType GetPersonType()
{
        CompType person_t(sizeof(H5Person));
        person_t.insertMember("id", HOFFSET(H5Person, id), PredType::NATIVE_UINT);
        person_t.insertMember("age", HOFFSET(H5Person, age), PredType::NATIVE_FLOAT);
        person_t.insertMember("daycare", HOFFSET(H5Person, daycare), PredType::NATIVE_UINT);
        person_t.insertMember("preschool", HOFFSET(H5Person, preschool), PredType::NATIVE_UINT);
        person_t.insertMember("k12School", HOFFSET(H5Person, k12School), PredType::NATIVE_UINT);
        person_t.insertMember("houseHold", HOFFSET(H5Person, houseHold), PredType::NATIVE_UINT);
        person_t.insertMember("workplace", HOFFSET(H5Person, workplace), PredType::NATIVE_UINT);
        person_t.insertMember("primaryCommunity", HOFFSET(H5Person, primaryCommunity), PredType::NATIVE_UINT);
        person_t.insertMember("secondaryCommunity", HOFFSET(H5Person, secondaryCommunity), PredType::NATIVE_UINT);
        person_t.insertMember("college", HOFFSET(H5Person, college), PredType::NATIVE_UINT);
        return person_t;
}

CompType GetCommuteType()
{
        CompType commute_t(sizeof(H5Commute));
        commute_t.insertMember("to", HOFFSET(H5Commute, to), PredType::NATIVE_UINT);
        commute_t.insertMember("proportion", HOFFSET(H5Commute, proportion), PredType::NATIVE_DOUBLE);
        return commute_t;
}

StrType        GetStringType() { return StrType(PredType::C_S1, 200); }
const DataType GetH5Type(const unsigned int&) { return PredType::NATIVE_UINT; }
const DataType GetH5Type(int) { return PredType::NATIVE_INT; }
const DataType GetH5Type(float) { return PredType::NATIVE_FLOAT; }
const DataType GetH5Type(unsigned long) { return PredType::NATIVE_ULONG; }
const DataType GetH5Type(double) { return PredType::NATIVE_DOUBLE; }

const StrType  GetH5Type(const std::string& value) { return StrType(H5T_C_S1, value.size()); }
const CompType GetH5Type(H5Person) { return GetPersonType(); }
const CompType GetH5Type(H5Commute) { return GetCommuteType(); }
}
