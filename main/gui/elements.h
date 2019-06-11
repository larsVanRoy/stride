#ifndef ELEMENTS_H
#define ELEMENTS_H

#include <QObject>
#include <QString>


// To expose attributes (from C++) to QML

class Element : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int ID READ getID)
    Q_PROPERTY(double latitude READ getLatitude WRITE setLatitude NOTIFY dummyChanged)
    Q_PROPERTY(double longitude READ getLongitude WRITE setLongitude NOTIFY dummyChanged)
    Q_PROPERTY(double population READ getPopulation WRITE setPopulation NOTIFY dummyChanged)
    Q_PROPERTY(double illness READ getIllness WRITE setIllness NOTIFY dummyChanged)
public:
    explicit Element(QObject *parent=nullptr);
    Element(double _lat, double _long, double _pop, double _ill, QObject *parent=nullptr);

    Element(const Element&);

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

class Elements : public QObject
{
    Q_OBJECT
//    Q_PROPERTY(QList<Element*> m_elements READ getElements)

public:
    explicit Elements(QObject *parent = nullptr);
    ~Elements();
    // getters
    Q_INVOKABLE QList<QObject*> getElements();
    Q_INVOKABLE const QObject&  first();
    Q_INVOKABLE const QObject&  next();
    Q_INVOKABLE const QObject&  last();
    Q_INVOKABLE int size();
    //
    QList<QObject*> m_elements;


private:
    int position;
};

#endif // ELEMENTS_H
