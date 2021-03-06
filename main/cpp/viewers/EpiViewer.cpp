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
 * Definition of Observer of the epidemiological status of the locations
 */

#include "EpiViewer.h"
#include "geopop/io/EpiWriterFactory.h"
#include <iostream>
#include <string>
#include "util/FileSys.h"
#include "geopop/GeoGrid.h"
#include "sim/Sim.h"
#include "sim/SimRunner.h"
#include "pop/Population.h"

#include <exception>

using namespace std;
using namespace std::chrono;
using namespace stride::sim_event;

namespace stride {
namespace viewers {

using namespace stride::util;

EpiViewer::EpiViewer(std::shared_ptr <SimRunner> runner, const std::string &output_prefix, const std::string& filename, unsigned int step) :
m_epi_output(), m_runner(std::move(runner)), m_timestep(0), m_step_size(step) {

    const auto p = FileSys::BuildPath(output_prefix, filename);
    m_epi_output = geopop::EpiWriterFactory::CreateEpiWriter(p.c_str());
    if(m_step_size <= 0) {
        m_step_size = 1;
    }
}

void EpiViewer::Update(const sim_event::Id id)
{
    switch (id) {
        case Id::AtStart:
            m_epi_output->Initialize(m_runner->GetSim()->GetPopulation()->CRefGeoGrid());
            break;
        case Id::Stepped: {
            if(m_timestep%m_step_size == 0){
                const geopop::GeoGrid& geoGrid = m_runner->GetSim()->GetPopulation()->CRefGeoGrid();
                m_epi_output->Write(geoGrid, ++m_timestep);
            }
            break;
        }
        case Id::Finished: {
            if(m_timestep%m_step_size != 0){
                const geopop::GeoGrid& geoGrid = m_runner->GetSim()->GetPopulation()->CRefGeoGrid();
                m_epi_output->Write(geoGrid, ++m_timestep);
            }
            m_epi_output->Finalize();
            break;
        }
        default: break;
    }
    ++m_timestep;
}

} // namespace viewers
} // namespace stride
