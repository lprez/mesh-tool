#include "mainwindow.h"
#include "src/ui_mainwindow.h"

#include <string>
#include <QFileDialog>
#include <QLabel>
#include <QWidgetAction>#include <QDir>

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    std::map<VertexID, Vertex> vertices = {
        {0, Vertex({-1, -1, 0})},
        {1, Vertex({1, -1, 0})},
        {2, Vertex({0, 1, 0})}
    };
    std::map<FaceID, Face> faces = {
        {0, Face(0, 1, 2)}
    };
/*

    std::map<VertexID, Vertex> vertices = {
        {0, Vertex({-1, -1, 0.5}, {0, 0})},
        {1, Vertex({1, -1, 0.5}, {0, 0})},
        {2, Vertex({1, 1, 0.5}, {0, 0})},
        {3, Vertex({-1, 1, 0.5}, {0, 0})}
    };
    std::map<FaceID, Face> faces = {
        {0, Face(0, 1, 2)},
        {1, Face(0, 2, 3)}
    };

    std::map<VertexID, Vertex> vertices = {
        {0, Vertex({-1, -1, 0}, {0, 0})},
        {1, Vertex({1, -1, 0}, {0, 0})},
        {2, Vertex({0, 1, 0}, {0, 0})},
        {3, Vertex({0, 0, -2}, {0, 0})}
    };
    std::map<FaceID, Face> faces = {
        {0, Face(0, 1, 2)},
        {1, Face(0, 1, 3)},
        {2, Face(1, 2, 3)},
        {3, Face(2, 0, 3)}
    };
*/

    Mesh tmesh(vertices, faces); // nota: è solo la mesh inizializzatrice, la vera mesh è tenuta da MeshModel
    mesh_model = new MeshModel(this, tmesh);

    ui->meshView->setModel(mesh_model);


    load_plugins();
}

void MainWindow::load_plugins()
{
    QDir plugin_directory("plugins");

    if (!plugin_directory.exists()) plugin_directory.mkpath(".");

    QFileInfoList plugin_sources = plugin_directory.entryInfoList(QStringList() << "*.py", QDir::Files);

    for (QFileInfo plugin_info : plugin_sources) {
        try {
            PythonPlugin *plugin = PythonPlugin::load(plugin_info.canonicalFilePath().toLocal8Bit().data());
            if (plugin != nullptr) plugins.push_back(plugin);
        }  catch (PythonPluginException& e) {
            plugin_error_dialog(e);
        }
    }

    if (plugins.empty()) {
        QMessageBox::warning(this, tr(""), tr("Non è stato trovato nessun plugin"));
    }

    for (auto menu : {ui->menuFile, ui->menuEdit}) {
        QLabel *plugins_label = new QLabel(tr("<b><i>Plugins</i></b>"));
        plugins_label->setAlignment(Qt::AlignCenter);
        QWidgetAction *label_widget = new QWidgetAction(menu);
        label_widget->setDefaultWidget(plugins_label);
        menu->addAction(label_widget);
    }

    for (auto plugin : plugins) {
        std::wstring menu_string = plugin->menu();
        QMenu *menu;

        if (menu_string == std::wstring(L"File")) {
            menu = ui->menuFile;
        } else { // if (menu_string == std::wstring(L"Modifica")) {
            menu = ui->menuEdit;
        }

        QAction *action = new QAction(QIcon(), QString::fromWCharArray(plugin->entry().c_str()), menu);

        if (PythonTransformer* plugin_transformer = dynamic_cast<PythonTransformer*>(plugin)) {
            connect(action, &QAction::triggered, this, [=]() mutable {
                try {
                    mesh_model->replace(plugin_transformer->transform(mesh_model->getMesh()));
                }  catch (PythonPluginException& e) {
                    plugin_error_dialog(e);
                }
            });
        } else if (PythonImporter* plugin_importer = dynamic_cast<PythonImporter*>(plugin)) {
            connect(action, &QAction::triggered, this, [=]() {
                QString fname = QFileDialog::getOpenFileName(this);
                if (fname.count() > 0) {
                    try {
                        mesh_model->replace(plugin_importer->import_from(fname.toLocal8Bit().data()));
                    }  catch (PythonPluginException& e) {
                        plugin_error_dialog(e);
                    }
                }
            });
        } else if (PythonExporter* plugin_exporter = dynamic_cast<PythonExporter*>(plugin)) {
            connect(action, &QAction::triggered, this, [=]() {
                QString fname = QFileDialog::getSaveFileName(this);
                if (fname.count() > 0) {
                    try {
                        plugin_exporter->export_to(mesh_model->getMesh(), fname.toLocal8Bit().data());
                    }  catch (PythonPluginException& e) {
                        plugin_error_dialog(e);
                    }
                }
            });
        } else {
            continue;
        }

        menu->addAction(action);
    }
}

MainWindow::~MainWindow()
{
    delete ui;

    for (auto plugin : plugins) {
        delete plugin;
    }
}

void MainWindow::on_recalculate_normals()
{
    mesh_model->recalculate_normals();
}

void MainWindow::plugin_error_dialog(PythonPluginException &exception)
{
    QMessageBox::warning(this,
                         QString::fromWCharArray(exception.plugin_name.c_str()),
                         QString::fromWCharArray(exception.what_wide())
                         );
}
