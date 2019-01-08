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
 *  Copyright 2018, Niels Aerens, Thomas Avé, Jan Broeckhove, Tobia De Koninck, Robin Jadoul
 */

#pragma once

#include "Community.h"

namespace gengeopop {

class GeoGrid;

/**
 * A model of a Primary Community (as a ContactCenter)
 */
class PrimaryCommunity : public Community
{
public:
        /// Construct community with assigned ID.
        explicit PrimaryCommunity(unsigned int id) : Community(id) {}

        /// See ContactCenter::Fill.
        void Fill(const std::shared_ptr<GeoGrid>& geoGrid) override
        {
                if (m_pools.empty()) {
                        AddPool(geoGrid->CreateContactPool(stride::ContactPoolType::Id::PrimaryCommunity));
                }
        }

        /// See ContactCenter::GetType.
        std::string GetType() const override { return "Primary Community"; }
};

} // namespace gengeopop
