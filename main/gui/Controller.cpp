//
// Created by fj on 14.06.19.
//

#include "Controller.h"
#include "Location.h"
#include <cmath>


#include "../cpp/contact/AgeBrackets.h"
#include "../cpp/contact/ContactType.h"
#include "../cpp/geopop/EpiGrid.h"
#include "../cpp/geopop/io/EpiReader.h"
#include "../cpp/geopop/io/EpiReaderFactory.h"
#include "memory"

namespace gui {

Controller::Controller(QObject *parent) : QObject(parent), m_app(nullptr),
m_grid(nullptr), result({}), m_day(0), m_multiSelect({0,0}), show("all"),
m_selectedLocationIdSet(false) {}

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
    unsigned int smallest = -1;
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

QString Controller::GetName(unsigned int ID) {
    return QString("test GetName");
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
    std::shared_ptr<stride::PoolStatus> p = m_grid->GetById(ID)->GetPoolStatus(m_day);

    for(stride::AgeBrackets::AgeBracket ageBracket : stride::AgeBrackets::AgeBracketList) {
        std::shared_ptr<stride::HealthPool> h = p->getStatus(ageBracket);
        ill += h->operator[](stride::HealthStatus::Infectious);
        ill += h->operator[](stride::HealthStatus::InfectiousAndSymptomatic);
        ill += h->operator[](stride::HealthStatus::Symptomatic);
    }

    return ill;
}

double Controller::GetColor(unsigned int ID) {
    double color = 0;
    std::shared_ptr<stride::PoolStatus> p = m_grid->GetById(ID)->GetPoolStatus(m_day);
    if(show == "all") {
        for(stride::AgeBrackets::AgeBracket ageBracket : stride::AgeBrackets::AgeBracketList) {
            std::shared_ptr<stride::HealthPool> h = p->getStatus(ageBracket);
            color += h->operator[](stride::HealthStatus::Infectious);
            color += h->operator[](stride::HealthStatus::InfectiousAndSymptomatic);
            color += h->operator[](stride::HealthStatus::Symptomatic);
            color += h->operator[](stride::HealthStatus::Recovered);
        }
    }
    else{
        std::cout << "show: " << show << std::endl;
    }
    return color;
}

void Controller::nextDay() {
    if(m_day < m_grid->operator[](0)->maxDays() - 1){
        m_day++;
        QMetaObject::invokeMethod(m_app, "refreshMap");
        DisplayInfo();
    }
}

void Controller::previousDay() {
    if(m_day > 0){
        m_day--;
        QMetaObject::invokeMethod(m_app, "refreshMap");
        DisplayInfo();
    }
}

void Controller::SetInfo() {
    QObject* locName = m_app->findChild<QObject*>(QStringLiteral("locName"));
    if(locName) {
        std::cout << "Setting locName" << std::endl;
        locName->setProperty("text", QStringLiteral("woop"));
    }
    else{
        std::cerr << "locname not found" << std::endl;
    }
}

bool Controller::SetObjectText(const std::string& objectName, const std::string& text) {
    QObject* obj = m_app->findChild<QObject*>(QString::fromStdString(objectName));
    if(obj) {
        obj->setProperty("text", QString::fromStdString(text));
        return true;
    }
    std::cout << "Could not find child: " << objectName << std::endl;
    return false;   //failed
}

bool Controller::SetObjectPercentage(const std::string& objectName, double percentage, unsigned int precision) {
    double rounding = pow(10, precision);
    percentage = lround(percentage*rounding*100)/rounding;
    // get numbers before point:
    double tmp = percentage;
    unsigned int size = 0;
    while(tmp > 1){
        size++;
        tmp /= 10;
    }
    std::string str = std::to_string(percentage);

    if(precision > 0) {
        precision += 1;     //add comma
    }

    str = str.substr(0,size+precision);
    return SetObjectText(objectName, str + "%");
}

void Controller::DisplayInfo() {
    if(! m_selectedLocationIdSet){
        return;
    }

    const auto& loc = m_grid->GetById(m_selectedLocationId);
    double daycare = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::Daycare);
    double preschool = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::PreSchool);
    double k12school = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::K12School);
    double college = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::College);
    double workplace = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::Workplace);
    double retired = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::Retired);
    double total = daycare + preschool + k12school + college + workplace + retired;

    unsigned int precision = 2;
    SetObjectText("locName", loc->GetName());
    SetObjectText("populationValue", std::to_string(loc->GetPopCount()));
    SetObjectPercentage("totalValue", total,precision);
    SetObjectPercentage("daycareValue", daycare,precision);
    SetObjectPercentage("preschoolValue", preschool,precision);
    SetObjectPercentage("k12schoolValue", k12school,precision);
    SetObjectPercentage("collegeValue", college,precision);
    SetObjectPercentage("workplaceValue", workplace, precision);
    SetObjectPercentage("retiredValue", retired, precision);
}

void Controller::SetSelectedLocation(unsigned int ID) {
    const auto& loc = m_grid->GetById(ID);
    if(m_selectedLocationId != loc->GetID()){
        m_selectedLocationId = loc->GetID();
        m_selectedLocationIdSet = true;
        DisplayInfo();
    }
}

unsigned int Controller::GetCurrentDay() {
    return m_day;
}

void Controller::InitializeMultiSelect(double longitude, double latitude) {
    m_multiSelect = {longitude, latitude};
}

void Controller::BoxSelect(double longitude, double latitude) {
    std::cout << "Box: {" << m_multiSelect.get<0>() << ", " <<m_multiSelect.get<1>() << "} and {" << longitude << ", " << latitude << "}" << std::endl;
    std::set<const geopop::EpiLocation<geopop::Coordinate>*> selectedLoc = m_grid->LocationsInBox(m_multiSelect.get<0>(), m_multiSelect.get<1>(), longitude, latitude);
    for(const geopop::EpiLocation<geopop::Coordinate>* epi : selectedLoc){
        std::cout << "name: " << epi->GetName() << "\n";
    }
}

void Controller::RadiusSelect(double distance) {

    std::cout << "Rad: {" << m_multiSelect.get<0>() << ", " <<m_multiSelect.get<1>() << "} and {" <<
            distance << "}" << std::endl;

    const geopop::EpiLocation center(-1, -1, m_multiSelect);
    std::vector<const geopop::EpiLocation<geopop::Coordinate>*> selectedLoc = m_grid->LocationsInRadius(center, distance);
    for(const geopop::EpiLocation<geopop::Coordinate>* epi : selectedLoc){
        std::cout << "name: " << epi->GetName() << "\n";
    }
}
}