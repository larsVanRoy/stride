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

    Q_INVOKABLE QList<QObject*> getLocations();

    Q_INVOKABLE QString GetLocationName(unsigned int ID);

    Q_INVOKABLE QString GetLatitude(unsigned int ID);

    Q_INVOKABLE QString GetLongitude(unsigned int ID);

    Q_INVOKABLE QString GetPopCount(unsigned int ID);

    Q_INVOKABLE QString GetIll(unsigned int ID);

    Q_INVOKABLE double GetIllDouble(unsigned int ID);

    Q_INVOKABLE void nextDay();

    Q_INVOKABLE void previousDay();

    unsigned int GetCurrentDay();

//    Q_INVOKABLE void UpdateInfo(unsigned int ID);

    QList<QObject*> result;

    std::shared_ptr<geopop::EpiGrid> m_grid;

private:
    unsigned int m_day; ///current day
};

}

#endif //STRIDE_CONTROLLER_H
