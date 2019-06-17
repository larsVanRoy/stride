//
// Created by fj on 14.06.19.
//

#include "Controller.h"
#include "Location.h"
#include <cmath>

#include "../cpp/geopop/EpiGrid.h"
#include "../cpp/geopop/io/EpiReader.h"
#include "../cpp/geopop/io/EpiReaderFactory.h"
#include "memory"

namespace gui {

Controller::Controller(QObject *parent) : QObject(parent), m_app(nullptr), m_grid(nullptr), result({}), m_day(0), m_multiSelect({0,0}) {}

Controller::~Controller(){
    for(QObject* q : result) {
        delete q;
    }
}

void Controller::loadFile() {
    std::shared_ptr<geopop::EpiReader> reader = nullptr;
    try {
        reader = geopop::EpiReaderFactory::CreateEpiReader("epi-output.json");
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return;
    }
    m_grid = reader->Read();
};

QList<QObject*> Controller::getLocations() {
//    QList<QObject*> result;
    for(QObject* q : result) {
        delete q;
    }
    result = {};

    if(m_grid.get() == nullptr){
        return result;
    }

    unsigned int largest = 0;
    unsigned int smallest = 100000000000;
    for(size_t i = 0; i < m_grid->size(); ++i) {
        auto loc = m_grid->operator[](i);
        if (largest < loc->GetPopCount()) {
            largest = loc->GetPopCount();
        }
        if (smallest > loc->GetPopCount()) {
            smallest = loc->GetPopCount();
        }
        try {
            result.push_back(
                    new Location(loc->GetCoordinate().get<0>(), loc->GetCoordinate().get<1>(), loc->GetPopCount(),
                                 GetIllDouble(loc->GetID()), loc->GetID()));
        }
        catch(...){
            std::cerr << "Whoopsie" << std::endl;
            throw "nope";
        }
    }
    std::cout << "\nLargest:  " << largest << std::endl;
    std::cout << "Smallest: " << smallest << std::endl;
    return result;
}

QString Controller::GetLatitude(unsigned int ID) {
    return QString::number(m_grid->GetById(ID)->GetCoordinate().get<0>(), 'f', 5);
}

QString Controller::GetLongitude(unsigned int ID) {
    return QString::number(m_grid->GetById(ID)->GetCoordinate().get<1>(), 'f', 5);
}

QString Controller::GetLocationName(unsigned int ID) {
    return QString::fromStdString(m_grid->GetById(ID)->GetName());
}

QString Controller::GetPopCount(unsigned int ID) {
    return QString::number(m_grid->GetById(ID)->GetPopCount());
}

QString Controller::GetIll(unsigned int ID) {
    return QString::number(GetIllDouble(ID)*100, 'f', 2);
}

double Controller::GetIllDouble(unsigned int ID) {
    double ill = 0.0;
    try {
        std::shared_ptr<stride::HealthPool> h = m_grid->GetById(ID)->GetPoolStatus(m_day)->getStatus(
                stride::ContactType::Id::Household);
        ill += h->operator[](stride::HealthStatus::Infectious);
        ill += h->operator[](stride::HealthStatus::InfectiousAndSymptomatic);
        ill += h->operator[](stride::HealthStatus::Symptomatic);
    }
    catch(...){
        std::cerr << "Whoopsie2 " << ID << std::endl;
        throw "nope2";
    }
    return ill;
}

void Controller::nextDay() {
    if(m_day < m_grid->operator[](0)->maxDays() - 1){
        m_day++;
        QMetaObject::invokeMethod(m_app, "refreshMap");
    }
}

void Controller::previousDay() {
    if(m_day > 0){
        m_day--;
        QMetaObject::invokeMethod(m_app, "refreshMap");
    }
}

void Controller::SetInfo() {
    std::cout << "Is nullptr: " << (m_app == nullptr) << std::endl;
    std::cout << "Object window type: " << m_app->isWindowType() << std::endl;
    std::cout << "Object name: " << m_app->objectName().toStdString() << std::endl;

    QMetaObject::invokeMethod(m_app, "test");
}

unsigned int Controller::GetCurrentDay() {
    return m_day;
}

void Controller::InitializeMultiSelect(double longitude, double latitude) {
    m_multiSelect = {longitude, latitude};
}

void Controller::BoxSelect(double longitude, double latitude) {
    std::cout << "Box: {" << m_multiSelect.get<0>() << ", " <<m_multiSelect.get<1>() << "} and {" << longitude << ", " << latitude << "}" << std::endl;
    std::set<const geopop::EpiLocation*> selectedLoc = m_grid->LocationsInBox(m_multiSelect.get<0>(), m_multiSelect.get<1>(), longitude, latitude);
    for(const geopop::EpiLocation* epi : selectedLoc){
        std::cout << "name: " << epi->GetName() << "\n";
    }
}

void Controller::RadiusSelect(double distance) {

    std::cout << "Rad: {" << m_multiSelect.get<0>() << ", " <<m_multiSelect.get<1>() << "} and {" <<
            distance << "}" << std::endl;

    const geopop::EpiLocation center(-1, -1, m_multiSelect);
    std::vector<const geopop::EpiLocation*> selectedLoc = m_grid->LocationsInRadius(center, distance);
    for(const geopop::EpiLocation* epi : selectedLoc){
        std::cout << "name: " << epi->GetName() << "\n";
    }
}
}