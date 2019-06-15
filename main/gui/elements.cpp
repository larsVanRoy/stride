#include "elements.h"
#include <iostream>
#include <QtQuick>

namespace gui {

static int idGen = 0;

Element::Element(QObject *parent) : QObject(parent) {
    this->m_longitude = 0;
    this->m_latitude = 0;
    this->m_population = 0;
    this->m_illness = 0;
    this->ID = idGen;
    idGen++;
}

Element::Element(double _long, double _lat, double _pop, double _ill, QObject *parent) : QObject(parent) {
    this->m_longitude = _lat;
    this->m_latitude = _long;
    this->m_population = _pop;
    this->m_illness = _ill;
}

int Element::getID() const {
    return this->ID;
}

//Element::Element(const Element&) {

//}

//andere waarden dan in de header?
Elements::Elements(QObject *parent) : QObject(parent) {
    // Hier moet je de private variabelen bekend makken!!
    qDebug() << "C++ Elements";  // info naar het scherm
    position = 0;
    m_elements = {};
    m_elements.append(new Element(52.36469902, 4.89990234, 5000, 0.8)); //Amsterdam
    m_elements.append(new Element(52, 4.4, 200, 0.5));
    m_elements.append(new Element(52, 4.8, 5, 0.1));
}

Elements::~Elements() {
    for (auto *el : this->m_elements) {
        delete el;
    }
}
//Elements::Elements(double lat, double lon, double pop, double ill) {
//    m_latitude = lat;
//    m_longitude = lon;
//    m_population = pop;
//    m_illness = ill;
//}

double Element::getLatitude() {
    qDebug() << "C++ getLatitude";  // info naar het scherm
    return m_latitude;
}

double Element::getLongitude() const {
    qDebug() << "C++ getLongitude";  // info naar het scherm
    return m_longitude;
}

double Element::getPopulation() const {
    qDebug() << "C++ getPopulation";  // info naar het scherm
    return m_population;
}

double Element::getIllness() const {
    qDebug() << "C++ getIllness";  // info naar het scherm
    return m_illness;
}

// setters
void Element::setLatitude(double &in) {
    qDebug() << "C++ setLatitude";  // info naar het scherm

    m_latitude = in;
    emit dummyChanged();
}

void Element::setLongitude(double &in) {
    qDebug() << "C++ setLongitude";  // info naar het scherm

    m_longitude = in;
    emit dummyChanged();
}

void Element::setPopulation(double &in) {
    qDebug() << "C++ setPopulation";  // info naar het scherm

    m_population = in;
    emit dummyChanged();
}

void Element::setIllness(double &in) {
    qDebug() << "C++ setIllness";  // info naar het scherm

    m_illness = in;
    emit dummyChanged();
}

QList<QObject *> Elements::getElements() {
    return m_elements;
}

const QObject &Elements::first() {

    return *m_elements.at(0);
}

const QObject &Elements::next() {
    position++;
    return *m_elements.at(position);
}

const QObject &Elements::last() {
    return *m_elements.at(m_elements.size() - 1);
}

int Elements::size() {
    return m_elements.size();
}


}