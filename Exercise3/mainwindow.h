#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

#include "ModelPartList.h"
#include "ModelPart.h"

#include <vtkSmartPointer.h>

class vtkGenericOpenGLRenderWindow;
class vtkRenderer;
class QVTKOpenGLNativeWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

signals:
    void statusUpdateMessage(const QString& message, int timeout);

private slots:
    void on_Button1_clicked();
    void on_Button2_clicked();
    void handleTreeClicked(const QModelIndex& index);
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionExit_triggered();
    void on_actionItem_Options_triggered();
    void on_ButtonVR_clicked();

private:
    Ui::MainWindow* ui = nullptr;
    ModelPartList* partList = nullptr;

    QVTKOpenGLNativeWidget* vtkWidget = nullptr;

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderer> renderer;

    void updateRender();
    void updateRenderFromTree(const QModelIndex& index);
};

#endif