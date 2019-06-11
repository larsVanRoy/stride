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

#pragma once

#include "H5Cpp.h"
namespace H5 {

struct H5Commute
{
        unsigned int to;
        double       proportion;
};

struct H5Person
{
        unsigned int id;
        float        age;
        unsigned int daycare;
        unsigned int preschool;
        unsigned int k12School;
        unsigned int houseHold;
        unsigned int workplace;
        unsigned int primaryCommunity;
        unsigned int secondaryCommunity;
        unsigned int college;
};

CompType GetPersonType();

CompType GetCommuteType();

StrType GetStringType();

const DataType GetH5Type(const unsigned int&);
const DataType GetH5Type(int);
const DataType GetH5Type(float);
const DataType GetH5Type(double);
const DataType GetH5Type(unsigned long);
const StrType  GetH5Type(const std::string& value);
const CompType GetH5Type(H5Person);
const CompType GetH5Type(H5Commute);
}