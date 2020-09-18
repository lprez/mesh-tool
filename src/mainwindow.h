#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pythonplugin.h"
#include "meshmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
public Q_SLOTS:
    // Lanciata quando si clicca su "Ricalcola normali"
    void on_recalculate_normals();
    // Lanciata quando si clicca su "Smooth shading"
    void on_smooth_shading();
    // Lanciata quando si clicca su "Flat shading"
    void on_flat_shading();

    // Lanciata quando si riceve il segnale mesh_changed
    void on_mesh_changed();

private:
    void load_plugins();
    void plugin_error_dialog(PythonPluginException& exception);
    Ui::MainWindow *ui;
    MeshModel *mesh_model;
    std::vector<PythonPlugin*> plugins;
};
#endif // MAINWINDOW_H
