#include "mainwindow.h"
#include "src/Nodes/rootnode.h"
#include "ui_mainwindow.h"
#include "src/Connectors/connectorline.h"
#include "src/Connectors/connectorlineparentwidget.h"
#include "src/Connectors/inputconnector.h"
#include "src/Connectors/outputconnector.h"
#include "src/Nodes/nodeparentwidget.h"
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->window()->showMaximized();
    fileName = UNTITLED;
    nodeScene = new QGraphicsScene(this);
    outputParameters = new QStringList();
    // add default output parameters
    *outputParameters << "lambda" << "mu" << "rho";
    createActions();

    this->setWindowTitle(fileName);
    this->setCentralWidget(ui->graphicsView);

    /// TODO: change the size such that nodeScene fits in the window and more !
    int menuBarHeight = ui->menubar->height();
    int toolBarHeight = ui->toolBar->height();
    int startusBarHeight = ui->statusbar->height();
    QRectF sceneRectInit = QRectF(0, 0, 1700 - 25, 1000 - menuBarHeight - toolBarHeight - startusBarHeight - 20);
    nodeScene->setSceneRect(sceneRectInit);
    ui->graphicsView->setScene(nodeScene);
    nodeScene->addRect(sceneRectInit, QPen(Qt::white));

    connectActions();
    RootNode* rootNode = new RootNode();
    proxyRoot = addNode(rootNode);
    emit rootNode->transferOutputsRequested(rootNode->getOutputs());
}

MainWindow::~MainWindow()
{
    delete ui;
    delete deleteNodeAction;
    delete outputParameters;
    delete proxyRoot;
    delete nodeScene;
}

void MainWindow::connectActions()
{
    connect(nodeScene, SIGNAL(focusItemChanged(QGraphicsItem*, QGraphicsItem*, Qt::FocusReason)), this, SLOT(getNewFocusItem(QGraphicsItem*, QGraphicsItem*, Qt::FocusReason)));
    // state changed?
    connect(ui->actionAddConstantMap, SIGNAL(triggered(bool)), this, SLOT(actionAddConstantMap()));
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

    outputParameters->clear();
    // add default output parameters
    *outputParameters << "rho" << "lambda" << "mu";
}

void MainWindow::newFile()
{
    if (saveDiscardOrCancelBeforeOpenOrNew())
        return;
    fileName = "";
    this->setWindowTitle(UNTITLED);
    clearAll();
}

void MainWindow::openFile()
{
    /// TODO (on va se marrer)
    // is true if canceled
    if (saveDiscardOrCancelBeforeOpenOrNew())
        return;

    // get the path of the file we want to open
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/home/juliehbr", tr("Yaml files (*.yaml)"));

    // if canceled, fileName is empty
    if (fileName.isEmpty())
        return;

    // open file
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(this, "Error", "Could not open file");
        return;
    }

    this->setWindowTitle(QFileInfo(file).baseName());
    clearAll();
    /// TODO (on va se marrer) --> read file, create nodes and connect them to each other
    file.close();
}

void MainWindow::saveFile()
{
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
    ///     - create Root class -> can add connectors to it -> create order
    ///     - create connection between two nodes
    ///     - from Root -> get all connected nodes
    ///     - what to do when they are nodes that are not connected? --> ask Lukas
    ///     update this part

    //YAML::Emitter out;

    // for now, only save focus item
    QGraphicsProxyWidget* itm = (QGraphicsProxyWidget*) nodeScene->focusItem();
    if (itm == nullptr)
        return;
    NodeBase* node = (NodeBase*) itm->widget();
    // undefined reference?
    switch (node->getTypeOfNode())
        switch (0)
        {
            case NODE:
                break;
            case ROOT:
                break;
            case CONSTANTMAPNODE:
                //saveConstantMapNode(&out, (ConstantMapNode*) node);
                break;
            default:
                break;
        }
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
/// ==================== OPEN FUNCTIONS FOR EACH NODE TYPE ====================
/// ===========================================================================

void MainWindow::openConstantMapNode()
{
    /// TODO
}


/// ===========================================================================
/// ==================== SAVE FUNCTIONS FOR EACH NODE TYPE ====================
/// ===========================================================================

void MainWindow::saveConstantMapNode(YAML::Emitter* out, ConstantMapNode* node)
{
    /// TODO
}


/// ===========================================================================
/// ===================== BASIC FUNCTIONS TO HANDLE NODES =====================
/// ===========================================================================

void MainWindow::createActions()
{
    deleteNodeAction = new QAction(tr("&Delete"), this);
    connect(deleteNodeAction, &QAction::triggered, this, &MainWindow::deleteNode);
}

QGraphicsProxyWidget* MainWindow::addNode(NodeBase* node)
{
    // add a graphic widget as parent & proxy to communicate between node and scene
    NodeParentWidget* nodeParentWidget = new NodeParentWidget();
    nodeParentWidget->setGeometry(node->geometry());
    nodeParentWidget->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    nodeScene->addItem(nodeParentWidget);
    //nodeScene->addRect(node->geometry(), QPen(Qt::red));
    QGraphicsProxyWidget* proxyNode = nodeScene->addWidget(node);
    proxyNode->setParentItem(nodeParentWidget);

    // the root does not need any input
    if (node->getTypeOfNode() != ROOT)
    {
        // Add input connector
        ConnectorBase* inputConnector = new InputConnector(nodeParentWidget);
        connect(inputConnector, SIGNAL(transferOutputsRequested(QStringList*)), node, SLOT(transferOutputs(QStringList*)));
        inputConnector->setGeometry(-8, 20, 15, 15);
        QGraphicsProxyWidget* inputConnectorProxy = nodeScene->addWidget(inputConnector);
        inputConnectorProxy->setParentItem(nodeParentWidget);
        node->setInputConnector(inputConnectorProxy);
        connectConnectors(inputConnector, nodeParentWidget);
    }

    // Add one output connector
    ConnectorBase* outputConnector = new OutputConnector(nodeParentWidget);
    connect(node, SIGNAL(transferOutputsRequested(QStringList*)), outputConnector, SLOT(transferOutputs(QStringList*)));
    int x, y;
    x = node->width() - 7;
    y = node->height() - 58;
    outputConnector->setGeometry(x, y, 15, 15);
    QGraphicsProxyWidget* outputConnectorProxy = nodeScene->addWidget(outputConnector);
    outputConnectorProxy->setParentItem(nodeParentWidget);
    node->addOutputConnector(outputConnectorProxy);
    connectConnectors(outputConnector, nodeParentWidget);

    connect(node, SIGNAL(resized(QRectF)), nodeParentWidget, SLOT(resize(QRectF)));
    connect(node, SIGNAL(nodeContextMenuRequested(QPoint)), this, SLOT(nodeContextMenu(QPoint)));
    connect(node, SIGNAL(addOutputConnectorRequested()), this, SLOT(actionAddOutputConnector()));
    connect(node, SIGNAL(deleteOutputConnectorRequested()), this, SLOT(actionDeleteOutputConnector()));
    nodeScene->setFocusItem(nodeParentWidget);
    return proxyNode;
}

void MainWindow::deleteNode()
{
    QGraphicsItem* nodeToRemove = nodeScene->focusItem();
    if (nodeToRemove == nullptr)
    {
        qDebug() << "Could not delete node...";
        return;
    }
    // remove parent to remove node & connectors
    nodeScene->removeItem(nodeToRemove->parentItem());
    auto proxyChildren = nodeToRemove->parentItem()->childItems();
    while (!proxyChildren.isEmpty())
    {
        deleteProxy((QGraphicsProxyWidget*) proxyChildren.takeLast());
    }
}

/// TODO
void MainWindow::deleteProxy(QGraphicsProxyWidget* proxy)
{
    if (proxy->widget()->objectName() == "Node")
    {
        //  delete proxy->widget();
    }
    else if (proxy->widget()->objectName() == "Connector")
    {
        ConnectorBase* connector = (ConnectorBase*) proxy->widget();
        connector->deleteLine();
        delete connector;
    }
    else
        qDebug() << proxy->widget();
    // qDebug() << "delete proxy";
    //delete proxy;
    //qDebug() << "delete proxy done";
}


/// ===========================================================================
/// =================== BASIC FUNCTIONS TO HANDLE CONNECTORS ==================
/// ===========================================================================

void MainWindow::addOutputConnector()
{
    QGraphicsProxyWidget* currentProxyNode = (QGraphicsProxyWidget*) nodeScene->focusItem();
    if (currentProxyNode == nullptr)
    {
        qDebug() << "Could node add connector";
        return;
    }
    NodeBase* currentNode = (NodeBase*) currentProxyNode->widget();
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*) currentProxyNode->parentItem();
    ConnectorBase* outputConnector = new OutputConnector(nodeParentWidget);
    connect(currentNode, SIGNAL(transferOutputsRequested(QStringList*)), outputConnector, SLOT(transferOutputs(QStringList*)));
    emit currentNode->transferOutputsRequested(currentNode->getOutputs());

    QGraphicsProxyWidget* outputConnectorProxy = nodeScene->addWidget(outputConnector);
    outputConnectorProxy->setParentItem(currentProxyNode->parentItem());
    currentNode->addOutputConnector(outputConnectorProxy);
    currentNode->performResize();

    int x = currentNode->geometry().width() - 7;
    int y = currentNode->geometry().height() - 58;
    outputConnector->setGeometry(x, y, 15, 15);

    connectConnectors(outputConnector, nodeParentWidget);
}

bool MainWindow::deleteOutputConnector()
{
    QGraphicsProxyWidget* currentProxyNode = (QGraphicsProxyWidget*) nodeScene->focusItem();
    if (currentProxyNode == nullptr)
    {
        qDebug() << "Could not delete connector";
        return false;
    }
    NodeBase* currentNode = (NodeBase*) currentProxyNode->widget();

    QList<QGraphicsProxyWidget*>* outputConnectors = currentNode->getOutputConnectors();
    if (outputConnectors->size() <= 1)
    {
        qDebug() << "don't remove last connector";
        return false;
    }
    QGraphicsProxyWidget* outputConnector = outputConnectors->takeLast();
    nodeScene->removeItem(outputConnector);
    delete outputConnector;
    currentNode->performResize();
    return true;
}

void MainWindow::connectConnectors(ConnectorBase* connector, NodeParentWidget* nodeParentWidget)
{
    connect(connector, SIGNAL(createConnectorLineResquested(ConnectorBase*)), this, SLOT(actionCreateConnectorLine(ConnectorBase*)));
    connect(nodeParentWidget, SIGNAL(hasMoved()), connector, SLOT(nodeParentWidgetHasMoved()));
    connect(connector, SIGNAL(isConnectorLineOnMe(ConnectorBase*)), this, SLOT(actionCheckIfConnectorNeedsConnection(ConnectorBase*)));
}

void MainWindow::createConnectorLine(ConnectorBase* connector)
{
    ConnectorLine* connectorLine = new ConnectorLine(connector, connector->getCenterPos());
    ConnectorLineParentWidget* connectorLineParentWidget = new ConnectorLineParentWidget();
    connectorLineParentWidget->setFlags(QGraphicsItem::ItemStacksBehindParent);
    nodeScene->addItem(connectorLineParentWidget);
    QGraphicsProxyWidget* connectorLineProxy = nodeScene->addWidget(connectorLine);
    connectorLineProxy->setParentItem(connectorLineParentWidget);
    connectorLine->setConnectorLineProxy(connectorLineProxy);

    connect(this, SIGNAL(connectConnectorToLine(ConnectorBase*)), connectorLine, SLOT(connectTo(ConnectorBase*)));
    connect(connectorLine, SIGNAL(drawnIsDone(ConnectorLine*)), this, SLOT(actionConnectorLineDrawn(ConnectorLine*)));
    connect(connectorLine, SIGNAL(deleteConnectorLine(QGraphicsProxyWidget*)), this, SLOT(actionDeleteConnectorLine(QGraphicsProxyWidget*)));
}

void MainWindow::deleteConnectorLine(QGraphicsProxyWidget* connectorLineProxy)
{
    nodeScene->removeItem(connectorLineProxy);
    delete connectorLineProxy;
}

void MainWindow::saveNewConnectorLine(ConnectorLine* connectorLine)
{
    newConnectorLine = connectorLine;
}

void MainWindow::checkConnectionBetweenConnectorAndLine(ConnectorBase* connector)
{
    QPointF positionToCheck = newConnectorLine->getPositionToCheck();
    if (abs(connector->getCenterPos().x() - positionToCheck.x()) <= 7.5 && abs(connector->getCenterPos().y() - positionToCheck.y()) <= 7.5)
        emit connectConnectorToLine(connector);
}


/// ===========================================================================
/// ============================ ADD MAP FUNCTIONS ============================
/// ===========================================================================

void MainWindow::addConstantMapNode()
{
    ConstantMapNode* constantMapNode = new ConstantMapNode(nodeScene);
    addNode(constantMapNode);
}


/// ===========================================================================
/// ============================== PRIVATE SLOTS ==============================
/// ===========================================================================

void MainWindow::nodeContextMenu(QPoint pos)
{
    QMenu menu(this);
    menu.addAction(deleteNodeAction);
    menu.exec(pos);
}

void MainWindow::getNewFocusItem(QGraphicsItem* newFocusItem, QGraphicsItem* oldFocusItem, Qt::FocusReason reason)
{
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

void MainWindow::actionCreateConnectorLine(ConnectorBase* connector)
{
    createConnectorLine(connector);
}

void MainWindow::actionDeleteConnectorLine(QGraphicsProxyWidget* connectorLineProxy)
{
    deleteConnectorLine(connectorLineProxy);
}

void MainWindow::actionConnectorLineDrawn(ConnectorLine* connectorLine)
{
    saveNewConnectorLine(connectorLine);
}

void MainWindow::actionCheckIfConnectorNeedsConnection(ConnectorBase* connector)
{
    checkConnectionBetweenConnectorAndLine(connector);
}

void MainWindow::actionAddOutputConnector()
{
    addOutputConnector();
}

void MainWindow::actionDeleteOutputConnector()
{
    deleteOutputConnector();
}

void MainWindow::actionAddConstantMap()
{
    addConstantMapNode();
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
    deleteNode();
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

