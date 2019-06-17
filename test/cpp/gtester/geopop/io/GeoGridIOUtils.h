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

#pragma once

#include "contact/ContactType.h"
#include "geopop/GeoGrid.h"
#include "geopop/Location.h"
#include "H5Cpp.h"

namespace proto {
class GeoGrid;
class GeoGrid_Location;
class GeoGrid_Location_Coordinate;
class GeoGrid_Location_ContactPools;
class GeoGrid_Location_ContactPools_ContactPool;
class GeoGrid_Person;
} // namespace proto

namespace stride {
class ContactPool;
class Population;
} // namespace stride

using namespace geopop;

void CompareContactPool(stride::ContactPool*                                    contactPool,
                        const proto::GeoGrid_Location_ContactPools_ContactPool& protoContactPool);

void CompareContactPools(stride::ContactType::Id                                    typeId,
                         const stride::util::SegmentedVector<stride::ContactPool*>& contactPools,
                         const proto::GeoGrid_Location_ContactPools&                protoContactPools);

void CompareCoordinate(const Coordinate& coordinate, const proto::GeoGrid_Location_Coordinate& protoCoordinate);

void CompareLocation(const Location<Coordinate>& location, const proto::GeoGrid_Location& protoLocation);

void ComparePerson(const proto::GeoGrid_Person& protoPerson);

void CompareGeoGrid(GeoGrid& geoGrid);

void CompareGeoGrid(proto::GeoGrid& protoGrid);

std::shared_ptr<GeoGrid> GetPopulatedGeoGrid(stride::Population*);

std::shared_ptr<GeoGrid> GetCommutesGeoGrid(stride::Population*);

template<typename T>
void ReadHDF5Attribute(const std::string& name, T* data, const H5::H5Object& object)
{
        auto attr = object.openAttribute(name);
        switch (attr.getTypeClass()) {
        default: attr.read(attr.getDataType(), data); break;
        case H5T_COMPOUND: attr.read(attr.getCompType(), data); break;
        }
}
template <>
inline void ReadHDF5Attribute(const std::string& name, std::string* data, const H5::H5Object& object)
{
        auto attr = object.openAttribute(name);
        attr.read(H5::StrType(H5T_C_S1, H5T_VARIABLE), *data);
}
template <typename T>
void ReadHDF5Dataset(const std::string& name, T* data, const H5::H5Object& object)
{
        auto dataSet = object.openDataSet(name);
        switch (dataSet.getTypeClass()) {
        default: dataSet.read(data, dataSet.getDataType()); break;
        case H5T_STRING: dataSet.read(data, dataSet.getStrType()); break;
        case H5T_COMPOUND: dataSet.read(data, dataSet.getCompType()); break;
        }
}
