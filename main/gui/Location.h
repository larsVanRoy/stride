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
    Q_PROPERTY(double latitude READ getLatitude WRITE setLatitude NOTIFY dummyChanged)
    Q_PROPERTY(double longitude READ getLongitude WRITE setLongitude NOTIFY dummyChanged)
    Q_PROPERTY(double population READ getPopulation WRITE setPopulation NOTIFY dummyChanged)
    Q_PROPERTY(double illness READ getIllness WRITE setIllness NOTIFY dummyChanged)
public:
    explicit Location(QObject *parent=nullptr);
    Location(double _long, double _lat, double _pop, double _ill, QObject *parent=nullptr);

//    Element(const Element&);

    Q_INVOKABLE double getLatitude();
    double getLongitude() const;
    double getPopulation() const;
    double getIllness() const;
    int getID() const;
    // setters
    void   setLatitude(double &in);
    void   setLongitude(double &in);
    void   setPopulation(double &in);
    void   setIllness(double &in);
    signals:
            void dummyChanged();
private:
    double m_latitude;
    double m_longitude;
    double m_population;
    double m_illness;
    int ID;
};
}


#endif //STRIDE_LOCATION_H
