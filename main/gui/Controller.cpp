//
// Created by fj on 14.06.19.
//

#include "Controller.h"
#include "Location.h"


#include "../cpp/geopop/EpiGrid.h"
#include "../cpp/geopop/io/EpiReader.h"
#include "../cpp/geopop/io/EpiReaderFactory.h"
#include "memory"

namespace gui {

Controller::Controller(QObject *parent) : QObject(parent), m_grid(nullptr), result({}), m_day(0) {}

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
        std::shared_ptr<geopop::EpiLocation> loc = m_grid->operator[](i);
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
    std::cout << "old day: " << m_day << "\n";
    std::cout << "max day: " << m_grid->operator[](0)->maxDays() << std::endl;
    if(m_day < m_grid->operator[](0)->maxDays() - 1){
        m_day++;
    }
    std::cout << "new day: " << m_day << std::endl;
    std::cout << "\n";
}

void Controller::previousDay() {
    std::cout << "old day: " << m_day << "\n";
    std::cout << "min day: " << 0 << std::endl;
    if(m_day > 0){
        m_day--;
    }
    std::cout << "new day: " << m_day << std::endl;
    std::cout << "\n";
}

unsigned int Controller::GetCurrentDay() {
    return m_day;
}
}