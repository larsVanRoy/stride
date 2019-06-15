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

Controller::Controller(QObject *parent) : QObject(parent), m_grid(nullptr), result({}) {}

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
    std::cout << "Grid size: " << m_grid->size();
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
        if(largest < loc->GetPopCount()){
            largest = loc->GetPopCount();
        }
        if(smallest > loc->GetPopCount()){
            smallest = loc->GetPopCount();
        }
        result.push_back(new Location(loc->GetCoordinate().get<0>(), loc->GetCoordinate().get<1>(), loc->GetPopCount(), 0.4));
    }
    std::cout << "\nLargest:  " << largest << std::endl;
    std::cout << "Smallest: " << smallest << std::endl;
    return result;
}

}