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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header for the GuiInterface class.
 */

#include "geopop/geo/Coordinate.h"

namespace stride {

using locID unsigned;

class GuiInterface {
public:
    GuiInterface();

    ~GuiInterface();

    /// read file
    void Read(const string &filename);

    bool addTime(unsigned int);

    const Coordinate& getLocationCoordinate(locId);

    const string& getLocationName(locId);

    const long& getLocationPopCount(locId);

    const vector<double>& getLocationHealth();

    const vector<double> getRadiusHealth(const Coordinate&, double radius);

    const vector<double> getBoxHealth(const Coordinate&, const Coordinate&);

private:
    //EpiGrid m_epigrid;

    unsigned time;          ///current time
};

}