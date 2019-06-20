//
// Created by fj on 14.06.19.
//

#include "Controller.h"
#include "Location.h"
#include <cmath>
#include <QVariant>


#include "../cpp/contact/AgeBrackets.h"
#include "../cpp/contact/ContactType.h"
#include "../cpp/geopop/EpiGrid.h"
#include "../cpp/geopop/io/EpiReader.h"
#include "../cpp/geopop/io/EpiReaderFactory.h"
#include "memory"

namespace gui {

using namespace stride;
using namespace stride::AgeBrackets;


Controller::Controller(std::shared_ptr<geopop::EpiGrid> grid) :
QObject(nullptr), m_selectedLocations({}), m_selectedAgeBrackets(AgeBracketList),
m_selectedHealthStatus({}), m_selection(StateSelect::unassigned), m_day(0),
m_maxDay(0),  m_step(1), m_multiSelect({0,0}), result({}),
m_grid(grid), m_app(nullptr)
{
    m_selectedHealthStatus = {
            stride::HealthStatus::Symptomatic,
            stride::HealthStatus::Infectious,
            stride::HealthStatus::Recovered,
            stride::HealthStatus::InfectiousAndSymptomatic};
    m_maxDay = m_step*m_grid->operator[](0)->maxDays();
}

Controller::~Controller(){
    for(QObject* q : result) {
        delete q;
    }
}

void Controller::Initialize(QObject* rootwindow) {
    m_app = rootwindow;
    Update();
}

QList<QObject*> Controller::getLocations() {
    for(QObject* q : result) {
        delete q;
    }
    result = {};

    if(m_grid == nullptr){
        return result;
    }

    unsigned int largest = 0;
    unsigned int smallest = std::numeric_limits<unsigned int>::lowest();

    for(size_t i = 0; i < m_grid->size(); ++i) {
        auto loc = m_grid->operator[](i);
        if (largest < loc->GetPopCount()) {
            largest = loc->GetPopCount();
        }
        if (smallest > loc->GetPopCount()) {
            smallest = loc->GetPopCount();
        }
        result.push_back(new Location(loc->GetCoordinate().get<0>(), loc->GetCoordinate().get<1>(), loc->GetPopCount(), loc->GetID()));
    }
    return result;
}

QString Controller::GetPopCount(unsigned int ID) {
    return QString::number(m_grid->GetById(ID)->GetPopCount());
}

double Controller::GetColor(unsigned int ID) {

    double color = 0;
    double bracketSize = 0;
    std::shared_ptr<stride::PoolStatus> p = m_grid->GetById(ID)->GetPoolStatus(m_day);
    for(stride::AgeBrackets::AgeBracket ageBracket : m_selectedAgeBrackets) {
        const auto& h = p->getStatus(ageBracket);
        color += h->sum(m_selectedHealthStatus);
        bracketSize += h->sum(stride::HealthStatusList);
    }
    return color/bracketSize;
}

void Controller::nextDay() {
    if(m_day < m_grid->operator[](0)->maxDays() - 1){
        m_day++;
        Update();
    }
}

void Controller::previousDay() {
    if(m_day > 0){
        m_day--;
        Update();
    }
}

void Controller::DisplayDay(){
    QObject* dayObject = m_app->findChild<QObject*>("currentDay");
    if(dayObject)
        dayObject->setProperty("text", QVariant(GetCurrentDay()));
}

bool Controller::SetObjectText(const std::string& objectName, const std::string& text) {
    QObject* obj = m_app->findChild<QObject*>(QString::fromStdString(objectName));
    if(obj) {
        obj->setProperty("text", QVariant(QString::fromStdString(text)));
        return true;
    }
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
    if(str.size() == 0){
        return "0";
    }
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

    std::vector<double> percentages(stride::AgeBrackets::NumOfAgeBrackets()+1, 0);
    std::vector<double> total_populations(stride::AgeBrackets::NumOfAgeBrackets()+1, 0);

    std::vector<double> temp_percentage(stride::AgeBrackets::NumOfAgeBrackets(), 0); //temporary
    std::vector<double> temp_population(stride::AgeBrackets::NumOfAgeBrackets(), 0); //temporary

    for(const auto& loc : m_selectedLocations) {
        for(const auto& b : m_selectedAgeBrackets) {
            percentages[ToSize(b)]           = loc->GetPoolStatus(m_day)->getPercentage(b, m_selectedHealthStatus)*loc->GetPopCount();
            percentages[NumOfAgeBrackets()] += percentages[ToSize(b)];

            total_populations[ToSize(b)]           = loc->GetPoolStatus(m_day)->getPercentage(b)*loc->GetPopCount();
            total_populations[NumOfAgeBrackets()]  += total_populations[ToSize(b)];
        }
    }

    double tot_daycare     = percentages[0]/total_populations[0];
    double tot_preschool   = percentages[1]/total_populations[1];
    double tot_k12school   = percentages[2]/total_populations[2];
    double tot_college     = percentages[3]/total_populations[3];
    double tot_workplace   = percentages[4]/total_populations[4];
    double tot_retired     = percentages[5]/total_populations[5];
    double tot_total       = percentages[6]/total_populations[6];


    unsigned int precision = 2;

    SetObjectText("populationValue",DoubleToString(total_populations[6], 0));
    SetObjectPercentage("totalValue",     tot_total,precision);
    SetObjectPercentage("daycareValue",   tot_daycare,precision);
    SetObjectPercentage("preschoolValue", tot_preschool,precision);
    SetObjectPercentage("k12schoolValue", tot_k12school,precision);
    SetObjectPercentage("collegeValue",   tot_college,precision);
    SetObjectPercentage("workplaceValue", tot_workplace, precision);
    SetObjectPercentage("retiredValue",   tot_retired, precision);

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

QString Controller::GetCurrentDay() {
    unsigned int day = m_day*m_step;
    return QString::fromStdString(std::to_string(day+1) + "/" + std::to_string(m_maxDay));
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
    m_selectedLocations = {};
    m_selectedLocations = m_grid->LocationsInRadius(center, distance);
    m_selection = StateSelect::radius;
    DisplayInfo();
}

void Controller::Update() {
    DisplayDay();
    DisplayInfo();
    QMetaObject::invokeMethod(m_app, "refreshMap");
}

void Controller::AgeSelect(QString age) {
    if(age.toStdString() == "daycare"){
        m_selectedAgeBrackets.insert(stride::AgeBrackets::AgeBracket::Daycare);
    }
    else if(age.toStdString() == "preschool"){
        m_selectedAgeBrackets.insert(stride::AgeBrackets::AgeBracket::PreSchool);
    }
    else if(age.toStdString() == "k12school"){
        m_selectedAgeBrackets.insert(stride::AgeBrackets::AgeBracket::K12School);
    }
    else if(age.toStdString() == "college"){
        m_selectedAgeBrackets.insert(stride::AgeBrackets::AgeBracket::College);
    }
    else if(age.toStdString() == "workplace"){
        m_selectedAgeBrackets.insert(stride::AgeBrackets::AgeBracket::Workplace);
    }
    else if(age.toStdString() == "retired"){
        m_selectedAgeBrackets.insert(stride::AgeBrackets::AgeBracket::Retired);
    }
    Update();
}

void Controller::AgeDeSelect(QString age) {
    if(age.toStdString() == "daycare"){
        m_selectedAgeBrackets.erase(stride::AgeBrackets::AgeBracket::Daycare);
    }
    if(age.toStdString() == "preschool"){
        m_selectedAgeBrackets.erase(stride::AgeBrackets::AgeBracket::PreSchool);
    }
    if(age.toStdString() == "k12school"){
        m_selectedAgeBrackets.erase(stride::AgeBrackets::AgeBracket::K12School);
    }
    if(age.toStdString() == "college"){
        m_selectedAgeBrackets.erase(stride::AgeBrackets::AgeBracket::College);
    }
    if(age.toStdString() == "workplace"){
        m_selectedAgeBrackets.erase(stride::AgeBrackets::AgeBracket::Workplace);
    }
    if(age.toStdString() == "retired"){
        m_selectedAgeBrackets.erase(stride::AgeBrackets::AgeBracket::Retired);
    }
    Update();
}

void Controller::HealthSelect(QString health) {
    if(health.toStdString() == "immune"){
        m_selectedHealthStatus.insert(stride::HealthStatus::Immune);
    }
    else if(health.toStdString() == "exposed"){
        m_selectedHealthStatus.insert(stride::HealthStatus::Exposed);
    }
    else if(health.toStdString() == "infectious"){
        m_selectedHealthStatus.insert(stride::HealthStatus::Infectious);
    }
    else if(health.toStdString() == "symptomatic"){
        m_selectedHealthStatus.insert(stride::HealthStatus::Symptomatic);
    }
    else if(health.toStdString() == "recovered"){
        m_selectedHealthStatus.insert(stride::HealthStatus::Recovered);
    }
    else if(health.toStdString() == "susceptible"){
        m_selectedHealthStatus.insert(stride::HealthStatus::Susceptible);
    }
    else if(health.toStdString() == "infectious and symptomatic"){
        m_selectedHealthStatus.insert(stride::HealthStatus::InfectiousAndSymptomatic);
    }
    Update();
}

void Controller::HealthDeSelect(QString health) {
    if(health.toStdString() == "immune"){
        m_selectedHealthStatus.erase(stride::HealthStatus::Immune);
    }
    else if(health.toStdString() == "exposed"){
        m_selectedHealthStatus.erase(stride::HealthStatus::Exposed);
    }
    else if(health.toStdString() == "infectious"){
        m_selectedHealthStatus.erase(stride::HealthStatus::Infectious);
    }
    else if(health.toStdString() == "symptomatic"){
        m_selectedHealthStatus.erase(stride::HealthStatus::Symptomatic);
    }
    else if(health.toStdString() == "recovered"){
        m_selectedHealthStatus.erase(stride::HealthStatus::Recovered);
    }
    else if(health.toStdString() == "susceptible"){
        m_selectedHealthStatus.erase(stride::HealthStatus::Susceptible);
    }
    else if(health.toStdString() == "infectious and symptomatic"){
        m_selectedHealthStatus.erase(stride::HealthStatus::InfectiousAndSymptomatic);
    }
    Update();
}
}