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
#include "../cpp/geopop/GeoGrid.h"
#include "Controller.h"


using namespace gui;

int main(int argc, char *argv[]) {
    std::string filename;
    if(argc != 2) {
        std::cout << "use: ./executable [filename]" << std::endl;
        return -1;
    }
    else {
        filename = argv[1];
    }


    std::shared_ptr<geopop::EpiReader> reader = nullptr;
    try {
        reader = geopop::EpiReaderFactory::CreateEpiReader(filename);
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    std::shared_ptr<geopop::EpiGrid> grid = reader->Read();

    QGuiApplication::setApplicationName("Stride");
    QGuiApplication app(argc, argv);

    QQuickView view;

    Controller controller(grid);

    if (QFontDatabase::addApplicationFont("resources/fontello.ttf") == -1)
        qWarning() << "Failed to load resources/fontello.ttf";

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("controller", &controller);

    engine.load(QUrl::fromLocalFile("resources/AppWindow.qml"));
    if (engine.rootObjects().isEmpty())
        return -1;

    controller.Initialize(engine.rootObjects()[0]);

    return app.exec();
}

