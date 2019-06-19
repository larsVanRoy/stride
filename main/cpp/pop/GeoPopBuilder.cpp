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
 *  Copyright 2018, 2019 Jan Broeckhove and Bistromatics group.
 */

/**
 * @file
 * Initialize populations: implementation. NOTICE: WorkplacePopulator logic
 * requires that CollegePopulator is executed prior to WorkplacePopulator.
 */

#include "GeoPopBuilder.h"

#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/generators/Generator.h"
#include "geopop/io/ReaderFactory.h"
#include "geopop/populators/Populator.h"
#include "pop/Population.h"
#include "pop/SurveySeeder.h"
#include "util/FileSys.h"

#include "util/RnMan.h"
#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/optional/optional.hpp>
#include <contact/ContactType.h>
#include <spdlog/logger.h>

namespace stride {

using namespace std;
using namespace util;
using namespace ContactType;
using namespace boost::property_tree;
using namespace geopop;

GeoPopBuilder::GeoPopBuilder(const ptree& config, RnMan& rnMan, shared_ptr<spdlog::logger> strideLogger)
    : AbstractPopBuilder(config, rnMan, move(strideLogger))
{
}

shared_ptr<Population> GeoPopBuilder::Build(shared_ptr<Population> pop)
{
        m_stride_logger->trace("Building geopop.");

        // ------------------------------------------------------------
        // Get GeoGrid associated with 'pop'.
        // ------------------------------------------------------------
        auto& geoGrid = pop->RefGeoGrid();

        // ------------------------------------------------------------
        // Set the GeoGridConfig.
        // ------------------------------------------------------------

        m_stride_logger->trace("Starting Reading of Household files.");

        GeoGridConfig ggConfig(m_config);

        // default config
        ggConfig.SetData(m_config.get<string>("run.geopop_gen.household_file"));

        // major cities
        boost::optional<const ptree&> temp_config = m_config.get_child_optional("run.geopop_gen.major_cities");
        if (temp_config){
                ggConfig.SetMajorCitiesData(m_config.get<string>("run.geopop_gen.major_cities"));
        }

        // specific configs
        map<unsigned int, string> possible_household_files =
                {
                        {1,  "run.geopop_gen.antwerp_household_file"},
                        {2,  "run.geopop_gen.flemish_brabant_household_file"},
                        {3,  "run.geopop_gen.west_flanders_household_file"},
                        {4,  "run.geopop_gen.east_flanders_household_file"},
                        {7,  "run.geopop_gen.limburg_household_file"},
                        {11, "run.geopop_gen.major_cities_household_file"}
                };

        std::map<unsigned int, string> files;
        boost::optional<const ptree& > temp_household;

        for (const auto& pair : possible_household_files){
                temp_household = m_config.get_child_optional(pair.second);
                if (temp_household){
                        files[pair.first] = m_config.get<string>(pair.second);
                }
        }
        if( !files.empty() ){
                ggConfig.SetData(files, geoGrid);
        }

        m_stride_logger->trace("Finished Reading of Household files.");

        // ------------------------------------------------------------
        // Set the workplace data.
        // ------------------------------------------------------------
        boost::optional<const ptree& > workplace_config = m_config.get_child_optional("run.geopop_gen.workplace_file");
        if(workplace_config) {
            m_stride_logger->trace("Starting Reading of Workplace files.");
                ggConfig.SetWorkplaceData(m_config.get<string>("run.geopop_gen.workplace_file"));
            m_stride_logger->trace("Finished Reading of Workplace files.");
        }

        // ------------------------------------------------------------
        // Read locations file (commute file only if present).
        // ------------------------------------------------------------
        string commutesFile;
        auto   geopop_gen = m_config.get_child("run.geopop_gen");
        if (geopop_gen.count("commuting_file")) {
                commutesFile = m_config.get<string>("run.geopop_gen.commuting_file");
        }

        m_stride_logger->trace("Starting MakeLocations");
        MakeLocations(geoGrid, ggConfig, m_config.get<string>("run.geopop_gen.cities_file"), commutesFile);
        m_stride_logger->trace("Finished MakeLocations");

        // ------------------------------------------------------------
        // Generate Geo.
        // ------------------------------------------------------------
        m_stride_logger->trace("Starting MakePools");
        MakePools(geoGrid, ggConfig);
        m_stride_logger->trace("Finished MakePools");

        // ------------------------------------------------------------
        // Generate Pop.
        // ------------------------------------------------------------
        m_stride_logger->trace("Starting MakePersons");
        MakePersons(geoGrid, ggConfig);
        m_stride_logger->trace("Finished MakePersons");

        // ------------------------------------------------------------
        // Done.
        // ------------------------------------------------------------
        m_stride_logger->trace("Done building geopop.");

        return pop;
}

void GeoPopBuilder::MakeLocations(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig, const string& citiesFileName,
                                  const string& commutingFileName)
{
        const auto locationsReader = ReaderFactory::CreateLocationsReader(citiesFileName);
        locationsReader->FillGeoGrid(geoGrid);

        if (!commutingFileName.empty()) {
                const auto commutesReader = ReaderFactory::CreateCommutesReader(commutingFileName);
                commutesReader->FillGeoGrid(geoGrid);
        }

        for (unsigned i = 0; i <geoGrid.size(); ++i) {
                geoGrid[i]->SetPopCount(geoGridConfig.param.pop_size);
        }
        geoGrid.Finalize();
}

void GeoPopBuilder::MakePools(GeoGrid& geoGrid, GeoGridConfig& geoGridConfig)
{
        DaycareGenerator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        PreSchoolGenerator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        K12SchoolGenerator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        CollegeGenerator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        WorkplaceGenerator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        PrimaryCommunityGenerator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        SecondaryCommunityGenerator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        HouseholdGenerator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);
}

void GeoPopBuilder::MakePersons(GeoGrid& geoGrid, GeoGridConfig& geoGridConfig)
{
        // NOTICE: WorkplacePopulator logic requires that CollegePopulator
        // has been executed prior to WorkplacePopulator.

        DaycarePopulator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        PreSchoolPopulator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        HouseholdPopulator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        K12SchoolPopulator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        CollegePopulator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        PrimaryCommunityPopulator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        SecondaryCommunityPopulator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        WorkplacePopulator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);
}

} // namespace stride
