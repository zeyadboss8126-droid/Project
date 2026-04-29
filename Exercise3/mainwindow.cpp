#include "mainwindow.h"
#include "ui_mainwindow.h"

// ── VTK OpenVR headers MUST come before QVTKOpenGLNativeWidget ──
// vtkOpenVRRenderWindow pulls in its own gl.h; if Qt's OpenGL bridge
// loads first the duplicate-include guard fires (C1189 / E0035).
#include <vtkOpenVRRenderer.h>
#include <vtkOpenVRRenderWindow.h>
#include <vtkOpenVRRenderWindowInteractor.h>

// ── Qt-VTK bridge + standard VTK rendering headers ──
#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkCamera.h>

// ── Qt headers ──
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QStatusBar>
#include <QTreeView>
#include <QVBoxLayout>

#include <functional>

#include "optiondialog.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QVBoxLayout* layout = new QVBoxLayout(ui->vtkPlaceholder);
    layout->setContentsMargins(0, 0, 0, 0);

    vtkWidget = new QVTKOpenGLNativeWidget();
    layout->addWidget(vtkWidget);

    ui->vtkPlaceholder->setLayout(layout);

    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    vtkWidget->setRenderWindow(renderWindow);

    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);
    renderer->SetBackground(0.1, 0.2, 0.4);

    partList = new ModelPartList("Parts List");
    ui->treeView->setModel(partList);

    ui->treeView->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->treeView->addAction(ui->actionItem_Options);

    connect(ui->treeView, &QTreeView::clicked,
        this, &MainWindow::handleTreeClicked);

    connect(this, &MainWindow::statusUpdateMessage,
        ui->statusbar, &QStatusBar::showMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleTreeClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    ModelPart* item = static_cast<ModelPart*>(index.internalPointer());

    if (!item)
        return;

    emit statusUpdateMessage(QString("Selected: %1").arg(item->getName()), 0);
}

void MainWindow::on_Button1_clicked()
{
    on_actionItem_Options_triggered();
}

void MainWindow::on_Button2_clicked()
{
    QModelIndex index = ui->treeView->currentIndex();

    if (!index.isValid()) {
        emit statusUpdateMessage("No item selected", 0);
        return;
    }

    ModelPart* part = static_cast<ModelPart*>(index.internalPointer());

    if (!part) {
        emit statusUpdateMessage("Invalid selection", 0);
        return;
    }

    vtkSmartPointer<vtkActor> actor = part->getActor();

    if (actor != nullptr) {
        renderer->RemoveActor(actor);
        vtkWidget->renderWindow()->Render();
        emit statusUpdateMessage(QString("Removed from render: %1").arg(part->getName()), 0);
    }
    else {
        emit statusUpdateMessage("Selected item has no actor", 0);
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open STL File"),
        "",
        tr("STL Files (*.stl);;All Files (*)")
    );

    if (fileName.isEmpty())
        return;

    QModelIndex index = ui->treeView->currentIndex();

    QFileInfo info(fileName);
    QList<QVariant> data = { info.fileName(), "true", "255,0,89" };

    QModelIndex newIndex = partList->appendChild(index, data);

    ModelPart* newPart = static_cast<ModelPart*>(newIndex.internalPointer());

    if (newPart) {
        newPart->loadSTL(fileName);
        emit statusUpdateMessage(QString("Loaded: %1").arg(info.fileName()), 0);
    }

    ui->treeView->expandAll();
    updateRender();
}

void MainWindow::on_actionSave_triggered()
{
    QMessageBox::information(this, "Save", "Save is not implemented yet.");
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionItem_Options_triggered()
{
    QModelIndex index = ui->treeView->currentIndex();

    if (!index.isValid()) {
        emit statusUpdateMessage("No item selected", 0);
        return;
    }

    ModelPart* part = static_cast<ModelPart*>(index.internalPointer());

    if (!part) {
        emit statusUpdateMessage("Invalid selection", 0);
        return;
    }

    OptionDialog dlg(this);
    dlg.setModelPart(part);

    if (dlg.exec() == QDialog::Accepted) {
        partList->dataChanged(index, index);
        updateRender();
        emit statusUpdateMessage(QString("Updated: %1").arg(part->getName()), 0);
    }
}

void MainWindow::updateRender()
{
    renderer->RemoveAllViewProps();

    int rows = partList->rowCount(QModelIndex());

    for (int i = 0; i < rows; ++i) {
        updateRenderFromTree(partList->index(i, 0, QModelIndex()));
    }

    renderer->ResetCamera();

    vtkCamera* camera = renderer->GetActiveCamera();
    if (camera) {
        camera->Azimuth(30);
        camera->Elevation(20);
    }

    renderer->ResetCameraClippingRange();
    renderer->Render();
    vtkWidget->renderWindow()->Render();
}

void MainWindow::updateRenderFromTree(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    if (selectedPart && selectedPart->visible()) {
        vtkSmartPointer<vtkActor> actor = selectedPart->getActor();

        if (actor != nullptr) {
            renderer->AddActor(actor);
        }
    }

    int rows = partList->rowCount(index);

    for (int i = 0; i < rows; ++i) {
        updateRenderFromTree(partList->index(i, 0, index));
    }
}

void MainWindow::on_ButtonVR_clicked()
{
    QMessageBox::information(this, "VR",
        "Start SteamVR and connect the headset before using real VR.");

    vtkSmartPointer<vtkOpenVRRenderer> vrRenderer =
        vtkSmartPointer<vtkOpenVRRenderer>::New();

    vtkSmartPointer<vtkOpenVRRenderWindow> vrWindow =
        vtkSmartPointer<vtkOpenVRRenderWindow>::New();

    vtkSmartPointer<vtkOpenVRRenderWindowInteractor> vrInteractor =
        vtkSmartPointer<vtkOpenVRRenderWindowInteractor>::New();

    vrWindow->AddRenderer(vrRenderer);
    vrInteractor->SetRenderWindow(vrWindow);

    vrRenderer->SetBackground(0.1, 0.1, 0.2);

    std::function<void(const QModelIndex&)> addActors =
        [&](const QModelIndex& index)
        {
            if (!index.isValid()) return;

            ModelPart* part = static_cast<ModelPart*>(index.internalPointer());

            if (part && part->visible() && part->getActor()) {
                vtkSmartPointer<vtkActor> copiedActor =
                    vtkSmartPointer<vtkActor>::New();

                copiedActor->ShallowCopy(part->getActor());
                vrRenderer->AddActor(copiedActor);
            }

            int rows = partList->rowCount(index);
            for (int i = 0; i < rows; ++i)
                addActors(partList->index(i, 0, index));
        };

    int topRows = partList->rowCount(QModelIndex());
    for (int i = 0; i < topRows; ++i)
        addActors(partList->index(i, 0, QModelIndex()));

    vrRenderer->ResetCamera();
    vrWindow->Render();
    vrInteractor->Start();
}