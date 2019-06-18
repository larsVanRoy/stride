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
m_selectedLocations({}), m_selection(StateSelect::unassigned),
m_selectedAgeBrackets(stride::AgeBrackets::AgeBracketList),
m_selectedHealthStatus({})
{
    m_selectedHealthStatus = {stride::HealthStatus::Infectious, stride::HealthStatus::Symptomatic, stride::HealthStatus::InfectiousAndSymptomatic};
}

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
        result.push_back(new Location(loc->GetCoordinate().get<0>(), loc->GetCoordinate().get<1>(), loc->GetPopCount(),
                             GetIllDouble(loc->GetID()), loc->GetID()));
    }
    return result;
}

QString Controller::GetName(unsigned int ID) {
    return QString("test GetName");
}

QString Controller::GetPopCount(unsigned int ID) {
    return QString::number(m_grid->GetById(ID)->GetPopCount());
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
    for(stride::AgeBrackets::AgeBracket ageBracket : stride::AgeBrackets::AgeBracketList) {
        std::shared_ptr<stride::HealthPool> h = p->getStatus(ageBracket);
        for(const stride::HealthStatus& hs : m_selectedHealthStatus) {
            color += h->operator[](hs);
        }
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

std::string DoubleToString(double d, unsigned int precision = 0){
    double rounding = pow(10, precision);
    d = lround(d*rounding)/rounding;
    // get numbers before point:
    double tmp = d;
    unsigned int size = 0;
    while(tmp > 1){
        size++;
        tmp /= 10;
    }
    std::string str = std::to_string(d);

    if(precision > 0) {
        precision += 1;     //add comma
    }

    str = str.substr(0,size+precision);
    return str;
}

bool Controller::SetObjectPercentage(const std::string& objectName, double percentage, unsigned int precision) {

    return SetObjectText(objectName, DoubleToString(percentage*100, precision) + "%");
}

void Controller::DisplayInfo() {
    switch(m_selection){
        case StateSelect::unassigned: {
            return;
        }
        case StateSelect::location: {
            SetObjectText("locName", m_selectedLocations.at(0)->GetName());
            break;
        }
        case StateSelect::box: {
            SetObjectText("locName", "Box");
            break;
        }
        case StateSelect::radius: {
            SetObjectText("locName", "Radius");
            break;
        }
    }

    double tot_population   = 0;
    double tot_daycare      = 0;
    double tot_preschool    = 0;
    double tot_k12school    = 0;
    double tot_college      = 0;
    double tot_workplace    = 0;
    double tot_retired      = 0;
    double population   = 0;
    double daycare      = 0;
    double preschool    = 0;
    double k12school    = 0;
    double college      = 0;
    double workplace    = 0;
    double retired      = 0;
    double total    = 0;

    std::cout << "\n";
    for(auto& loc : m_selectedLocations) {
        population  += loc->GetPopCount();
        daycare     = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::Daycare, m_selectedHealthStatus)   * loc->GetPopCount();
        preschool   = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::PreSchool, m_selectedHealthStatus) * loc->GetPopCount();
        k12school   = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::K12School, m_selectedHealthStatus) * loc->GetPopCount();
        college     = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::College, m_selectedHealthStatus)   * loc->GetPopCount();
        workplace   = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::Workplace, m_selectedHealthStatus) * loc->GetPopCount();
        retired     = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::Retired, m_selectedHealthStatus)   * loc->GetPopCount();
        total       += daycare + preschool + k12school + college + workplace + retired;

        std::cout << "loc:\n";
        double tmp;
        tmp = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::Daycare);

        std::cout << "\t" << daycare << "/" << tmp << std::endl;
        if(tmp != 0)
            tot_daycare += daycare;
        tmp = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::PreSchool);
        std::cout << "\t" << preschool << "/" << tmp << std::endl;
        if(tmp != 0)
            tot_preschool += preschool;
        tmp = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::K12School);
        std::cout << "\t" << k12school << "/" << tmp << std::endl;
        if(tmp != 0)
            tot_k12school += k12school;
        tmp = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::College);
        std::cout << "\t" << college << "/" << tmp << std::endl;
        if(tmp != 0)
            tot_college += college/tmp;
        tmp = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::Workplace);
        std::cout << "\t" << workplace << "/" << tmp << std::endl;
        if(tmp != 0)
            tot_workplace += workplace;
        tmp = loc->GetPoolStatus(m_day)->getPercentage(stride::AgeBrackets::AgeBracket::Retired);
        std::cout << "\t" << retired << "/" << tmp << std::endl;
        if(tmp != 0)
            tot_retired += retired;
    }
    std::cout << "\n";

    daycare     /= population;
    preschool   /= population;
    k12school   /= population;
    college     /= population;
    workplace   /= population;
    retired     /= population;
    total       /= population;

    unsigned int precision = 2;

    SetObjectText("populationValue", DoubleToString(population, 0));
    SetObjectPercentage("totalValue", total,precision);
    SetObjectPercentage("daycareValue", daycare,precision);
    SetObjectPercentage("preschoolValue", preschool,precision);
    SetObjectPercentage("k12schoolValue", k12school,precision);
    SetObjectPercentage("collegeValue", college,precision);
    SetObjectPercentage("workplaceValue", workplace, precision);
    SetObjectPercentage("retiredValue", retired, precision);

    QMetaObject::invokeMethod(m_app->findChild<QObject*>("sideRect"), "open");
}

void Controller::SetSelectedLocation(unsigned int ID) {
    if(m_selection == StateSelect::location) {
        if(ID == m_selectedLocations.at(0)->GetID()){
            // location already selected
            return;
        }
    }
    m_selection = StateSelect::location;
    m_selectedLocations = {m_grid->GetById(ID).get()};

    DisplayInfo();
}

unsigned int Controller::GetCurrentDay() {
    return m_day;
}

void Controller::InitializeMultiSelect(double longitude, double latitude) {
    m_multiSelect = {longitude, latitude};
}

void Controller::BoxSelect(double longitude, double latitude) {
    std::set<const geopop::EpiLocation<geopop::Coordinate>*> selectedLoc = m_grid->LocationsInBox(m_multiSelect.get<0>(), m_multiSelect.get<1>(), longitude, latitude);

    m_selectedLocations = {};
    m_selectedLocations.assign(selectedLoc.begin(), selectedLoc.end());
    m_selection = StateSelect::box;
    DisplayInfo();
}

void Controller::RadiusSelect(double distance) {
    const geopop::EpiLocation center(-1, -1, m_multiSelect);    // create dummy location
    std::vector<const geopop::EpiLocation<geopop::Coordinate>*> selectedLoc = m_grid->LocationsInRadius(center, distance);
    m_selectedLocations = selectedLoc;
    m_selection = StateSelect::radius;
    DisplayInfo();
}
}