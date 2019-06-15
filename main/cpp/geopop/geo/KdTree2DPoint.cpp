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

#include "KdTree2DPoint.h"

#include "GeoLocation.h"

#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/algorithms/within.hpp>
#include <boost/geometry/core/access.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/strategies/geographic/distance.hpp>

namespace geopop {

namespace geogrid_detail {

template <class Data>
KdTree2DPoint<Data>::KdTree2DPoint(const GeoLocation<Data>* loc) : m_pt(loc->GetCoordinate()), m_location(loc)
{
}

template <class Data>
bool KdTree2DPoint<Data>::operator==(const KdTree2DPoint<Data>& other) const { return Distance(other) < 0.001; }

template <class Data>
bool KdTree2DPoint<Data>::InBox(const AABBox<KdTree2DPoint<Data>>& box) const
{
        return boost::geometry::within(m_pt, boost::geometry::model::box<Coordinate>{box.lower.m_pt, box.upper.m_pt});
}

template <class Data>
bool KdTree2DPoint<Data>::InRadius(const KdTree2DPoint<Data>& start, double radius) const { return Distance(start) <= radius; }

template <class Data>
double KdTree2DPoint<Data>::Distance(const KdTree2DPoint<Data>& other) const
{
        return boost::geometry::distance(m_pt, other.m_pt, boost::geometry::strategy::distance::geographic<>{}) /
               1000.0;
}

} // namespace geogrid_detail
} // namespace geopop
