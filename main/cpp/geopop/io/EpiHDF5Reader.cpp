//
// Created by benji on 16/06/19.
//

#include "EpiHDF5Reader.h"
#include "EpiGridHDF5Types.h"
#include "GeoGridHDF5Types.h"
namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace H5;

std::shared_ptr<EpiGrid> EpiHDF5Reader::Read()
{
        ReadLocations();
        ReadHistory();
        return m_grid;
}

void EpiHDF5Reader::ReadLocations()
{
        size_t locations_size;
        auto   dataset = m_file.openDataSet("Locations");
        ReadAttribute("size", &locations_size, dataset);
        std::vector<H5GeoLocation> locations(locations_size);
        ReadDataset("Locations", locations.data(), m_file);
        for (size_t i = 0; i < locations_size; ++i) {
                auto h5location  = locations[i];
                auto epilocation = std::make_shared<EpiLocation<Coordinate>>(
                    h5location.id, h5location.province, Coordinate{h5location.longitude, h5location.latitude},
                    string(h5location.name), h5location.population);
                m_grid->AddLocation(epilocation);
        }
}

void EpiHDF5Reader::ReadHistory()
{
        size_t timesteps_size;
        ReadAttribute("nrTimesteps", &timesteps_size, m_file);

        std::vector<unsigned int> timesteps(timesteps_size);
        ReadDataset("Timesteps", timesteps.data(), m_file);

        for(const auto& timestep: timesteps){
                ReadTimeStep(timestep);
        }
}

void EpiHDF5Reader::ReadTimeStep(unsigned int timestep) {

        unsigned nrLocations;
        auto group = m_file.openGroup("Steps").openGroup("Step"+to_string(timestep));
        ReadAttribute("nrLocations", &nrLocations, group);

        for(size_t i = 0; i < nrLocations; ++i){
                auto location_group = group.openGroup("Location"+to_string(i));
                ReadHistoryLocation(location_group);
        }
}

void EpiHDF5Reader::ReadHistoryLocation(const H5::Group& location_group) {
        unsigned int id;
        ReadAttribute("id", &id, location_group);
        auto loc = m_grid->GetById(id);
        ReadLocationPools(location_group, loc);
}

void EpiHDF5Reader::ReadLocationPools(const H5::Group& location_group, shared_ptr<EpiLocation<Coordinate>> location) {
        std::shared_ptr<PoolStatus> status = std::make_shared<stride::PoolStatus>();

        for (Id i : IdList) {
                std::vector<double> health_status(NumOfHealthStatus());
                auto group = location_group.openGroup("Pools");
                ReadDataset(ToString(i), health_status.data(), group);

                auto h = std::make_shared<stride::HealthPool>();
                for (size_t j = 0; j < health_status.size(); ++j){
                        h->setHealth(static_cast<HealthStatus >(j), health_status[j]);
                }

                status->addStatus(i, h);
        }
        location->AddPoolStatus(status);
}

template <typename T>
void EpiHDF5Reader::ReadAttribute(const std::string& name, T* data, const H5::H5Object& object)
{
        auto attr = object.openAttribute(name);
        switch (attr.getTypeClass()) {
        default: attr.read(attr.getDataType(), data); break;
        case H5T_COMPOUND: attr.read(attr.getCompType(), data); break;
        }
}

template <>
void EpiHDF5Reader::ReadAttribute(const std::string& name, std::string* data, const H5::H5Object& object)
{
        auto attr = object.openAttribute(name);
        attr.read(StrType(H5T_C_S1, H5T_VARIABLE), *data);
}
template <typename T>
void EpiHDF5Reader::ReadDataset(const std::string& name, T* data, const H5::H5Object& object)
{
        auto dataSet = object.openDataSet(name);
        switch (dataSet.getTypeClass()) {
        default: dataSet.read(data, dataSet.getDataType()); break;
        case H5T_STRING: dataSet.read(data, dataSet.getStrType()); break;
        case H5T_COMPOUND: dataSet.read(data, dataSet.getCompType()); break;
        }
}
void EpiHDF5Reader::Print() {
        for (unsigned int i = 0; i < m_grid->size(); ++i) {
                std::cout << "Location: " << i << "\n";
                std::cout << "\t" << m_grid-> operator[](i)->GetName() << "\n";
                std::cout << "\t" << m_grid-> operator[](i)->GetPopCount() << "\n";
                std::cout << "\t(" << m_grid->operator[](i)->GetCoordinate().get<0>() << ", "
                          << m_grid->         operator[](i)->GetCoordinate().get<1>() << ")\n";
                std::cout << "\t"
                          << m_grid->operator[](i)->GetPoolStatus(0)->operator[](stride::HealthStatus::Susceptible)[0]
                          << "\n";
                std::cout << "\t"
                          << m_grid->operator[](i)->GetPoolStatus(0)->operator[](stride::HealthStatus::Exposed)[0]
                          << "\n";
                std::cout << "\t"
                          << m_grid->operator[](i)->GetPoolStatus(0)->operator[](stride::HealthStatus::Infectious)[0]
                          << "\n";
                std::cout << "\t"
                          << m_grid->operator[](i)->GetPoolStatus(0)->operator[](stride::HealthStatus::Symptomatic)[0]
                          << "\n";
                std::cout << "\t"
                          << m_grid->operator[](i)->GetPoolStatus(0)->operator[](
                                 stride::HealthStatus::InfectiousAndSymptomatic)[0]
                          << "\n";
                std::cout << "\t"
                          << m_grid->operator[](i)->GetPoolStatus(0)->operator[](stride::HealthStatus::Recovered)[0]
                          << "\n";
                std::cout << "\t"
                          << m_grid->operator[](i)->GetPoolStatus(0)->operator[](stride::HealthStatus::Immune)[0]
                          << "\n";
                std::cout << std::endl;
        }
}

}