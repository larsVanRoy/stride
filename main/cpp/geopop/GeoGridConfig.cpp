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

#include "GeoGridConfig.h"

#include "contact/AgeBrackets.h"
#include "contact/ContactType.h"
#include "geopop/io/HouseholdReader.h"
#include "geopop/io/WorkplaceReader.h"
#include "geopop/io/ReaderFactory.h"
#include "util/FileSys.h"
#include "util/StringUtils.h"
#include "util/CSV.h"

#include <boost/property_tree/ptree.hpp>
#include <cmath>
#include <iomanip>

namespace geopop {

using namespace std;
using namespace boost::property_tree;
using namespace stride::AgeBrackets;
using namespace stride::ContactType;
using stride::util::intToDottedString;

GeoGridConfig::GeoGridConfig() : param{}, refHHs{}, regionInfo{}, majorCities{} {
    refHHs[0] = refHH{};
}

GeoGridConfig::GeoGridConfig(const ptree& configPt) : GeoGridConfig()
{
        const auto pt                      = configPt.get_child("run.geopop_gen");
        param.pop_size                     = pt.get<unsigned int>("population_size");
        param.participation_college        = pt.get<double>("participation_college");
        param.fraction_workplace_commuters = pt.get<double>("fraction_workplace_commuters");
        param.fraction_college_commuters   = pt.get<double>("fraction_college_commuters");
        param.participation_workplace       = pt.get<double>("particpation_workplace");

        people[Id::Daycare]            = pt.get<unsigned int>("people_per_Daycare", 10U);
        people[Id::PreSchool]          = pt.get<unsigned int>("people_per_PreSchool", 120U);
        people[Id::K12School]          = pt.get<unsigned int>("people_per_K12School", 500U);
        people[Id::College]            = pt.get<unsigned int>("people_per_College", 3000U);
        people[Id::Workplace]          = pt.get<unsigned int>("people_per_Workplace", 20U);
        people[Id::PrimaryCommunity]   = pt.get<unsigned int>("people_per_PrimaryCommunity", 2000U);
        people[Id::SecondaryCommunity] = pt.get<unsigned int>("people_per_SecondaryCommunity", 2000U);

        pools[Id::Daycare]   = pt.get<unsigned int>("pools_per_Daycare", 1U);
        pools[Id::PreSchool] = pt.get<unsigned int>("pools_per_PreSchool", 6U);
        pools[Id::K12School] = pt.get<unsigned int>("pools_per_K12School", 25U);
        pools[Id::College]   = pt.get<unsigned int>("pools_per_College", 20U);
}

void GeoGridConfig::SetData(const string& householdsFileName)
{
        auto householdsReader = ReaderFactory::CreateHouseholdReader(householdsFileName);
        auto& householdRef = refHHs[0];
        householdsReader->SetReferenceHouseholds(householdRef.person_count, householdRef.ages);
        const auto popSize = param.pop_size;

        //----------------------------------------------------------------
        // Determine age makeup of reference houshold population.
        //----------------------------------------------------------------
        const auto ref_p_count   = householdRef.person_count;
        const auto averageHhSize = static_cast<double>(ref_p_count) / static_cast<double>(householdRef.ages.size());

        auto ref_daycare   = 0U;
        auto ref_preschool = 0U;
        auto ref_k12school = 0U;
        auto ref_college   = 0U;
        auto ref_workplace = 0U;
        for (const auto& hhAgeProfile : householdRef.ages) {
                for (const auto& age : hhAgeProfile) {
                        if (Daycare::HasAge(age)) {
                                ref_daycare++;
                        }
                        if (PreSchool::HasAge(age)) {
                                ref_preschool++;
                        }
                        if (K12School::HasAge(age)) {
                                ref_k12school++;
                        }
                        if (College::HasAge(age)) {
                                ref_college++;
                        }
                        if (Workplace::HasAge(age)) {
                                ref_workplace++;
                        }
                }
        }
        //----------------------------------------------------------------
        // Scale up to the generated population size.
        //----------------------------------------------------------------
        const auto fraction_daycare_age   = static_cast<double>(ref_daycare) / static_cast<double>(ref_p_count);
        const auto fraction_preschool_age = static_cast<double>(ref_preschool) / static_cast<double>(ref_p_count);
        const auto fraction_k12school_age = static_cast<double>(ref_k12school) / static_cast<double>(ref_p_count);
        const auto fraction_college_age   = static_cast<double>(ref_college) / static_cast<double>(ref_p_count);
        const auto fraction_workplace_age = static_cast<double>(ref_workplace) / static_cast<double>(ref_p_count);

        const auto age_count_daycare   = static_cast<unsigned int>(floor(popSize * fraction_daycare_age));
        const auto age_count_preschool = static_cast<unsigned int>(floor(popSize * fraction_preschool_age));
        const auto age_count_k12school = static_cast<unsigned int>(floor(popSize * fraction_k12school_age));
        const auto age_count_college   = static_cast<unsigned int>(floor(popSize * fraction_college_age));
        const auto age_count_workplace = static_cast<unsigned int>(floor(popSize * fraction_workplace_age));

        auto& provInfo = regionInfo[0];

        provInfo.popcount_daycare = static_cast<unsigned int>(floor(param.participation_daycare * age_count_daycare));

        provInfo.popcount_preschool = static_cast<unsigned int>(
            floor(param.participation_preschool * age_count_preschool));

        provInfo.popcount_k12school = age_count_k12school;

        provInfo.popcount_college = static_cast<unsigned int>(floor(param.participation_college * age_count_college));

        provInfo.popcount_workplace = static_cast<unsigned int>(
            floor(param.participation_workplace * (age_count_workplace - provInfo.popcount_college)));

        provInfo.count_households = static_cast<unsigned int>(floor(static_cast<double>(popSize) / averageHhSize));

        provInfo.popcount = popSize;
}

void GeoGridConfig::SetData(const std::map<unsigned int, string>& householdFileNames, GeoGrid& geoGrid)
{
        //----------------------------------------------------------------
        // Read the given householdFiles
        //----------------------------------------------------------------
        for(auto& pair: householdFileNames){
                unsigned int provinceID = pair.first;
                const string& householdFile = pair.second;

                refHHs[provinceID] = refHH{};
                auto& householdRef = refHHs[provinceID];

                auto householdReader = ReaderFactory::CreateHouseholdReader(householdFile);
                householdReader->SetReferenceHouseholds(householdRef.person_count, householdRef.ages);

                //----------------------------------------------------------------
                // Determine age makeup of reference houshold population.
                //----------------------------------------------------------------
                const auto ref_p_count   = householdRef.person_count;
                const auto averageHhSize = static_cast<double>(ref_p_count) / static_cast<double>(householdRef.ages.size());

                auto ref_daycare   = 0U;
                auto ref_preschool = 0U;
                auto ref_k12school = 0U;
                auto ref_college   = 0U;
                auto ref_workplace = 0U;
                for (const auto& hhAgeProfile : householdRef.ages) {
                        for (const auto& age : hhAgeProfile) {
                                if (Daycare::HasAge(age)) {
                                        ref_daycare++;
                                }
                                if (PreSchool::HasAge(age)) {
                                        ref_preschool++;
                                }
                                if (K12School::HasAge(age)) {
                                        ref_k12school++;
                                }
                                if (College::HasAge(age)) {
                                        ref_college++;
                                }
                                if (Workplace::HasAge(age)) {
                                        ref_workplace++;
                                }
                        }
                }
                //----------------------------------------------------------------
                // Scale up to the generated population size.
                //----------------------------------------------------------------
                const auto fraction_daycare_age   = static_cast<double>(ref_daycare) / static_cast<double>(ref_p_count);
                const auto fraction_preschool_age = static_cast<double>(ref_preschool) / static_cast<double>(ref_p_count);
                const auto fraction_k12school_age = static_cast<double>(ref_k12school) / static_cast<double>(ref_p_count);
                const auto fraction_college_age   = static_cast<double>(ref_college) / static_cast<double>(ref_p_count);
                const auto fraction_workplace_age = static_cast<double>(ref_workplace) / static_cast<double>(ref_p_count);

                unsigned int popSize = 0;
                if (provinceID == 11){
                        for(unsigned int i = 0; i < geoGrid.size(); ++i){
                                const auto& loc = geoGrid[i];
                                if(find(majorCities.begin(), majorCities.end(), loc->GetName()) != majorCities.end()){
                                        popSize += loc->GetPopCount();
                                }
                        }
                }
                else{
                        for(unsigned int i = 0; i < geoGrid.size(); ++i){
                                const auto& loc = geoGrid[i];
                                if(loc->GetProvince() == provinceID){
                                        popSize += loc->GetPopCount();
                                }
                        }
                }

                const auto age_count_daycare   = static_cast<unsigned int>(floor(popSize * fraction_daycare_age));
                const auto age_count_preschool = static_cast<unsigned int>(floor(popSize * fraction_preschool_age));
                const auto age_count_k12school = static_cast<unsigned int>(floor(popSize * fraction_k12school_age));
                const auto age_count_college   = static_cast<unsigned int>(floor(popSize * fraction_college_age));
                const auto age_count_workplace = static_cast<unsigned int>(floor(popSize * fraction_workplace_age));

                auto& provInfo = regionInfo[provinceID];

                provInfo.popcount_daycare = static_cast<unsigned int>(floor(param.participation_daycare * age_count_daycare));

                provInfo.popcount_preschool = static_cast<unsigned int>(
                        floor(param.participation_preschool * age_count_preschool));

                provInfo.popcount_k12school = age_count_k12school;

                provInfo.popcount_college = static_cast<unsigned int>(floor(param.participation_college * age_count_college));

                provInfo.popcount_workplace = static_cast<unsigned int>(
                        floor(param.participation_workplace * (age_count_workplace - provInfo.popcount_college)));

                provInfo.count_households = static_cast<unsigned int>(floor(static_cast<double>(popSize) / averageHhSize));

                provInfo.popcount = popSize;
        }
}

void GeoGridConfig::SetWorkplaceData(const std::string &workplaceFileName) 
{
        auto workplaceReader = ReaderFactory::CreateWorkplaceReader(workplaceFileName);
        workplaceReader->SetReferenceWorkplaces(param.workplace_distribution);
}

void GeoGridConfig::SetMajorCitiesData(const std::string &majorCitiesFileName)
{
        auto majorCitiesReader = ReaderFactory::CreateMajorCitiesReader(majorCitiesFileName);
        majorCitiesReader->SetReferenceCities(majorCities);
}

ostream& operator<<(ostream& out, const GeoGridConfig& config)
{
        const int w = 53;
        out.setf(std::ios_base::left, std::ios_base::adjustfield);
        out << "Input:" << endl;
        out << setw(w) << "Fraction college commuters:" << config.param.fraction_college_commuters << "\n";
        out << setw(w) << "Fraction workplace commuters:" << config.param.fraction_workplace_commuters << "\n";
        out << setw(w) << "Participation fraction of daycare:" << config.param.participation_daycare << "\n";
        out << setw(w) << "Participation fraction of preschool:" << config.param.participation_preschool << "\n";
        out << setw(w) << "Participation fraction of college:" << config.param.participation_college << "\n";
        out << setw(w) << "Participation fraction of workplace:" << config.param.participation_workplace << "\n";
        out << setw(w) << "Number of major cities" << config.majorCities.size() << "\n";
        out << setw(w) << "Target population size" << intToDottedString(config.param.pop_size) << "\n"
            << "\n";
        out << "Calculated:"
            << "\n";
        for(const auto& pair: config.regionInfo){
            out << setw(w) << "for region:" << config.idNames.at(pair.first) << "\n";
            const auto& poolInfo = pair.second;
            out << setw(w) << "    Daycare toddler count:" << intToDottedString(poolInfo.popcount_daycare) << "\n";
            out << setw(w) << "    PreSchool student count:" << intToDottedString(poolInfo.popcount_preschool) << "\n";
            out << setw(w) << "    K12School student count:" << intToDottedString(poolInfo.popcount_k12school) << "\n";
            out << setw(w) << "    College student count:" << intToDottedString(poolInfo.popcount_college) << "\n";
            out << setw(w) << "    Workplace person count:" << intToDottedString(poolInfo.popcount_workplace) << "\n";
            out << endl;
        }

        return out;
}

} // namespace geopop
