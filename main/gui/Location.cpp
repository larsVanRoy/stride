//
// Created by fj on 14.06.19.
//

#include "Location.h"
#include <QDebug>

namespace gui{

static int _idGen = 0;

Location::Location(QObject *parent) : QObject(parent) {
    this->m_longitude = 0;
    this->m_latitude = 0;
    this->m_population = 0;
    this->m_illness = 0;
    this->ID = _idGen;
    _idGen++;
}

Location::Location(double _long, double _lat, double _pop, double _ill, QObject *parent) : QObject(parent) {
    this->m_longitude = _long;
    this->m_latitude = _lat;
    this->m_population = _pop;
    this->m_illness = _ill;
}

int Location::getID() const {
    return this->ID;
}

double Location::getLatitude() {
    qDebug() << "C++ getLatitude";  // info naar het scherm
    return m_latitude;
}

double Location::getLongitude() const {
    qDebug() << "C++ getLongitude";  // info naar het scherm
    return m_longitude;
}

double Location::getPopulation() const {
    qDebug() << "C++ getPopulation";  // info naar het scherm
    return m_population;
}

double Location::getIllness() const {
    qDebug() << "C++ getIllness";  // info naar het scherm
    return m_illness;
}

// setters
void Location::setLatitude(double &in) {
    qDebug() << "C++ setLatitude";  // info naar het scherm

    m_latitude = in;
    emit dummyChanged();
}

void Location::setLongitude(double &in) {
    qDebug() << "C++ setLongitude";  // info naar het scherm

    m_longitude = in;
    emit dummyChanged();
}

void Location::setPopulation(double &in) {
    qDebug() << "C++ setPopulation";  // info naar het scherm

    m_population = in;
    emit dummyChanged();
}

void Location::setIllness(double &in) {
    qDebug() << "C++ setIllness";  // info naar het scherm

    m_illness = in;
    emit dummyChanged();
}


}