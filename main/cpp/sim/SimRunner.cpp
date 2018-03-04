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
 * Implementation for SimRunner.
 */

#include "SimRunner.h"

#include "output/AdoptedFile.h"
#include "output/CasesFile.h"
#include "output/PersonsFile.h"
#include "output/SummaryFile.h"
#include "sim/SimulatorBuilder.h"
#include "sim/event/Id.h"
#include "sim/event/Payload.h"
#include "util/ConfigInfo.h"
#include "util/FileSys.h"
#include "util/StringUtils.h"
#include "util/TimeStamp.h"
#include "viewers/CliViewer.h"

#include <boost/property_tree/xml_parser.hpp>
#include <functional>
#include <omp.h>
#include <spdlog/spdlog.h>

namespace stride {

using namespace output;
using namespace util;
using namespace sim_event;
using namespace boost::filesystem;
using namespace boost::property_tree;
using namespace std;
using namespace std::chrono;

SimRunner::SimRunner()
    : m_is_running(false), m_operational(false), m_output_prefix(""), m_pt_config(), m_sim(make_shared<Simulator>()),
      m_logger(nullptr)
{
}

bool SimRunner::Setup(const ptree& run_config_pt, shared_ptr<spdlog::logger> logger)
{
        // -----------------------------------------------------------------------------------------
        // Intro.
        // -----------------------------------------------------------------------------------------
        bool status     = true;
        m_pt_config     = run_config_pt;
        m_logger        = logger;
        m_output_prefix = m_pt_config.get<string>("run.output_prefix", "");

        // -----------------------------------------------------------------------------------------
        // Create logger
        // Transmissions:     [TRANSMISSION] <infecterID> <infectedID> <contactpoolID>
        // <day>
        // General contacts:  [CNT] <person1ID> <person1AGE> <person2AGE>  <at_home>
        // <at_work> <at_school> <at_other>
        // -----------------------------------------------------------------------------------------
        spdlog::set_async_mode(1048576);
        boost::filesystem::path logfile_path = m_output_prefix;
        if (run_config_pt.get<bool>("run.use_install_dirs")) {
                logfile_path += "_logfile";
        } else {
                logfile_path /= "logfile";
        }
        auto file_logger = spdlog::rotating_logger_mt("contact_logger", logfile_path.c_str(),
                                                      numeric_limits<size_t>::max(), numeric_limits<size_t>::max());
        file_logger->set_pattern("%v"); // Remove meta data from log => time-stamp of logging

        // ------------------------------------------------------------------------------
        // Create the simulator.
        //------------------------------------------------------------------------------
        const auto track_index_case = m_pt_config.get<bool>("run.track_index_case");
        const auto num_threads      = m_pt_config.get<unsigned int>("run.num_threads");
        m_logger->info("Building the simulator.");
        m_sim = SimulatorBuilder::Build(m_pt_config, num_threads, track_index_case);
        m_logger->info("Done building the simulator.");

        // -----------------------------------------------------------------------------------------
        // Check the simulator.
        // -----------------------------------------------------------------------------------------
        if (m_sim->IsOperational()) {
                m_logger->info("Done checking the simulator.");
        } else {
                m_logger->critical("Invalid configuration => terminate without output");
                status = false;
        }
        return status;
}

/// Run the simulator with config information provided.
void SimRunner::Run()
{
        // -----------------------------------------------------------------------------------------
        // Run the simulator.
        // -----------------------------------------------------------------------------------------
        const auto           num_days{m_pt_config.get<unsigned int>("run.num_days")};
        vector<unsigned int> cases(num_days);
        vector<unsigned int> adopted(num_days);
        Notify({m_sim, Id::AtStart});
        for (unsigned int i = 0; i < num_days; i++) {
                m_sim->TimeStep();
                Notify({m_sim, Id::Stepped});
                // cases[i]   = m_sim->GetPopulation()->GetInfectedCount();
                // adopted[i] = m_sim->GetPopulation()->GetAdoptedCount();
                // m_logger->info("     Simulated day: {:4}  Done, infected count: {:7}      Adopters count: {:7}", i,
                //               cases[i], adopted[i]);
        }
        Notify({m_sim, Id::Finished});
}

} // namespace stride
