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
    void on_recalculate_normals();

private:
    void load_plugins();
    void plugin_error_dialog(PythonPluginException& exception);
    Ui::MainWindow *ui;
    MeshModel *mesh_model;
    std::vector<PythonPlugin*> plugins;
};
#endif // MAINWINDOW_H
