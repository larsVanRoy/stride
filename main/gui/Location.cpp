//
// Created by fj on 14.06.19.
//

#include "Location.h"
#include <QDebug>

namespace gui{

Location::Location(double longitude, double latitude, double population, unsigned int ID, QObject *parent) :
QObject(parent), m_longitude(longitude), m_latitude(latitude), m_population(population), ID(ID)
{

}

int Location::getID() const {
    return ID;
}

double Location::getLatitude() const {
    return m_latitude;
}

double Location::getLongitude() const {
    return m_longitude;
}

double Location::getPopulation() const {
    return m_population;
}

}