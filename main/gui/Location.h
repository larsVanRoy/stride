//
// Created by fj on 14.06.19.
//

#ifndef STRIDE_LOCATION_H
#define STRIDE_LOCATION_H

#include <QObject>
#include <QString>

namespace gui {

class Location : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int ID READ getID)
    Q_PROPERTY(double latitude READ getLatitude)
    Q_PROPERTY(double longitude READ getLongitude)
    Q_PROPERTY(double population READ getPopulation)

public:
//    explicit Location(QObject *parent=nullptr);
    explicit Location(double _long, double _lat, double _pop, unsigned int ID, QObject *parent=nullptr);

    double getLatitude() const;
    double getLongitude() const;
    double getPopulation() const;
    int getID() const;
private:
    double m_longitude;
    double m_latitude;
    double m_population;
    int ID;
};
}


#endif //STRIDE_LOCATION_H
