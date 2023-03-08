#include "mainwindow.h"
#include "src/Nodes/rootnode.h"
#include "src/Nodes/Filters/switchnode.h"
#include "ui_mainwindow.h"
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
    // add builder actions
    connect(ui->actionAddLayeredModel, SIGNAL(triggered(bool)), this, SLOT(actionAddLayeredModel()));
    connect(ui->actionAddInclude, SIGNAL(triggered(bool)), this, SLOT(actionAddInclude()));

    // add filter actions
    connect(ui->actionAddAny, SIGNAL(triggered(bool)), this, SLOT(actionAddAny()));
    connect(ui->actionAddAxisAlignedCuboidalDomainFilter, SIGNAL(triggered(bool)), this, SLOT(actionAddAxisAlignedCuboidalDomainFilter()));
    connect(ui->actionAddSphericalDomainFilter, SIGNAL(triggered(bool)), this, SLOT(actionAddSphericalDomainFilter()));
    connect(ui->actionAddGroupFilter, SIGNAL(triggered(bool)), this, SLOT(actionAddGroupFilter()));
    connect(ui->actionAddSwitch, SIGNAL(triggered(bool)), this, SLOT(actionAddSwitch()));

    // add maps actions
    connect(ui->actionAddConstantMap, SIGNAL(triggered(bool)), this, SLOT(actionAddConstantMap()));
    connect(ui->actionAddIdentityMap, SIGNAL(triggered(bool)), this, SLOT(actionAddIdentityMap()));
    connect(ui->actionAddAffineMap, SIGNAL(triggered(bool)), this, SLOT(actionAddAffineMap()));
    connect(ui->actionAddPolynomialMap, SIGNAL(triggered(bool)), this, SLOT(actionAddPolynomialMap()));
    connect(ui->actionAddFunctionMap, SIGNAL(triggered(bool)), this, SLOT(actionAddFunctionMap()));
    connect(ui->actionAddLuaMap, SIGNAL(triggered(bool)), this, SLOT(actionAddLuaMap()));
    connect(ui->actionAddASAGI, SIGNAL(triggered(bool)), this, SLOT(actionAddASAGI()));
    connect(ui->actionAddSCECFile, SIGNAL(triggered(bool)), this, SLOT(actionAddSCECFile()));
    connect(ui->actionAddEvalModel, SIGNAL(triggered(bool)), this, SLOT(actionAddEvalModel()));
    connect(ui->actionAddOptimalStress, SIGNAL(triggered(bool)), this, SLOT(actionAddOptimalStress()));
    connect(ui->actionAddAndersonianStress, SIGNAL(triggered(bool)), this, SLOT(actionAddAndersonianStress()));

    // basic actions
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

    YAML::Node fileNode = YAML::LoadFile(fileName.toStdString());
    RootNode* rootNode = (RootNode*) widgetsHandler->getProxyRoot()->widget();
    openNode(widgetsHandler->getProxyRoot(), &fileNode, rootNode->getOutputs());
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

void MainWindow::openNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* outputs)
{
    QString tagOfNode = QString::fromStdString((*node).Tag());

    // read tag of node
    // builders
    if (tagOfNode == "!LayeredModel")
        openLayeredModelNode(parentProxyNode, node, outputs);
    else if (tagOfNode == "!Include")
        openIncludeNode(parentProxyNode, node, outputs);

    // filters
    else if (tagOfNode == "!Any")
        openAnyNode(parentProxyNode, node, outputs);
    else if (tagOfNode == "!AxisAlignedCuboidalDomainFilter")
        openAxisAlignedCuboidalDomainFilterNode(parentProxyNode, node, outputs);
    else if (tagOfNode == "!SphericalDomainFilter")
        openSphericalDomainFilterNode(parentProxyNode, node, outputs);
    else if (tagOfNode == "!GroupFilter")
        openGroupFilterNode(parentProxyNode, node, outputs);
    else if (tagOfNode == "!Switch")
        openSwitchNode(parentProxyNode, node, outputs);

    // maps
    else if (tagOfNode == "!ConstantMap")
        openConstantMapNode(parentProxyNode, node);
    else if (tagOfNode == "!IdentityMap")
        openIdentityMapNode(parentProxyNode, node, outputs);
    else if (tagOfNode == "!AffineMap")
        openAffineMapNode(parentProxyNode, node, outputs);
    else if (tagOfNode == "!PolynomialMap")
        openPolynomialMapNode(parentProxyNode, node, outputs);
    else if (tagOfNode == "!FunctionMap")
        openFunctionMapNode(parentProxyNode, node, outputs);
    else if (tagOfNode == "!LuaMap")
        openLuaMapNode(parentProxyNode, node, outputs);
    else if (tagOfNode == "!ASAGI")
        openASAGINode(parentProxyNode, node, outputs);
    else if (tagOfNode == "!SCECFile")
        openSCECFileNode(parentProxyNode, node, outputs);
    else if (tagOfNode == "!EvalModel")
        openEvalModelNode(parentProxyNode, node, outputs);
    else if (tagOfNode == "!OptimalStress")
        openOptimalStressNode(parentProxyNode, node, outputs);
    else if (tagOfNode == "!AndersonianStress")
        openAndersonianStressNode(parentProxyNode, node, outputs);
    else
        qDebug() << tagOfNode << " does not exist or has not been implemented yet";
}

void MainWindow::openComponents(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* outputs)
{
    if (!(*node)["components"])
        return;
    YAML::Node componentsNode = (*node)["components"];
    if (componentsNode.IsMap())
    {
        openNode(parentProxyNode, &componentsNode, outputs);
    }
    else if (componentsNode.IsSequence())
    {
        for (std::size_t i = 0; i < componentsNode.size(); i++)
        {
            YAML::Node componentsNodeI = componentsNode[i];
            openNode(parentProxyNode, &componentsNodeI, outputs);
        }
    }
    else
        qDebug() << "components of " << parentProxyNode << " is neither a map nor a sequence";
}

void MainWindow::openIncludeNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    Q_UNUSED(inputs);

    QString file = "";
    if (!(*node).IsNull())
        file = QString::fromStdString((*node).as<std::string>());

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addIncludeNode(widgetsHandler->getPosNextTo(parentProxyNode), file);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());
}

void MainWindow::openLayeredModelNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    if (!(*node)["map"])
    {
        qDebug() << "ERROR: no map part in layered model";
        return;
    }

    if (!(*node)["interpolation"])
    {
        qDebug() << "ERROR: no interpolation part in layered model";
        return;
    }

    if (!(*node)["parameters"])
    {
        qDebug() << "ERROR: no parameters part in layered model";
        return;
    }

    if (!(*node)["nodes"])
    {
        qDebug() << "ERROR: no nodes part in layered model";
        return;
    }

    QString interpolation = QString::fromStdString((*node)["interpolation"].as<std::string>());

    QStringList* outputs = new QStringList();
    if ((*node)["parameters"])
    {
        for (size_t i = 0; i < (*node)["parameters"].size(); i++)
            outputs->append(QString::fromStdString((*node)["parameters"][i].as<std::string>()));
    }

    QMap<double, QList<double>>* values = new QMap<double, QList<double>>();
    for (YAML::iterator it = (*node)["nodes"].begin(); it != (*node)["nodes"].end(); it++)
    {
        double node = it->first.as<double>();
        QList<double> nodeValues;
        for (size_t i = 0; i < it->second.size(); i++)
            nodeValues.append(it->second[i].as<double>());
        values->insert(node, nodeValues);
    }

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addLayeredModelNode(widgetsHandler->getPosNextTo(parentProxyNode), inputs, outputs, values, interpolation);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    // add model
    YAML::Node model = (*node)["map"];
    if (!model.Tag().empty())
        openNode(proxyNode, &model, inputs);
}

void MainWindow::openAnyNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addAnyNode(widgetsHandler->getPosNextTo(parentProxyNode), inputs);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    openComponents(proxyNode, node, inputs);
}

void MainWindow::openAxisAlignedCuboidalDomainFilterNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    if (!(*node)["limits"])
    {
        qDebug() << "ERROR: no limits part in axis aligned cuboidal domain filter";
        return;
    }

    QList<double>* values = new QList<double>();
    for (YAML::const_iterator it = (*node)["limits"].begin(); it != (*node)["limits"].end(); ++it)
    {
        *values << it->second[0].as<double>();
        *values << it->second[1].as<double>();
    }

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addAxisAlignedCuboidalDomainFilterNode(widgetsHandler->getPosNextTo(parentProxyNode), inputs, values);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    openComponents(proxyNode, node, ((NodeBase*)proxyNode->widget())->getOutputs());
}

void MainWindow::openSphericalDomainFilterNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    if (!(*node)["radius"])
    {
        qDebug() << "ERROR: no radius part in spherical domain filter";
        return;
    }

    if (!(*node)["center"])
    {
        qDebug() << "ERROR: no center part in spherical domain filter";
        return;
    }

    QList<double>* values = new QList<double>();

    values->append((*node)["radius"].as<double>());

    for (YAML::const_iterator it = (*node)["center"].begin(); it != (*node)["center"].end(); ++it)
    {
        *values << it->second.as<double>();
    }

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addSphericalDomainFilterNode(widgetsHandler->getPosNextTo(parentProxyNode), inputs, values);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    openComponents(proxyNode, node, inputs);
}

void MainWindow::openGroupFilterNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    // maps matrix and translation values to outputs
    QList<double>* values = new QList<double>();

    YAML::iterator it = node->begin();
    if (it->second.IsScalar())
        values->append(it->second.as<double>());
    else
    {
        for (std::size_t i = 0; i < it->second.size(); i++)
            values->append(it->second[i].as<double>());
    }

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addGroupFilterNode(widgetsHandler->getPosNextTo(parentProxyNode), inputs, values);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    openComponents(proxyNode, node, inputs);
}

void MainWindow::openSwitchNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    if (!node->IsMap())
    {
        qDebug() << "Switch node is not a map!";
        return;
    }

    QList<QStringList*> values;
    for (YAML::iterator it = node->begin(); it != node->end(); ++it)
    {
        QStringList* parameters = new QStringList();
        for (size_t i = 0; i < it->first.size(); i++)
            parameters->append(QString::fromStdString(it->first[i].as<std::string>()));
        values.append(parameters);
    }

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addSwitchNode(widgetsHandler->getPosNextTo(parentProxyNode), inputs, inputs, values);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    // connect each "component" to a switch component node
    SwitchNode* switchNode = (SwitchNode*) proxyNode->widget();
    QList<QGraphicsProxyWidget*> switchComponentProxies = switchNode->getSwitchComponentProxies();
    if (values.size() != switchComponentProxies.size())
    {
        qDebug() << "Open switch node: values.size() != switchComponentProxies.size()";
        return;
    }

    YAML::iterator it = node->begin();
    for (int i = 0; i < switchComponentProxies.size(); i++)
    {
        YAML::Node componentNode = it->second;
        openNode(switchComponentProxies.at(i), &componentNode, inputs);
        it++;
    }
}

void MainWindow::openConstantMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node)
{
    if (!(*node)["map"])
    {
        qDebug() << "ERROR: no map part in constant map";
        return;
    }

    QStringList* outputs = new QStringList();
    QList<double>* values = new QList<double>();
    for (YAML::const_iterator it = (*node)["map"].begin(); it != (*node)["map"].end(); ++it)
    {
        *outputs << it->first.as<std::string>().c_str();
        *values << it->second.as<double>();
    }

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addConstantMapNode(widgetsHandler->getPosNextTo(parentProxyNode), outputs, values);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    openComponents(proxyNode, node, outputs);
}

void MainWindow::openIdentityMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addIdentityMapNode(widgetsHandler->getPosNextTo(parentProxyNode), inputs);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    openComponents(proxyNode, node, inputs);
}

void MainWindow::openAffineMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    // get output and matrix value
    if (!(*node)["matrix"] || !(*node)["translation"])
    {
        qDebug() << "ERROR : no matrix or translation part in affine map";
        return;
    }

    // outputs of Affine Map
    QStringList* outputs = new QStringList();
    // maps matrix and translation values to outputs
    QMap<QString, QList<double>>* values = new QMap<QString, QList<double>>();
    for (YAML::const_iterator it = (*node)["matrix"].begin(); it != (*node)["matrix"].end(); ++it)
    {
        QString output = QString::fromStdString(it->first.as<std::string>());
        outputs->append(output);

        QList<double> matrixValues;
        for (size_t i = 0; i < it->second.size(); i++)
            matrixValues.append(it->second[i].as<double>());
        values->insert(output, matrixValues);
    }

    // get translation part
    for (YAML::const_iterator it = (*node)["translation"].begin(); it != (*node)["translation"].end(); ++it)
    {
        QString output = QString::fromStdString(it->first.as<std::string>());
        double translationValue = it->second.as<double>();

        QList<double> matrixAndTranslationValues = values->value(output);
        matrixAndTranslationValues.append(translationValue);

        values->insert(output, matrixAndTranslationValues);
    }

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addAffineMapNode(widgetsHandler->getPosNextTo(parentProxyNode), inputs, outputs, values);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    openComponents(proxyNode, node, outputs);
}

void MainWindow::openPolynomialMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    if (!(*node)["map"])
    {
        qDebug() << "ERROR: no map part in polynomial map";
        return;
    }

    // outputs of Polynomial Map
    QStringList* outputs = new QStringList();
    // maps matrix and translation values to outputs
    QMap<QString, QList<double>>* values = new QMap<QString, QList<double>>();
    for (YAML::const_iterator it = (*node)["map"].begin(); it != (*node)["map"].end(); ++it)
    {
        QString output = QString::fromStdString(it->first.as<std::string>());
        outputs->append(output);

        QList<double> matrixValues;
        for (size_t i = 0; i < it->second.size(); i++)
            matrixValues.append(it->second[i].as<double>());
        values->insert(output, matrixValues);
    }

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addPolynomialMapNode(widgetsHandler->getPosNextTo(parentProxyNode), inputs, outputs, values);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    openComponents(proxyNode, node, outputs);
}

void MainWindow::openFunctionMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    if (!(*node)["map"])
    {
        qDebug() << "ERROR: no map part in function map";
        return;
    }

    QMap<QString, QString>* values = new QMap<QString, QString>();
    for (YAML::iterator it = (*node)["map"].begin(); it != (*node)["map"].end(); it++)
    {
        values->insert(QString::fromStdString(it->first.as<std::string>()), QString::fromStdString(it->second.as<std::string>()));
    }
    QStringList* outputs = new QStringList(values->keys());

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addFunctionMapNode(widgetsHandler->getPosNextTo(parentProxyNode), inputs, outputs, values);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    openComponents(proxyNode, node, outputs);
}

void MainWindow::openLuaMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    QStringList* outputs = new QStringList();
    YAML::iterator it = (*node).begin();
    if (QString::fromStdString(it->first.as<std::string>()) != "returns")
    {
        qDebug() << "ERROR: no returns part in lua map";
        delete outputs;
        return;
    }

    for (std::size_t i = 0; i < it->second.size(); i++)
    {
        outputs->append(QString::fromStdString(it->second[i].as<std::string>()));
    }

    ++it;
    if (QString::fromStdString(it->first.as<std::string>()) != "function")
    {
        qDebug() << "ERROR: no function part in lua map";
        delete outputs;
        return;
    }

    QString value;
    value = QString::fromStdString(it->second.as<std::string>());

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addLuaMapNode(widgetsHandler->getPosNextTo(parentProxyNode), inputs, outputs, value);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    openComponents(proxyNode, node, outputs);
}

void MainWindow::openASAGINode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    Q_UNUSED(inputs);

    QStringList* outputs = new QStringList();
    if ((*node)["parameters"])
    {
        for (size_t i = 0; i < (*node)["parameters"].size(); i++)
            outputs->append(QString::fromStdString((*node)["parameters"][i].as<std::string>()));
    }

    QString file = "";
    if (!(*node)["file"].IsNull())
        file = QString::fromStdString((*node)["file"].as<std::string>());

    QString var = "";
    if (!(*node)["var"].IsNull())
        var = QString::fromStdString((*node)["var"].as<std::string>());

    QString interpolation = "linear";
    if (!(*node)["interpolation"].IsNull())
        interpolation = QString::fromStdString((*node)["interpolation"].as<std::string>());

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addASAGINode(widgetsHandler->getPosNextTo(parentProxyNode), outputs, file, var, interpolation);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    openComponents(proxyNode, node, outputs);
}

void MainWindow::openSCECFileNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    Q_UNUSED(inputs);

    QString file = "";
    if (!(*node)["file"].IsNull())
        file = QString::fromStdString((*node)["file"].as<std::string>());

    QString interpolation = "linear";
    if (!(*node)["interpolation"].IsNull())
        interpolation = QString::fromStdString((*node)["interpolation"].as<std::string>());

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addSCECFileNode(widgetsHandler->getPosNextTo(parentProxyNode), file, interpolation);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    openComponents(proxyNode, node, ((NodeBase*) proxyNode->widget())->getOutputs());
}

void MainWindow::openEvalModelNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    QStringList* outputs = new QStringList();
    if ((*node)["parameters"])
    {
        for (size_t i = 0; i < (*node)["parameters"].size(); i++)
            outputs->append(QString::fromStdString((*node)["parameters"][i].as<std::string>()));
    }

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addEvalModelNode(widgetsHandler->getPosNextTo(parentProxyNode), inputs, outputs);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    // add model
    YAML::Node model = (*node)["model"];
    if (!model.Tag().empty())
        openNode(proxyNode, &model, inputs);

    openComponents(proxyNode, node, outputs);
}

void MainWindow::openOptimalStressNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    Q_UNUSED(inputs);

    if (!(*node)["constants"])
    {
        qDebug() << "ERROR: no constants part in optimal stress";
        return;
    }

    double mu_d = (*node)["constants"]["mu_d"].as<double>();
    double mu_s = (*node)["constants"]["mu_s"].as<double>();
    double strike = (*node)["constants"]["strike"].as<double>();
    double dip = (*node)["constants"]["dip"].as<double>();
    double rake = (*node)["constants"]["rake"].as<double>();
    double cohesion = (*node)["constants"]["cohesion"].as<double>();
    double s2ratio = (*node)["constants"]["s2ratio"].as<double>();
    double r = (*node)["constants"]["R"].as<double>();
    double effectiveConfiningStress = (*node)["constants"]["effectiveConfiningStress"].as<double>();

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addOptimalStressNode(widgetsHandler->getPosNextTo(parentProxyNode), mu_d, mu_s, strike, dip, rake, cohesion, s2ratio, r, effectiveConfiningStress);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    openComponents(proxyNode, node, ((NodeBase*) proxyNode->widget())->getOutputs());
}

void MainWindow::openAndersonianStressNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs)
{
    Q_UNUSED(inputs);

    if (!(*node)["constants"])
    {
        qDebug() << "ERROR: no constants part in optimal stress";
        return;
    }

    double mu_d = (*node)["constants"]["mu_d"].as<double>();
    double mu_s = (*node)["constants"]["mu_s"].as<double>();
    double sh_max = (*node)["constants"]["SH_max"].as<double>();
    double s_v = (*node)["constants"]["S_v"].as<double>();
    double cohesion = (*node)["constants"]["cohesion"].as<double>();
    double s2ratio = (*node)["constants"]["s2ratio"].as<double>();
    double s = (*node)["constants"]["S"].as<double>();
    double sig_zz = (*node)["constants"]["sig_zz"].as<double>();

    // add node
    QGraphicsProxyWidget* proxyNode = widgetsHandler->addAndersonianStressNode(widgetsHandler->getPosNextTo(parentProxyNode), mu_d, mu_s, sh_max, s_v, cohesion, s2ratio, s, sig_zz);

    // connect them
    widgetsHandler->connectNodes((NodeBase*) parentProxyNode->widget(), (NodeBase*) proxyNode->widget());

    openComponents(proxyNode, node, ((NodeBase*) proxyNode->widget())->getOutputs());
}

void MainWindow::getNewFocusItem(QGraphicsItem* newFocusItem, QGraphicsItem* oldFocusItem, Qt::FocusReason reason)
{
    Q_UNUSED(oldFocusItem);
    Q_UNUSED(reason);
    if (newFocusItem == nullptr)
    {
        enableDisableIcons(false);
    }
    else
    {
        QGraphicsProxyWidget* currentProxyNode = (QGraphicsProxyWidget*) newFocusItem;
        NodeBase* currentNode = (NodeBase*) currentProxyNode->widget();
        if (currentNode == nullptr || currentNode->getTypeOfNode() == ROOTNODE)
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


/// ===========================================================================
/// =========================== ACTION ADD FILTERS ============================
/// ===========================================================================

void MainWindow::actionAddAny()
{
    widgetsHandler->addAnyNode();
}

void MainWindow::actionAddAxisAlignedCuboidalDomainFilter()
{
    widgetsHandler->addAxisAlignedCuboidalDomainFilterNode();
}

void MainWindow::actionAddSphericalDomainFilter()
{
    widgetsHandler->addSphericalDomainFilterNode();
}

void MainWindow::actionAddGroupFilter()
{
    widgetsHandler->addGroupFilterNode();
}

void MainWindow::actionAddSwitch()
{
    widgetsHandler->addSwitchNode();
}


/// ===========================================================================
/// ============================= ACTION ADD MAPS =============================
/// ===========================================================================

void MainWindow::actionAddConstantMap()
{
    widgetsHandler->addConstantMapNode();
}

void MainWindow::actionAddIdentityMap()
{
    widgetsHandler->addIdentityMapNode();
}

void MainWindow::actionAddAffineMap()
{
    widgetsHandler->addAffineMapNode();
}

void MainWindow::actionAddPolynomialMap()
{
    widgetsHandler->addPolynomialMapNode();
}

void MainWindow::actionAddFunctionMap()
{
    widgetsHandler->addFunctionMapNode();
}

void MainWindow::actionAddLuaMap()
{
    widgetsHandler->addLuaMapNode();
}

void MainWindow::actionAddASAGI()
{
    widgetsHandler->addASAGINode();
}

void MainWindow::actionAddSCECFile()
{
    widgetsHandler->addSCECFileNode();
}

void MainWindow::actionAddEvalModel()
{
    widgetsHandler->addEvalModelNode();
}

void MainWindow::actionAddOptimalStress()
{
    widgetsHandler->addOptimalStressNode();
}

void MainWindow::actionAddAndersonianStress()
{
    widgetsHandler->addAndersonianStressNode();
}

void MainWindow::actionAddInclude()
{
    widgetsHandler->addIncludeNode();
}

void MainWindow::actionAddLayeredModel()
{
    widgetsHandler->addLayeredModelNode();
}


/// ===========================================================================
/// ============================== BASIC ACTIONS ==============================
/// ===========================================================================

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

