#include "mainwindow.h"
#include "src/Nodes/rootnode.h"
#include "ui_mainwindow.h"
#include "src/Connectors/connector.h"
#include "src/Nodes/nodeparentwidget.h"
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fileName = UNTITLED;
    nodeScene = new QGraphicsScene(this);
    outputParameters = new QStringList();
    // add default output parameters
    *outputParameters << "lambda" << "mu" << "rho";
    createActions();

    this->setWindowTitle(fileName);
    this->setCentralWidget(ui->graphicsView);

    // let it like that for now
    nodeScene->setSceneRect(this->centralWidget()->rect());
    ui->graphicsView->setScene(nodeScene);

    connectActions();
    proxyRoot = addNode(new RootNode());
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
    connect(ui->actionNewOutputParameter, SIGNAL(triggered(bool)), this, SLOT(actionNewOutputParameter()));
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

void MainWindow::addNewOutputParameter()
{
    /// TODO: find the best way to add, remove, save and display the output parameters...
    this->outputParameters->append("Test VALUE");
    this->outputParameters->append("mama");
    this->outputParameters->sort();
    QList<int> test;
    test << outputParameters->indexOf("Test VALUE");// << outputParameters->indexOf("mama");
    emit outputParameterAdded(test);
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
        Connector* inputConnector = new Connector();
        inputConnector->setGeometry(-8, 20, 15, 15);
        QGraphicsProxyWidget* inputConnectorProxy = nodeScene->addWidget(inputConnector);
        inputConnectorProxy->setParentItem(nodeParentWidget);

        node->setInputConnector(inputConnectorProxy);
    }

    // Add one output connector
    Connector* outputConnector = new Connector();
    int x, y;
    x = node->width() - 7;
    y = node->height() - 58;
    outputConnector->setGeometry(x, y, 15, 15);
    QGraphicsProxyWidget* outputConnectorProxy = nodeScene->addWidget(outputConnector);
    outputConnectorProxy->setParentItem(nodeParentWidget);

    node->addOutputConnector(outputConnectorProxy);

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
    //nodeScene->removeItem(proxy);
    /// TODO: have to find how to delete proxy and widgets
    // deleteProxy(proxy);
}

void MainWindow::deleteProxy(QGraphicsItem* proxy)
{
    // Node* node = (Node*) proxy->widget();
    /*if (node != nullptr)
            delete node;
        QGraphicsWidget* nodeParentWidget = (QGraphicsWidget*)proxy->parentItem();
        if (nodeParentWidget != nullptr)
            delete nodeParentWidget;*/
    //delete proxy;
}

void MainWindow::addOutputConnector()
{
    QGraphicsProxyWidget* currentProxyNode = (QGraphicsProxyWidget*) nodeScene->focusItem();
    if (currentProxyNode == nullptr)
    {
        qDebug() << "Could node add connector";
        return;
    }
    NodeBase* currentNode = (NodeBase*) currentProxyNode->widget();

    Connector* outputConnector = new Connector();
    QGraphicsProxyWidget* outputConnectorProxy = nodeScene->addWidget(outputConnector);
    outputConnectorProxy->setParentItem(currentProxyNode->parentItem());
    currentNode->addOutputConnector(outputConnectorProxy);
    currentNode->performResize();

    int x = currentNode->geometry().width() - 7;
    int y = currentNode->geometry().height() - 58;
    outputConnector->setGeometry(x, y, 15, 15);
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


/// ===========================================================================
/// ============================ ADD MAP FUNCTIONS ============================
/// ===========================================================================

void MainWindow::addConstantMapNode()
{
    ConstantMapNode* constantMapNode = new ConstantMapNode(nodeScene, outputParameters);
    addNode(constantMapNode);
    connect(this, SIGNAL(outputParameterAdded(QList<int>)), constantMapNode, SLOT(insertNewOutputAtIndex(QList<int>)));
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

void MainWindow::actionNewOutputParameter()
{
    addNewOutputParameter();
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

