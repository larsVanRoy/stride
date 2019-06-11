#ifdef QT_WIDGETS_LIB
#include <QApplication>
#else
#include <QGuiApplication>
#endif
#include <QFontDatabase>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlFileSelector>
#include <memory>
#include <fstream>
//#include <QQuickStyle>

#include "../cpp/geopop/EpiGrid.h"
#include "../cpp/geopop/io/EpiReader.h"
#include "../cpp/geopop/io/EpiReaderFactory.h"
#include "elements.h"
#include "../cpp/geopop/GeoGrid.h"


int main(int argc, char *argv[])
{
    QGuiApplication::setApplicationName("Stride");
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);


    QFontDatabase fontDatabase;
    if (fontDatabase.addApplicationFont("resources/fontello.ttf") == -1)
        qWarning() << "Failed to load resources/fontello.ttf";

    qmlRegisterType<Elements>("elements", 1, 0, "Elements");
    qmlRegisterType<Element>("elements", 1, 0, "Element");

    QStringList selectors;

    QQmlApplicationEngine engine;
    QQmlFileSelector::get(&engine)->setExtraSelectors(selectors);

    engine.load(QUrl::fromLocalFile("resources/AppWindow.qml"));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
