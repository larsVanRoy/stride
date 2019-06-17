#pragma once
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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Observer for the epidemiological status (epi-output).
 */

#include "geopop/io/EpiWriter.h"
#include <fstream>
#include "sim/SimRunner.h"
#include "sim/event/Id.h"

namespace stride {

class SimRunner;

namespace viewers {

/// Viewer of Simulator for cases output.
class EpiViewer
{
public:
    /// Instantiate cases viewer.
    EpiViewer(std::shared_ptr<SimRunner> runner, const std::string& output_prefix, int step);

    ~EpiViewer() {m_file.close();};

    /// Let viewer perform update.
    void Update(sim_event::Id id);

private:
    std::shared_ptr<geopop::EpiWriter>    m_epi_output;
    std::shared_ptr<SimRunner> m_runner;
    unsigned m_timestep;
    unsigned m_step_size;
    std::ofstream m_file;
};

} // namespace viewers
} // namespace stride
