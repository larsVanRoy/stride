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
#include "../cpp/geopop/EpiLocation.h"

namespace geopop {
class EpiGrid;
}

namespace gui {

enum class StateSelect : unsigned short {
    unassigned,
    location,
    box,
    radius
};

class Controller : public QObject {
Q_OBJECT
public:
    explicit Controller(std::shared_ptr<geopop::EpiGrid>);

    ~Controller();

    /// No copy constructor.
    Controller(const Controller&) = delete;

    /// No copy assignment.
    Controller& operator=(const Controller&) = delete;

    void Initialize(QObject* rootwindow);

    Q_INVOKABLE void print() const {
        std::cout << "test controller" << std::endl;
    }

    Q_INVOKABLE QList<QObject*> getLocations();

    Q_INVOKABLE QString GetPopCount(unsigned int ID);

    Q_INVOKABLE void InitializeMultiSelect(double longitude, double latitude);

    Q_INVOKABLE void BoxSelect(double longitude, double latitude);

    Q_INVOKABLE void RadiusSelect(double distance);

    Q_INVOKABLE QList<double> GetMultiSelectCoordinate() { return {m_multiSelect.get<0>(), m_multiSelect.get<1>()};}

    Q_INVOKABLE void nextDay();

    Q_INVOKABLE void previousDay();

    Q_INVOKABLE double GetColor(unsigned int ID);

    Q_INVOKABLE QString GetCurrentDay();

    Q_INVOKABLE void DisplayInfo();

    Q_INVOKABLE void SetSelectedLocation(unsigned int ID);

    Q_INVOKABLE void AgeSelect(QString);

    Q_INVOKABLE void AgeDeSelect(QString);

    Q_INVOKABLE void HealthSelect(QString);

    Q_INVOKABLE void HealthDeSelect(QString);

    Q_INVOKABLE void Update();

    void DisplayDay();

private:
    bool SetObjectText(const std::string& objectName, const std::string& text);

    bool SetObjectPercentage(const std::string& objectName, double percentage, unsigned int precision);

    std::vector<const geopop::EpiLocation<geopop::Coordinate>*> m_selectedLocations;

    std::set<stride::AgeBrackets::AgeBracket> m_selectedAgeBrackets;

    std::set<stride::HealthStatus> m_selectedHealthStatus;

    StateSelect m_selection;

    unsigned int m_day;     ///current day

    unsigned int m_maxDay;  ///max day

    unsigned int m_step;    ///days between steps

    geopop::Coordinate m_multiSelect;
    
    QList<QObject*> result;

    std::shared_ptr<geopop::EpiGrid> m_grid;

    QObject* m_app;
};

}

#endif //STRIDE_CONTROLLER_H
