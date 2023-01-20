#include "mainwindow.h"
#include "src/Nodes/Maps/constantmapnode.h"
#include "src/Nodes/rootnode.h"
#include "ui_mainwindow.h"
#include "src/Connectors/inputs/inputconnector.h"
#include "src/Connectors/outputs/outputconnector.h"
#include "src/Nodes/nodeparentwidget.h"
#include "helpers.h"
#include "yaml-cpp/emitter.h"
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->window()->showMaximized();
    fileName = UNTITLED;
    nodeScene = new QGraphicsScene(this);

    this->setWindowTitle(fileName);
    this->setCentralWidget(ui->graphicsView);

    /// TODO: change the size such that nodeScene fits in the window and more !
    int menuBarHeight = ui->menubar->height();
    int toolBarHeight = ui->toolBar->height();
    int startusBarHeight = ui->statusbar->height();
    QRectF sceneRectInit = QRectF(0, 0, 1700 - 25, 1000 - menuBarHeight - toolBarHeight - startusBarHeight - 20);
    nodeScene->setSceneRect(sceneRectInit);
    ui->graphicsView->setScene(nodeScene);

    connectActions();

    widgetsHandler = new WidgetsHandler(nodeScene);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete widgetsHandler;
    delete nodeScene;
}

void MainWindow::connectActions()
{
    // state changed?
    connect(ui->actionAddConstantMap, SIGNAL(triggered(bool)), this, SLOT(actionAddConstantMap()));
    connect(ui->actionAddAffineMap, SIGNAL(triggered(bool)), this, SLOT(actionAddAffineMap()));
    connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(actionNew()));
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(actionOpen()));
    connect(ui->actionClose, SIGNAL(triggered(bool)), this, SLOT(actionClose()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(actionSave()));
    connect(ui->actionSaveAs, SIGNAL(triggered(bool)), this, SLOT(actionSaveAs()));
    connect(ui->actionDelete, SIGNAL(triggered(bool)), this, SLOT(actionDelete()));
    connect(ui->actionCut, SIGNAL(triggered(bool)), this, SLOT(actionCut()));
    connect(ui->actionCopy, SIGNAL(triggered(bool)), this, SLOT(actionCopy()));
    connect(ui->actionPaste, SIGNAL(triggered(bool)), this, SLOT(actionPaste()));
    connect(ui->actionUndo, SIGNAL(triggered(bool)), this, SLOT(actionUndo()));
    connect(ui->actionRedo, SIGNAL(triggered(bool)), this, SLOT(actionRedo()));
}

/// ===========================================================================
/// ======================== BASIC FUNCTIONS IN EDITOR ========================
/// ===========================================================================

void MainWindow::clearAll()
{
    /// TODO: delete all nodes somehow
    nodeScene->clear();
}

void MainWindow::newFile()
{
    if (saveDiscardOrCancelBeforeOpenOrNew())
        return;
    fileName = "";
    this->setWindowTitle(UNTITLED);
    clearAll();
    widgetsHandler->init();
}

void MainWindow::openFile()
{
    // is true if canceled
    if (saveDiscardOrCancelBeforeOpenOrNew())
        return;

    // get the path of the file we want to open
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/home/juliehbr", tr("Yaml files (*.yaml)"));

    // if canceled, fileName is empty
    if (fileName.isEmpty())
        return;

    // open file to get infos and see if it exists
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(this, "Error", "Could not open file");
        return;
    }

    this->setWindowTitle(QFileInfo(file).baseName());
    clearAll();
    widgetsHandler->init();
    file.close();

    /// TODO --> read file, create nodes and connect them to each other

    YAML::Node fileNode = YAML::LoadFile(fileName.toStdString());
    RootNode* rootNode = (RootNode*) widgetsHandler->getProxyRoot()->widget();

    // read tag of first node
    if (fileNode.Tag() == "!ConstantMap")
    {
        rootNode->getOutputs()->clear();
        rootNode->updateOutputs();
        openConstantMapNode(rootNode, &fileNode, rootNode->getOutputs());
    }
    else
        qDebug() << "tag is: " << fileNode.Tag().c_str();
}

void MainWindow::saveFile()
{
    // if file not existing yet, act as "save as"
    if (fileName == UNTITLED)
    {
        saveFileAs();
        return;
    }

    save();
}

void MainWindow::saveFileAs()
{
    QFileDialog saveAsDialog(this, tr("Save As"), "/home/juliehbr/", tr("Yaml files (*.yaml)"));
    saveAsDialog.setDefaultSuffix("yaml");
    saveAsDialog.setAcceptMode(QFileDialog::AcceptSave);

    // if save was canceled
    if (!saveAsDialog.exec())
        return;

    QStringList selectedFiles = saveAsDialog.selectedFiles();

    // don't know when it can happen
    if (selectedFiles.size() == 0)
        return;

    fileName = selectedFiles.first();
    save();
}

void MainWindow::save()
{
    QFile* file = new QFile(fileName);
    if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::information(this, "Error", "Could not save the file");
        return;
    }

    /// TODO:
    ///     - what to do when they are nodes that are not connected? --> ask Lukas

    YAML::Emitter out;

    if (widgetsHandler->getProxyRoot() == nullptr)
        qDebug() << "ERROR: no root !";

    emit widgetsHandler->saveRequested(&out);


    QTextStream outStream(file);

    outStream << out.c_str();
    outStream.flush();

    // needs to change title in case of save as
    if (this->windowTitle() != QFileInfo(*file).baseName())
        this->setWindowTitle(QFileInfo(*file).baseName());

    file->close();
    notSaved = false;
}

/// return true if canceled
bool MainWindow::saveDiscardOrCancelBeforeOpenOrNew()
{
    if (fileName.isEmpty() || notSaved)
    {
        QMessageBox::StandardButton response = QMessageBox::question(this, "Save document?", "Save changes to document " + fileName + " before closing?", QMessageBox::Discard | QMessageBox::Cancel | QMessageBox::Save);
        if (response == QMessageBox::Cancel)
            return true;
        else if (response == QMessageBox::Save)
            actionSave();
    }
    return false;
}

void MainWindow::enableDisableIcons(bool enable)
{
    ui->actionDelete->setEnabled(enable);
}

/// ===========================================================================
/// =========================== OPEN NODE FUNCTIONS ===========================
/// ===========================================================================

void MainWindow::openConstantMapNode(NodeBase* parentNode, YAML::Node* node, QStringList* outputs)
{
    bool outputsNeedUpdate = false;

    QStringList* outputs_node = new QStringList();
    QList<double>* values = new QList<double>();
    if ((*node)["map"])
    {
        for (YAML::const_iterator it = (*node)["map"].begin(); it != (*node)["map"].end(); ++it)
        {
            QString output = it->first.as<std::string>().c_str();
            if (!outputs->contains(output))
            {
                *outputs << output;
                outputsNeedUpdate = true;
            }
            *outputs_node << output;
            *values << it->second.as<double>();
        }
    }

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addConstantMapNode(outputs_node, values);

    // move it next to parent node
    QRect parentGeometry = parentNode->geometry();
    int right = parentGeometry.right();
    int top = parentGeometry.top();
    ((NodeParentWidget*) proxyNode->parentWidget())->move(right + 50, top);

    ConstantMapNode* constantMapNode = (ConstantMapNode*) proxyNode->widget();
    InputConnector* inputConnector = (InputConnector*) constantMapNode->getInputConnector()->widget();
    OutputConnector* outputConnector = parentNode->getFirstAvailableOutputConnector();

    widgetsHandler->createConnectorLine(outputConnector, inputConnector);

    /// TODO: add components

    if (outputsNeedUpdate)
        widgetsHandler->updateRoot();
}



void MainWindow::getNewFocusItem(QGraphicsItem* newFocusItem, QGraphicsItem* oldFocusItem, Qt::FocusReason reason)
{
    UNUSED(oldFocusItem);
    UNUSED(reason);
    if (newFocusItem == nullptr)
    {
        enableDisableIcons(false);
    }
    else
    {
        QGraphicsProxyWidget* currentProxyNode = (QGraphicsProxyWidget*) newFocusItem;
        NodeBase* currentNode = (NodeBase*) currentProxyNode->widget();
        if (currentNode == nullptr || currentNode->getTypeOfNode() == ROOT)
        {
            enableDisableIcons(false);
        }
        else
        {
            enableDisableIcons(true);
        }
    }
}

void MainWindow::stateChanged()
{
    notSaved = true;
}

void MainWindow::actionAddConstantMap()
{
    widgetsHandler->addConstantMapNode();
}

void MainWindow::actionAddAffineMap()
{
    widgetsHandler->addAffineMapNode();
}

void MainWindow::actionNew()
{
    newFile();
}

void MainWindow::actionOpen()
{
    openFile();
}

void MainWindow::actionClose()
{
    this->close();
}

void MainWindow::actionSave()
{
    saveFile();
}

void MainWindow::actionSaveAs()
{
    saveFileAs();
}

void MainWindow::actionDelete()
{
    widgetsHandler->deleteNode();
}

void MainWindow::actionCut()
{
    /// TODO
}

void MainWindow::actionCopy()
{
    /// TODO
}

void MainWindow::actionPaste()
{
    /// TODO
}

void MainWindow::actionUndo()
{
    /// TODO
}

void MainWindow::actionRedo()
{
    /// TODO
}

