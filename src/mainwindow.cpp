#include "mainwindow.h"
#include "src/Connectors/outputs/mathsoutputconnector.h"
#include "src/Nodes/Maps/constantmapnode.h"
#include "src/Nodes/Maps/affinemapnode.h"
#include "src/Nodes/rootnode.h"
#include "ui_mainwindow.h"
#include "src/Connectors/connectorline.h"
#include "src/Connectors/connectorlineparentwidget.h"
#include "src/Connectors/inputs/inputconnector.h"
#include "src/Connectors/outputs/outputconnector.h"
#include "src/Nodes/nodeparentwidget.h"
#include "helpers.h"
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
    newConnectorLine = nullptr;
    nodes = QList<QGraphicsProxyWidget*>();
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

    connectActions();

    addRoot();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete deleteNodeAction;
    delete proxyRoot;
    delete nodeScene;
}

void MainWindow::connectActions()
{
    connect(nodeScene, SIGNAL(focusItemChanged(QGraphicsItem*, QGraphicsItem*, Qt::FocusReason)), this, SLOT(getNewFocusItem(QGraphicsItem*, QGraphicsItem*, Qt::FocusReason)));
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
    addRoot();
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
    addRoot();
    file.close();

    /// TODO --> read file, create nodes and connect them to each other

    YAML::Node fileNode = YAML::LoadFile(fileName.toStdString());
    RootNode* rootNode = (RootNode*) proxyRoot->widget();

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

    if (proxyRoot == nullptr)
        qDebug() << "ERROR: no root !";

    emit saveRequested(&out);


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
/// ===================== BASIC FUNCTIONS TO HANDLE NODES =====================
/// ===========================================================================

void MainWindow::createActions()
{
    deleteNodeAction = new QAction(tr("&Delete"), this);
    connect(deleteNodeAction, &QAction::triggered, this, &MainWindow::deleteNode);
}

void MainWindow::addRoot(QStringList* outputs)
{
    RootNode* rootNode = new RootNode(outputs);

    // add a graphic widget as parent & proxy to communicate between node and scene
    NodeParentWidget* nodeParentWidget = new NodeParentWidget();
    nodeParentWidget->setGeometry(rootNode->geometry());
    nodeParentWidget->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    nodeScene->addItem(nodeParentWidget);
    //nodeScene->addRect(node->geometry(), QPen(Qt::red));
    proxyRoot = nodeScene->addWidget(rootNode);
    proxyRoot->setParentItem(nodeParentWidget);

    // Add one output connector
    ConnectorBase* outputConnector = new OutputConnector(nodeParentWidget);
    connect(rootNode, SIGNAL(transferOutputsRequested(QStringList*)), outputConnector, SLOT(transferOutputs(QStringList*)));
    outputConnector->setGeometry(rootNode->width() - 7, 20, 15, 15);
    QGraphicsProxyWidget* outputConnectorProxy = nodeScene->addWidget(outputConnector);
    outputConnectorProxy->setParentItem(nodeParentWidget);
    rootNode->setOutputConnector(outputConnectorProxy);
    connectConnector(outputConnector, nodeParentWidget);

    connect(rootNode, SIGNAL(resized(QRectF)), nodeParentWidget, SLOT(resize(QRectF)));
    connect(rootNode, SIGNAL(nodeContextMenuRequested(QPoint)), this, SLOT(nodeContextMenu(QPoint)));
    connect(rootNode, SIGNAL(addOutputConnectorRequested()), this, SLOT(actionAddOutputConnector()));
    connect(rootNode, SIGNAL(deleteOutputConnectorRequested()), this, SLOT(actionDeleteOutputConnector()));
    nodeScene->setFocusItem(nodeParentWidget);
    emit rootNode->transferOutputsRequested(rootNode->getOutputs());
    connect(this, SIGNAL(saveRequested(YAML::Emitter*)), rootNode, SLOT(save(YAML::Emitter*)));
}

void MainWindow::updateRoot()
{
    ((RootNode*) proxyRoot->widget())->updateOutputs();
}

QGraphicsProxyWidget* MainWindow::addNode(NodeBase* node)
{
    // add a graphic widget as parent & proxy to communicate between node and scene
    NodeParentWidget* nodeParentWidget = new NodeParentWidget();
    nodeParentWidget->setGeometry(node->geometry());
    nodeParentWidget->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    nodeScene->addItem(nodeParentWidget);
    QGraphicsProxyWidget* proxyNode = nodeScene->addWidget(node);
    proxyNode->setParentItem(nodeParentWidget);

    // Add input connector
    ConnectorBase* inputConnector = new InputConnector(nodeParentWidget);
    connect(inputConnector, SIGNAL(transferOutputsRequested(QStringList*)), node, SLOT(transferOutputs(QStringList*)));
    inputConnector->setGeometry(-8, 20, 15, 15);
    QGraphicsProxyWidget* inputConnectorProxy = nodeScene->addWidget(inputConnector);
    inputConnectorProxy->setParentItem(nodeParentWidget);
    node->setInputConnector(inputConnectorProxy);
    connectConnector(inputConnector, nodeParentWidget);
    connect(inputConnector, SIGNAL(saveRequested(YAML::Emitter*)), node, SLOT(save(YAML::Emitter*)));


    if (node->getTypeOfNode() != CONSTANTMAPNODE)
    {
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
        connectConnector(outputConnector, nodeParentWidget);
        connect(node, SIGNAL(addOutputConnectorRequested()), this, SLOT(actionAddOutputConnector()));
        connect(node, SIGNAL(deleteOutputConnectorRequested()), this, SLOT(actionDeleteOutputConnector()));
    }

    connect(node, SIGNAL(resized(QRectF)), nodeParentWidget, SLOT(resize(QRectF)));
    connect(node, SIGNAL(nodeContextMenuRequested(QPoint)), this, SLOT(nodeContextMenu(QPoint)));
    nodeScene->setFocusItem(nodeParentWidget);

    //// ---------------------------------------------------------------

    if (node->getTypeOfNode() == AFFINEMAPNODE)
    {
        // Add one output connector
        ConnectorBase* outputConnector = new MathsOutputConnector(nodeParentWidget);
        connect(node, SIGNAL(transferOutputsRequested(QStringList*)), outputConnector, SLOT(transferOutputs(QStringList*)));
        int x, y;
        x = node->width() - 7;
        y = 58;
        outputConnector->setGeometry(x, y, 15, 15);
        QGraphicsProxyWidget* outputConnectorProxy = nodeScene->addWidget(outputConnector);
        outputConnectorProxy->setParentItem(nodeParentWidget);
        node->addOutputConnector(outputConnectorProxy);
        connectConnector(outputConnector, nodeParentWidget);
        connect(node, SIGNAL(addOutputConnectorRequested()), this, SLOT(actionAddOutputConnector()));
        connect(node, SIGNAL(deleteOutputConnectorRequested()), this, SLOT(actionDeleteOutputConnector()));
    }

    /// -----------------------------------------------------------------

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

    connectConnector(outputConnector, nodeParentWidget);
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
    ((ConnectorBase*) outputConnector->widget())->deleteLine();
    //delete outputConnector->widget();
    delete outputConnector;
    currentNode->performResize();
    return true;
}

void MainWindow::connectConnector(ConnectorBase* connector, NodeParentWidget* nodeParentWidget)
{
    connect(connector, SIGNAL(createConnectorLineResquested(ConnectorBase*)), this, SLOT(actionCreateConnectorLine(ConnectorBase*)));
    connect(nodeParentWidget, SIGNAL(hasMoved()), connector, SLOT(nodeParentWidgetHasMoved()));
    connect(connector, SIGNAL(isConnectorLineOnMe(ConnectorBase*)), this, SLOT(actionCheckIfConnectorNeedsConnection(ConnectorBase*)));
}

void MainWindow::addOutputConnectorAtPos(QPointF pos)
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

    connectConnector(outputConnector, nodeParentWidget);
}

void MainWindow::deleteOutputConnectorAtPos(QPointF pos)
{
    UNUSED(pos);
}

void MainWindow::addConnectorLineToScene(ConnectorLine* connectorLine)
{
    ConnectorLineParentWidget* connectorLineParentWidget = new ConnectorLineParentWidget();
    connectorLineParentWidget->setFlags(QGraphicsItem::ItemStacksBehindParent);
    nodeScene->addItem(connectorLineParentWidget);
    QGraphicsProxyWidget* connectorLineProxy = nodeScene->addWidget(connectorLine);
    connectorLineProxy->setParentItem(connectorLineParentWidget);
    connectorLine->setConnectorLineProxy(connectorLineProxy);
    connectorLine->setGeometry(nodeScene->sceneRect().toRect());

    //nodeScene->addRect(connectorLine->geometry(), QPen(Qt::green));

    connect(this, SIGNAL(connectConnectorToLine(ConnectorBase*)), connectorLine, SLOT(connectTo(ConnectorBase*)));
    connect(connectorLine, SIGNAL(drawnIsDone(ConnectorLine*)), this, SLOT(actionConnectorLineDrawn(ConnectorLine*)));
    connect(connectorLine, SIGNAL(connectorLineConnected()), this, SLOT(actionConnectorLineConnected()));
    connect(connectorLine, SIGNAL(deleteConnectorLine(QGraphicsProxyWidget*)), this, SLOT(actionDeleteConnectorLine(QGraphicsProxyWidget*)));
}

void MainWindow::createConnectorLine(ConnectorBase* connector)
{
    ConnectorLine* connectorLine = new ConnectorLine(connector);
    addConnectorLineToScene(connectorLine);
}

void MainWindow::createConnectorLine(OutputConnector* outputConnector, InputConnector* inputConnector)
{
    ConnectorLine* connectorLine = new ConnectorLine(outputConnector, inputConnector);
    addConnectorLineToScene(connectorLine);
}

void MainWindow::deleteConnectorLine(QGraphicsProxyWidget* connectorLineProxy)
{
    nodeScene->removeItem(connectorLineProxy);
    delete connectorLineProxy;
    if (newConnectorLine != nullptr)
        removeNewConnectorLine();
}

void MainWindow::saveNewConnectorLine(ConnectorLine* connectorLine)
{
    newConnectorLine = connectorLine;
}

void MainWindow::removeNewConnectorLine()
{
    newConnectorLine = nullptr;
}

void MainWindow::checkConnectionBetweenConnectorAndLine(ConnectorBase* connector)
{
    if (newConnectorLine == nullptr)
        return;
    QPointF positionToCheck = newConnectorLine->getPositionToCheck();
    if (abs(connector->getCenterPos().x() - positionToCheck.x()) <= 7.5 && abs(connector->getCenterPos().y() - positionToCheck.y()) <= 7.5)
        emit connectConnectorToLine(connector);
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
    QGraphicsProxyWidget* proxyNode = addConstantMapNode(outputs_node, values);

    // move it next to parent node
    QRect parentGeometry = parentNode->geometry();
    int top = parentGeometry.top();
    int right = parentGeometry.right();
    ((NodeParentWidget*) proxyNode->parentWidget())->move(right + 50, top);

    ConstantMapNode* constantMapNode = (ConstantMapNode*) proxyNode->widget();
    InputConnector* inputConnector = (InputConnector*) constantMapNode->getInputConnector()->widget();
    OutputConnector* outputConnector = parentNode->getFirstAvailableOutputConnector();

    createConnectorLine(outputConnector, inputConnector);

    /// TODO: add components if lukas says okay

    if (outputsNeedUpdate)
        updateRoot();
}


/// ===========================================================================
/// ============================ ADD MAP FUNCTIONS ============================
/// ===========================================================================

QGraphicsProxyWidget* MainWindow::addConstantMapNode(QStringList* outputs, QList<double>* values)
{
    ConstantMapNode* constantMapNode = new ConstantMapNode(outputs, values);
    return addNode(constantMapNode);
}

QGraphicsProxyWidget* MainWindow::addAffineMapNode(QStringList* outputs, QList<double>* values)
{
    AffineMapNode* affineMapNode = new AffineMapNode(outputs, values);
    return addNode(affineMapNode);
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

void MainWindow::actionConnectorLineConnected()
{
    removeNewConnectorLine();
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

void MainWindow::actionAddAffineMap()
{
    addAffineMapNode();
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

