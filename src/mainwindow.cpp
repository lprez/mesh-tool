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

    // Geometria della mesh iniziale (un quadrato)
    std::map<VertexID, Vertex> vertices = {
        {0, Vertex({-1, -1, 0})},
        {1, Vertex({1, -1, 0})},
        {2, Vertex({1, 1, 0})},
        {3, Vertex({-1, 1, 0})}
    };
    std::map<FaceID, Face> faces = {
        {0, Face(0, 1, 2)},
        {1, Face(0, 2, 3)}
    };

    Mesh initial_mesh(vertices, faces);
    mesh_model = new MeshModel(this, initial_mesh);

    ui->meshView->setModel(mesh_model);


    load_plugins();
}

void MainWindow::load_plugins()
{
    QDir plugin_directory("plugins");

    if (!plugin_directory.exists()) plugin_directory.mkpath(".");

    QFileInfoList plugin_sources = plugin_directory.entryInfoList(QStringList() << "*.py", QDir::Files);

    // Carica i plugin
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

    // Aggiunge delle sezioni ai menu con titolo "Plugins"
    for (auto menu : {ui->menuFile, ui->menuEdit}) {
        QLabel *plugins_label = new QLabel(tr("<b><i>Plugins</i></b>"));
        plugins_label->setAlignment(Qt::AlignCenter);
        QWidgetAction *label_widget = new QWidgetAction(menu);
        label_widget->setDefaultWidget(plugins_label);
        menu->addAction(label_widget);
    }

    // Aggiunge delle azioni alla barra menu per richiamare i plugin
    for (auto plugin : plugins) {
        std::wstring menu_string = plugin->menu();
        QMenu *menu;

        // Il plugin decide su quale menu mettere l'azione
        if (menu_string == std::wstring(L"File")) {
            menu = ui->menuFile;
        } else { // if (menu_string == std::wstring(L"Modifica")) {
            menu = ui->menuEdit;
        }

        QAction *action = new QAction(QIcon(), QString::fromWCharArray(plugin->entry().c_str()), menu);

        // Callback dei segnali delle azioni
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
