//
// Created by fj on 14.06.19.
//

#ifndef STRIDE_CONTROLLER_H
#define STRIDE_CONTROLLER_H

#include <QObject>
#include <QString>
#include <iostream>
#include <memory>
#include <QString>
#include <QList>

#include "../cpp/geopop/geo/Coordinate.h"
//#include "../cpp/geopop/EpiGrid.h"

namespace geopop {
class EpiGrid;
}

namespace gui {

class Controller : public QObject {
Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);

    ~Controller();

    Q_PROPERTY(unsigned int m_day READ GetCurrentDay)

    Q_INVOKABLE void print() const {
        std::cout << "test controller" << std::endl;
    }

    Q_INVOKABLE void loadFile();

    Q_INVOKABLE QString GetName(unsigned int ID);

    Q_INVOKABLE QList<QObject*> getLocations();

    Q_INVOKABLE QString GetLocationName(unsigned int ID);

    Q_INVOKABLE QString GetLatitude(unsigned int ID);

    Q_INVOKABLE QString GetLongitude(unsigned int ID);

    Q_INVOKABLE QString GetPopCount(unsigned int ID);

    Q_INVOKABLE QString GetIll(unsigned int ID);

    Q_INVOKABLE double GetIllDouble(unsigned int ID);

    Q_INVOKABLE void InitializeMultiSelect(double longitude, double latitude);

    Q_INVOKABLE void BoxSelect(double longitude, double latitude);

    Q_INVOKABLE void RadiusSelect(double distance);

    Q_INVOKABLE QList<double> GetMultiSelectCoordinate() { return {m_multiSelect.get<0>(), m_multiSelect.get<1>()};}

    Q_INVOKABLE void nextDay();

    Q_INVOKABLE void previousDay();

    Q_INVOKABLE void SetInfo();

    Q_INVOKABLE void SetSelectedLocation(int ID) { m_selectedLocationId = ID; }

    Q_INVOKABLE double GetColor(unsigned int ID);

    unsigned int GetCurrentDay();

//    Q_INVOKABLE void UpdateInfo(unsigned int ID);

    std::string show;

    QList<QObject*> result;

    std::shared_ptr<geopop::EpiGrid> m_grid;

    QObject* m_app;
private:
    unsigned int m_selectedLocationId;

    unsigned int m_day; ///current day

    geopop::Coordinate m_multiSelect;

};

}

#endif //STRIDE_CONTROLLER_H
