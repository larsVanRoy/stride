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
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#include "contact/ContactPool.h"
#include "contact/IdSubscriptArray.h"
#include "pop/Person.h"
#include "util/SegmentedVector.h"
#include "util/RnMan.h"
#include "Location.h"
#include "GeoGrid.h"

#include <boost/property_tree/ptree_fwd.hpp>

#include <memory>
#include <ostream>
#include <map>

namespace geopop {

using namespace stride;

class GeoGrid;

/**
 * Configuration data mostly for generating a population, but also for computing
 * the required number of schools, workplaces, communities etc. for that population.
 */
class GeoGridConfig
{
public:
        /// Default constructor needed in test code.
        GeoGridConfig();

        /// Constructor that configures input data.
        explicit GeoGridConfig(const boost::property_tree::ptree& configPt);

        /// People per unit (= Household, K12School, College, etc.) for each of the ContactTypes.
        /// Default initialization. Order in which contacttypes are listed in the
        /// definition of the enumeration must be respected!
        stride::ContactType::IdSubscriptArray<unsigned int> people {1U, 10U, 250U, 500U, 3000U, 20U, 2000U, 2000U};

        /// Pools per unit (= Household, K12School, College, etc.) for each of the ContactTypes.
        /// Default initialization. Order in which contacttypes are listed in the
        /// definition of the enumeration must be respected!
        stride::ContactType::IdSubscriptArray<unsigned int> pools {1U, 1U, 5U, 25U, 20U, 1U, 1U, 1U};

        // -----------------------------------------------------------------------------------------
        // Parameters set by constructor with configuration property tree.
        // -----------------------------------------------------------------------------------------
        struct
        {
                /// Participation of daycare (fraction of people of daycare age going to daycare).
                double participation_daycare = 0.45;

                /// Participation of preschool (fraction of people of preschool age going to preschool).
                double participation_preschool = 0.99;

                /// Participation of college (fraction of people of college age going to college).
                double participation_college;

                /// Participation of workplace (fraction of people of work age and not going to
                /// college and having employment).
                double participation_workplace;

                /// Fraction of college students that commute.
                double fraction_college_commuters;

                /// Fraction of people in the workplace that commute.
                double fraction_workplace_commuters;

                /// Target population size for the generated population.
                unsigned int pop_size;

                /// configuration for distribution of workplace sizes
                std::vector<std::vector<double>> workplace_distribution;
        } param;

        // -----------------------------------------------------------------------------------------
        // The reference Households used to generate the population by random draws.
        // -----------------------------------------------------------------------------------------
        struct refHH
        {
                /// Number of persons in the reference household set.
                unsigned int person_count = 0U;

                /// Age profile per reference household.
                std::vector<std::vector<unsigned int>> ages{};
        };

        // -----------------------------------------------------------------------------------------
        // A map that contains the different specified configurations for the households
        // each specifically specified location will get an entry in the map
        // along with the required default configuration.
        // Whenever a locations province id is not in the map, the default will be chosen.
        // The 0 key will be used to specify the default household reference (which is required).
        // The respective provinces will be represented by their id.
        // Key 11 will be used to store potential references for major city references.
        // -----------------------------------------------------------------------------------------
        std::map<unsigned int, refHH> refHHs;

        // -----------------------------------------------------------------------------------------
        // The sizes of the generated workplace pools
        // Every pool's id will be linked to a corresponding size
        // -----------------------------------------------------------------------------------------
        std::map<unsigned int, unsigned int> wpPoolSizes{};

        // -----------------------------------------------------------------------------------------
        // These are numbers derived from the reference households, the target size of the generated
        // population and the input parameters relating participation in college and workplace.
        // These numbers are used as targets in the population generation process and are reproduced
        // (to very close approximation) in the generated population.
        // The numbers are set by the SetData method.
        // -----------------------------------------------------------------------------------------
        struct info
        {
                ///Numbers of individuals in Daycare.
                unsigned int popcount_daycare;

                ///Numbers of individuals in PreSchool.
                unsigned int popcount_preschool;

                /// Numbers of individuals in K12School.
                unsigned int popcount_k12school;

                /// Number of individuals in College.
                unsigned int popcount_college;

                /// Number of individuals in Workplace.
                unsigned int popcount_workplace;

                /// The number of households.
                unsigned int count_households;

                /// The size of the population within the region.
                unsigned int popcount;
        };

        // -----------------------------------------------------------------------------------------
        // Diversions in the age reference occurs when considering multiple references for provinces,
        // therefore we need a means to store differences info objects, the info object is read within
        // the setData function and for each defined household reference (0 is the default and 11 is
        // a special definition for major cities, which differ from the "general" households/cities
        // -----------------------------------------------------------------------------------------
        std::map<unsigned int, info> regionInfo;

        // -----------------------------------------------------------------------------------------
        // simple helper map, to convert ID's to their respective names
        // Id 0 is a special id, and is the general configuration that will be used in case
        // no specific data was given.
        // Id 11 is also a special id, and is used to symbolise the major cities, which are in
        // general more densely populated.
        // All other id's represent the corresponding provinces.
        // -----------------------------------------------------------------------------------------
        std::map<unsigned int, std::string> idNames{
                {0, "Default"},
                {1, "Antwerp"},
                {2, "Flemish Brabant"},
                {3, "West-Flanders"},
                {4, "East-Flanders"},
                {7, "Limburg"},
                {11, "Major Cities"}
        };

        // -----------------------------------------------------------------------------------------
        // simple vector of all known major cities, used in the household populator to check if a
        // location is a major city
        // -----------------------------------------------------------------------------------------
        std::vector<std::string> majorCities;

        // -----------------------------------------------------------------------------------------
        /// Read the househould data file, parse it and set data.
        // -----------------------------------------------------------------------------------------
        void SetData(const std::string& householdsFileName);

        // -----------------------------------------------------------------------------------------
        /// Read the househould data files, parse them and set data.
        // -----------------------------------------------------------------------------------------
        void SetData(const std::map<unsigned int, std::string>& householdFileNames, GeoGrid& geoGrid);

        // -----------------------------------------------------------------------------------------
        /// Read the workplace data file, parse it and set data.
        // -----------------------------------------------------------------------------------------
        void SetWorkplaceData(const std::string& workplaceFileName);

        // -----------------------------------------------------------------------------------------
        /// Read the major cities data file, parse it and set data.
        // -----------------------------------------------------------------------------------------
        void SetMajorCitiesData(const std::string& majorCitiesFileName);

        // -----------------------------------------------------------------------------------------
        /// helper function for << overload
        // -----------------------------------------------------------------------------------------
        friend std::ostream& operator<<(std::ostream& output, const GeoGridConfig& config);

};

} // namespace geopop
