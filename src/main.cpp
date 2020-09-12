#include "mainwindow.h"
#include "pythonplugin.h"
#include "meshmodel.h"

#include <QApplication>
#include <QObject>
#include <QDir>


int main(int argc, char *argv[])
{   
    QApplication a(argc, argv);

    std::vector<PythonPlugin*> plugins;
    QDir plugin_directory("plugins");
    QFileInfoList plugin_sources = plugin_directory.entryInfoList(QStringList() << "*.py", QDir::Files);

    for (QFileInfo plugin_info : plugin_sources) {
        PythonPlugin *plugin = PythonPlugin::load(plugin_info.canonicalFilePath().toLocal8Bit().data());
        if (plugin != nullptr) plugins.push_back(plugin);
    }

    QObject::connect(&a, &QCoreApplication::aboutToQuit, &a, [plugins]() {
        for (auto plugin : plugins) {
            delete plugin;
        }
    });

    MainWindow w(plugins);

    w.show();
    return a.exec();
}
