#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <string>
#include <QFileDialog>

MainWindow::MainWindow(const std::vector<PythonPlugin*>& plugins, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    std::map<VertexID, Vertex> vertices = {
        {0, Vertex({-1, -1, 0}, {0, 0})},
        {1, Vertex({1, -1, 0}, {0, 0})},
        {2, Vertex({0, 1, 0}, {0, 0})}
    };
    std::map<FaceID, Face> faces = {
        {0, Face(0, 1, 2)}
    };
/*

    std::map<VertexID, Vertex> vertices = {
        {0, Vertex({-1, -1, 0}, {0, 0})},
        {1, Vertex({1, -1, 0}, {0, 0})},
        {2, Vertex({1, 1, 0}, {0, 0})},
        {3, Vertex({-1, 1, 0}, {0, 0})}
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
    MeshModel *model = new MeshModel(this, tmesh);

    ui->meshView->setModel(model);

    for (auto plugin : plugins) {
        std::wstring menu_string = plugin->menu();
        QMenu *menu;

        if (menu_string == std::wstring(L"File")) {
            menu = ui->menuFile;
        } else { // if (menu_string == std::wstring(L"Modifica")) {
            menu = ui->menuModifica;
        }

        QAction *action = new QAction(QIcon(), QString::fromWCharArray(plugin->entry().c_str()), menu);

        if (PythonTransformer* plugin_transformer = dynamic_cast<PythonTransformer*>(plugin)) {
            std::function<Mesh(const Mesh&)> transformer =
                    [plugin_transformer](const Mesh& mesh) {return plugin_transformer->transform(mesh);};
            connect(action, &QAction::triggered, this, [=]() mutable {
                model->transform(transformer); });
        } else if (PythonImporter* plugin_importer = dynamic_cast<PythonImporter*>(plugin)) {
            connect(action, &QAction::triggered, this, [=]() {
                const char *fname = QFileDialog::getOpenFileName(this).toLocal8Bit().data();
                std::function<Mesh(const Mesh&)> importer = [=](const Mesh&) mutable {
                    return plugin_importer->import_from(fname);};
                model->transform(importer);
            });
        } else if (PythonExporter* plugin_exporter = dynamic_cast<PythonExporter*>(plugin)) {
            connect(action, &QAction::triggered, this, [=]() {
                const char *fname = QFileDialog::getSaveFileName(this).toLocal8Bit().data();
                plugin_exporter->export_to(model->getMesh(), fname);
            });
        }


        menu->addAction(action);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
