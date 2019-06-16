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
//#include <memory>
#include <fstream>
//#include <QQuickStyle>
#include <memory.h>
#include <QtQuick/QQuickView>
#include "../cpp/geopop/EpiGrid.h"
#include "../cpp/geopop/io/EpiReader.h"
#include "../cpp/geopop/io/EpiReaderFactory.h"
#include "elements.h"
#include "../cpp/geopop/GeoGrid.h"
#include "Controller.h"


using namespace gui;

int main(int argc, char *argv[]) {
    std::shared_ptr<geopop::EpiReader> reader = nullptr;
    try {
        reader = geopop::EpiReaderFactory::CreateEpiReader("epi-output.json");
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    std::shared_ptr<geopop::EpiGrid> grid = reader->Read();
    std::cout << "Grid size: " << grid->size();

    QGuiApplication::setApplicationName("Stride");
    QGuiApplication app(argc, argv);

    QQuickView view;

    Controller cont;

    if (QFontDatabase::addApplicationFont("resources/fontello.ttf") == -1)
        qWarning() << "Failed to load resources/fontello.ttf";


    qmlRegisterType<Elements>("location", 1, 0, "Location");

    QStringList selectors;

    QQmlApplicationEngine engine;
    QQmlFileSelector::get(&engine)->setExtraSelectors(selectors);

    engine.rootContext()->setContextProperty("controller", &cont);

    cont.m_grid = grid;

    engine.load(QUrl::fromLocalFile("resources/AppWindow.qml"));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

