#include "widgetshandler.h"
#include "src/Connectors/connectorlineparentwidget.h"
#include "src/Nodes/Builders/includenode.h"
#include "src/Nodes/Builders/layeredmodelnode.h"
#include "src/Nodes/Filters/anynode.h"
#include "src/Nodes/Filters/axisalignedcuboidaldomainfilternode.h"
#include "src/Nodes/Filters/sphericaldomainfilternode.h"
#include "src/Nodes/Filters/groupfilternode.h"
#include "src/Nodes/Filters/switchnode.h"
#include "src/Nodes/Maps/constantmapnode.h"
#include "src/Nodes/Maps/identitymapnode.h"
#include "src/Nodes/Maps/affinemapnode.h"
#include "src/Nodes/Maps/polynomialmapnode.h"
#include "src/Nodes/Maps/functionmapnode.h"
#include "src/Nodes/Maps/asaginode.h"
#include "src/Nodes/Maps/scecfilenode.h"
#include "src/Nodes/Maps/evalmodelnode.h"
#include "src/Nodes/Maps/optimalstressnode.h"
#include "src/Nodes/Maps/andersonianstressnode.h"
#include "src/Nodes/Maps/specialmapnode.h"
#include "src/Nodes/Extra/affinematrixnode.h"
#include "src/Nodes/Extra/translationnode.h"
#include "src/Nodes/Extra/polynomialmatrixnode.h"
#include "src/Nodes/Extra/switchcomponentnode.h"
#include "src/Nodes/rootnode.h"
#include <QMenu>

WidgetsHandler::WidgetsHandler(QGraphicsScene* nodeScene)
{
    this->nodeScene = nodeScene;
    newConnectorLine = nullptr;
    nodes = QList<QGraphicsProxyWidget*>();
    createActions();
    init();
}

WidgetsHandler::~WidgetsHandler()
{
    // nodeScene is deleted in mainwindow
    delete deleteNodeAction;
    delete proxyRoot;
    foreach (QGraphicsProxyWidget* proxy, nodes)
    {
        delete proxy;
    }
    delete newConnectorLine;
}

void WidgetsHandler::init()
{
    addRoot();
}

QGraphicsProxyWidget* WidgetsHandler::getProxyRoot() const
{
    return proxyRoot;
}

void WidgetsHandler::addRoot()
{
    RootNode* rootNode = new RootNode();
    proxyRoot = addNode(rootNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*) proxyRoot->parentWidget();

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    outputConnector->setGeometry(rootNode->width() - 7, 20, 15, 15);
    QGraphicsProxyWidget* outputConnectorProxy = nodeScene->addWidget(outputConnector);
    outputConnectorProxy->setParentItem(nodeParentWidget);
    rootNode->setOutputConnector(outputConnectorProxy);
    connect(rootNode, SIGNAL(transferOutputsRequested(QStringList*)), outputConnector, SLOT(transferOutputs(QStringList*)));
    emit rootNode->transferOutputsRequested(rootNode->getOutputs());
    connectConnector(outputConnector, proxyRoot);
}

QGraphicsProxyWidget* WidgetsHandler::addNode(NodeBase* node)
{
    // add a graphic widget as parent & proxy to communicate between node and scene
    NodeParentWidget* nodeParentWidget = new NodeParentWidget();
    nodeParentWidget->setGeometry(node->geometry());
    nodeParentWidget->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    nodeScene->addItem(nodeParentWidget);

    QGraphicsProxyWidget* proxyNode = nodeScene->addWidget(node);
    node->setProxyNode(proxyNode);
    proxyNode->setParentItem(nodeParentWidget);
    connectNode(proxyNode);

    return proxyNode;
}

void WidgetsHandler::deleteNode()
{
    QGraphicsItem* nodeToRemove = nodeScene->focusItem();
    if (nodeToRemove == nullptr)
    {
        qDebug() << "Could not delete node, no node selected";
        return;
    }

    // remove maths nodes
    NodeBase* node = (NodeBase*) ((QGraphicsProxyWidget*) nodeToRemove)->widget();
    if (node->getTypeOfNode() == ROOTNODE)
        return;
    node->clearMathNodes();
    if (node->getTypeOfNode() == SWITCHNODE)
        ((SwitchNode*) node)->clearSwitchComponentNodes();

    // remove parent to remove node & connectors
    nodeScene->removeItem(nodeToRemove->parentItem());
    QList<QGraphicsItem*> proxyChildren = nodeToRemove->parentItem()->childItems();
    while (!proxyChildren.isEmpty())
    {
        deleteProxy((QGraphicsProxyWidget*) proxyChildren.takeLast());
    }
}

/// TODO
void WidgetsHandler::deleteProxy(QGraphicsProxyWidget* proxy)
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

void WidgetsHandler::moveNodeNextTo(QGraphicsProxyWidget* parentProxyNode, QGraphicsProxyWidget* childProxyNode, QPointF pos)
{
    QRectF parentGeometry = ((NodeParentWidget*) parentProxyNode->parentWidget())->geometry();
    int right = parentGeometry.right() + 100 + pos.x();
    int top = parentGeometry.top() + pos.y();
    ((NodeParentWidget*) childProxyNode->parentWidget())->move(right, top);
}

void WidgetsHandler::connectNodes(NodeBase* parentNode, NodeBase* childNode)
{
    OutputConnector* outputConnector ;
    if (parentNode->getTypeOfNode() == EVALMODELNODE && !((EvalModelNode*) parentNode)->getOutputConnectorModel()->getConnectorLineConnected())
    {
        // assume that model is set before components
        outputConnector = ((EvalModelNode*) parentNode)->getOutputConnectorModel();
    }
    else
        outputConnector = parentNode->getFirstAvailableOutputConnector();
    InputConnector* inputConnector = (InputConnector*) childNode->getInputConnector()->widget();
    createConnectorLine(outputConnector, inputConnector);
}

void WidgetsHandler::addInputConnector(InputConnector* inputConnector, QGraphicsProxyWidget* proxyNode, QPointF pos)
{
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*) proxyNode->parentItem();
    NodeBase* node = (NodeBase*) proxyNode->widget();

    inputConnector->setGeometry(pos.x(), pos.y(), 15, 15);

    QGraphicsProxyWidget* inputConnectorProxy = nodeScene->addWidget(inputConnector);
    inputConnectorProxy->setParentItem(nodeParentWidget);
    node->setInputConnector(inputConnectorProxy);

    connectConnector(inputConnector, proxyNode);
}

void WidgetsHandler::addOutputConnector(OutputConnector* outputConnector, QGraphicsProxyWidget* proxyNode, QPointF pos)
{
    NodeBase* node = (NodeBase*) proxyNode->widget();
    if (outputConnector->getSubtypeOfConnector() != EVAL && outputConnector->getSubtypeOfConnector() != MATH)
    {
        connect(node, SIGNAL(transferOutputsRequested(QStringList*)), outputConnector, SLOT(transferOutputs(QStringList*)));
        emit node->transferOutputsRequested(node->getOutputs());
    }
    else if (outputConnector->getSubtypeOfConnector() == EVAL)
    {
        connect((EvalModelNode*) node, SIGNAL(transferInputsRequested(QStringList*)), outputConnector, SLOT(transferOutputs(QStringList*)));
        emit ((EvalModelNode*) node)->transferInputsRequested(node->getOutputs());
    }

    outputConnector->setGeometry(pos.x(), pos.y(), 15, 15);

    QGraphicsProxyWidget* outputConnectorProxy = nodeScene->addWidget(outputConnector);
    outputConnectorProxy->setParentItem(proxyNode->parentItem());
    if (outputConnector->getSubtypeOfConnector() == MATH)
        node->addMathOutputConnector(outputConnectorProxy);
    else if (outputConnector->getSubtypeOfConnector() ==  EVAL)
        ((EvalModelNode*) node)->setOutputConnectorModel(outputConnectorProxy);
    else
        node->addOutputConnector(outputConnectorProxy);
    node->performResize();

    connectConnector(outputConnector, proxyNode);

    if (outputConnector->getSubtypeOfConnector() == MATH)
    {
        // if affine map node, add matrix or translation node
        if (node->getTypeOfNode() == AFFINEMAPNODE)
        {
            AffineMapNode* affineMapNode = (AffineMapNode*) node;
            int numberOfOutputConnectors = affineMapNode->getMathOutputConnectors()->size();
            if (numberOfOutputConnectors % 2 == 1)
            {
                QGraphicsProxyWidget* affineMatrixProxyNode = addAffineMatrixNode(affineMapNode->getInputs());

                // move node next to affine node
                moveNodeNextTo(proxyNode, affineMatrixProxyNode, QPointF(0, pos.y()));

                InputConnector* affineMatrixInputConnector = (InputConnector*) ((NodeBase*) affineMatrixProxyNode->widget())->getInputConnector()->widget();
                OutputConnector* affineMatrixOutputConnector = (OutputConnector*) affineMapNode->getMathOutputConnectors()->at(numberOfOutputConnectors - 1)->widget();
                affineMapNode->addAffineMatrixProxy(affineMatrixProxyNode);
                createConnectorLine(affineMatrixOutputConnector, affineMatrixInputConnector);
            }
            else
            {
                QGraphicsProxyWidget* translationProxyNode = addTranslationNode();

                // move node next to node
                moveNodeNextTo(proxyNode, translationProxyNode, QPointF(0, pos.y()));

                InputConnector* translationInputConnector = (InputConnector*) ((NodeBase*) translationProxyNode->widget())->getInputConnector()->widget();
                OutputConnector* translationOutputConnector = (OutputConnector*) affineMapNode->getMathOutputConnectors()->at(numberOfOutputConnectors - 1)->widget();
                affineMapNode->addTranslationProxy(translationProxyNode);
                createConnectorLine(translationOutputConnector, translationInputConnector);
            }
        }
        // if polynomial map node, add matrix node
        else if (node->getTypeOfNode() == POLYNOMIALMAPNODE)
        {
            PolynomialMapNode* polynomialMapNode = (PolynomialMapNode*) node;
            QGraphicsProxyWidget* polynomialMatrixProxyNode = addPolynomialMatrixNode(polynomialMapNode->getInputs(), polynomialMapNode->getDegree());

            // move node next to node
            moveNodeNextTo(proxyNode, polynomialMatrixProxyNode, QPointF(0, pos.y()));

            InputConnector* polynomialMatrixInputConnector = (InputConnector*) ((NodeBase*) polynomialMatrixProxyNode->widget())->getInputConnector()->widget();
            OutputConnector* polynomialMatrixOutputConnector = (OutputConnector*) polynomialMapNode->getMathOutputConnectors()->back()->widget();
            polynomialMapNode->addPolynomialMatrixProxy(polynomialMatrixProxyNode);
            createConnectorLine(polynomialMatrixOutputConnector, polynomialMatrixInputConnector);
        }
    }
    else
    {
        // if switch node, add switch component node
        if (node->getTypeOfNode() == SWITCHNODE)
        {
            SwitchNode* switchNode = (SwitchNode*) node;
            QGraphicsProxyWidget* switchComponentProxy = addSwitchComponentNode(switchNode->getInputs(), switchNode->getOutputs());

            // move node next to node
            moveNodeNextTo(proxyNode, switchComponentProxy, QPointF(0, pos.y()));

            InputConnector* switchComponentInputConnector = (InputConnector*) ((NodeBase*) switchComponentProxy->widget())->getInputConnector()->widget();
            OutputConnector* switchComponentOutputConnector = (OutputConnector*) switchNode->getOutputConnectors()->back()->widget();
            switchNode->addSwitchComponentProxy(switchComponentProxy);
            createConnectorLine(switchComponentOutputConnector, switchComponentInputConnector);
        }
    }
}

void WidgetsHandler::connectConnector(ConnectorBase* connector, QGraphicsProxyWidget* proxyNode)
{
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*) proxyNode->parentItem();
    if (connector->getTypeOfConnector() == INPUTCONNECTOR)
    {
        NodeBase* node = (NodeBase*) proxyNode->widget();
        connect((InputConnector*) connector, SIGNAL(transferOutputsRequested(QStringList*)), node, SLOT(transferOutputsReceived(QStringList*)));
        connect((InputConnector*) connector, SIGNAL(saveRequested(YAML::Emitter*)), node, SLOT(save(YAML::Emitter*)));
    }
    connect(connector, SIGNAL(createConnectorLineResquested(ConnectorBase*)), this, SLOT(actionCreateConnectorLine(ConnectorBase*)));
    connect(nodeParentWidget, SIGNAL(hasMoved()), connector, SLOT(nodeParentWidgetHasMoved()));
    connect(connector, SIGNAL(isConnectorLineOnMe(ConnectorBase*)), this, SLOT(actionCheckIfConnectorNeedsConnection(ConnectorBase*)));
}

bool WidgetsHandler::deleteOutputConnector(QGraphicsProxyWidget* outputConnectorProxy)
{
    nodeScene->removeItem(outputConnectorProxy);
    ((ConnectorBase*) outputConnectorProxy->widget())->deleteLine();
    //delete outputConnector->widget();
    delete outputConnectorProxy;
    return true;
}

void WidgetsHandler::addConnectorLineToScene(ConnectorLine* connectorLine)
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

void WidgetsHandler::createConnectorLine(ConnectorBase* connector)
{
    ConnectorLine* connectorLine = new ConnectorLine(connector);
    addConnectorLineToScene(connectorLine);
}

void WidgetsHandler::createConnectorLine(OutputConnector* outputConnector, InputConnector* inputConnector)
{
    ConnectorLine* connectorLine = new ConnectorLine(outputConnector, inputConnector);
    addConnectorLineToScene(connectorLine);
}

void WidgetsHandler::deleteConnectorLine(QGraphicsProxyWidget* connectorLineProxy)
{
    nodeScene->removeItem(connectorLineProxy);
    delete connectorLineProxy;
    if (newConnectorLine != nullptr)
        removeNewConnectorLine();
}

void WidgetsHandler::saveNewConnectorLine(ConnectorLine* connectorLine)
{
    newConnectorLine = connectorLine;
}

void WidgetsHandler::removeNewConnectorLine()
{
    newConnectorLine = nullptr;
}

void WidgetsHandler::checkConnectionBetweenConnectorAndLine(ConnectorBase* connector)
{
    if (newConnectorLine == nullptr)
        return;
    QPointF positionToCheck = newConnectorLine->getPositionToCheck();
    if (abs(connector->getCenterPos().x() - positionToCheck.x()) <= 7.5 && abs(connector->getCenterPos().y() - positionToCheck.y()) <= 7.5)
        emit connectConnectorToLine(connector);
}

void WidgetsHandler::createActions()
{
    deleteNodeAction = new QAction(tr("&Delete"), this);
    connect(deleteNodeAction, &QAction::triggered, this, &WidgetsHandler::deleteNode);
}

/// ===========================================================================
/// ===================== BASIC FUNCTIONS TO HANDLE NODES =====================
/// ===========================================================================


/// ===========================================================================
/// =================== BASIC FUNCTIONS TO HANDLE CONNECTORS ==================
/// ===========================================================================


/// ===========================================================================
/// ========================== ADD BUILDER FUNCTIONS ==========================
/// ===========================================================================

QGraphicsProxyWidget* WidgetsHandler::addInclude(QString filePath)
{
    IncludeNode* includeNode = new IncludeNode(filePath);
    QGraphicsProxyWidget* proxyNode = addNode(includeNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addLayeredModel()
{
    LayeredModelNode* layeredModelNode = new LayeredModelNode();
    QGraphicsProxyWidget* proxyNode = addNode(layeredModelNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(layeredModelNode->geometry().width() - 7, layeredModelNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

    return proxyNode;
}


/// ===========================================================================
/// =========================== ADD FILTER FUNCTIONS ==========================
/// ===========================================================================

QGraphicsProxyWidget* WidgetsHandler::addAnyNode(QStringList* inputs)
{
    AnyNode* anyNode = new AnyNode(inputs);
    QGraphicsProxyWidget* proxyNode = addNode(anyNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    addOutputConnector(outputConnector, proxyNode, QPointF(anyNode->geometry().width() - 7, 20));

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addAxisAlignedCuboidalDomainFilterNode(QStringList* inputs, QList<double>* values)
{
    AxisAlignedCuboidalDomainFilterNode* axisAlignedCuboidalDomainFilterNode = new AxisAlignedCuboidalDomainFilterNode(inputs, values);
    QGraphicsProxyWidget* proxyNode = addNode(axisAlignedCuboidalDomainFilterNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(axisAlignedCuboidalDomainFilterNode->geometry().width() - 7, axisAlignedCuboidalDomainFilterNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addSphericalDomainFilterNode(QStringList* inputs, QList<double>* values)
{
    SphericalDomainFilterNode* sphericalDomainFilterNode = new SphericalDomainFilterNode(inputs, values);
    QGraphicsProxyWidget* proxyNode = addNode(sphericalDomainFilterNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(sphericalDomainFilterNode->geometry().width() - 7, sphericalDomainFilterNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addGroupFilterNode(QStringList* inputs, QList<double>* values)
{
    GroupFilterNode* groupFilterNode = new GroupFilterNode(inputs, values);
    QGraphicsProxyWidget* proxyNode = addNode(groupFilterNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(groupFilterNode->geometry().width() - 7, groupFilterNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addSwitchNode(QStringList* inputs, QStringList* outputs, QList<QStringList*> values)
{
    SwitchNode* switchNode = new SwitchNode(inputs, outputs);
    QGraphicsProxyWidget* proxyNode = addNode(switchNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(switchNode->geometry().width() - 7, switchNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

    switchNode->setValues(values);

    return proxyNode;
}


/// ===========================================================================
/// ============================ ADD MAP FUNCTIONS ============================
/// ===========================================================================

QGraphicsProxyWidget* WidgetsHandler::addConstantMapNode(QStringList* outputs, QList<double>* values)
{
    ConstantMapNode* constantMapNode = new ConstantMapNode(outputs, values);
    QGraphicsProxyWidget* proxyNode = addNode(constantMapNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(constantMapNode->geometry().width() - 7, constantMapNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addIdentityMapNode(QStringList* inputs)
{
    IdentityMapNode* identityMapNode = new IdentityMapNode(inputs);
    QGraphicsProxyWidget* proxyNode = addNode(identityMapNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    addOutputConnector(outputConnector, proxyNode, QPointF(identityMapNode->geometry().width() - 7, 20));

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addAffineMapNode(QStringList* inputs, QStringList* outputs, QMap<QString, QList<double>>* values)
{
    AffineMapNode* affineMapNode = new AffineMapNode(inputs, outputs);
    QGraphicsProxyWidget* proxyNode = addNode(affineMapNode);
    affineMapNode->setValues(values);

    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(affineMapNode->geometry().width() - 7, affineMapNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addPolynomialMapNode(QStringList* inputs, QStringList* outputs, QMap<QString, QList<double>>* values)
{
    PolynomialMapNode* polynomialMapNode = new PolynomialMapNode(inputs, outputs);
    QGraphicsProxyWidget* proxyNode = addNode(polynomialMapNode);
    polynomialMapNode->setValues(values);

    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(polynomialMapNode->geometry().width() - 7, polynomialMapNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addFunctionMapNode()
{
    FunctionMapNode* functionMapNode = new FunctionMapNode();
    QGraphicsProxyWidget* proxyNode = addNode(functionMapNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(functionMapNode->geometry().width() - 7, functionMapNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addASAGINode(QStringList* outputs, QString filePath, QString var, QString interpolation)
{
    ASAGINode* asagiNode = new ASAGINode(outputs, filePath, var, interpolation);
    QGraphicsProxyWidget* proxyNode = addNode(asagiNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(asagiNode->geometry().width() - 7, asagiNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addSCECFileNode(QString filePath, QString interpolation)
{
    SCECFileNode* scecFileNode = new SCECFileNode(filePath, interpolation);
    QGraphicsProxyWidget* proxyNode = addNode(scecFileNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(scecFileNode->geometry().width() - 7, scecFileNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addEvalModelNode(QStringList* inputs, QStringList* outputs)
{
    EvalModelNode* evalModelNode = new EvalModelNode(inputs, outputs);
    QGraphicsProxyWidget* proxyNode = addNode(evalModelNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add output connector for model
    OutputConnector* modelOutputConnector = new OutputConnector(nodeParentWidget, EVAL);
    addOutputConnector(modelOutputConnector, proxyNode, QPointF(evalModelNode->geometry().width() - 7, 58));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(evalModelNode->geometry().width() - 7, evalModelNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addOptimalStressNode(double mu_d, double mu_s, double strike, double dip, double rake, double cohesion, double s2ratio, double r, double effectiveConfiningStress)
{
    OptimalStressNode* optimalStressNode = new OptimalStressNode(mu_d, mu_s, strike, dip, rake, cohesion, s2ratio, r, effectiveConfiningStress);
    QGraphicsProxyWidget* proxyNode = addNode(optimalStressNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(optimalStressNode->geometry().width() - 7, optimalStressNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addAndersonianStressNode(double mu_d, double mu_s, double sh_max, double s_v, double cohesion, double s2ratio, double s, double sig_zz)
{
    AndersonianStressNode* andersonianStressNode = new AndersonianStressNode(mu_d, mu_s, sh_max, s_v, cohesion, s2ratio, s, sig_zz);
    QGraphicsProxyWidget* proxyNode = addNode(andersonianStressNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(andersonianStressNode->geometry().width() - 7, andersonianStressNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addSpecialMapNode()
{
    SpecialMapNode* specialMapNode = new SpecialMapNode();
    QGraphicsProxyWidget* proxyNode = addNode(specialMapNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(specialMapNode->geometry().width() - 7, specialMapNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addAffineMatrixNode(QStringList* inputs)
{
    AffineMatrixNode* matrixNode = new AffineMatrixNode(inputs);
    QGraphicsProxyWidget* proxyNode = addNode(matrixNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* mathInputConnector = new InputConnector(nodeParentWidget, MATH);
    addInputConnector(mathInputConnector, proxyNode, QPointF(-8, 20));

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addTranslationNode()
{
    TranslationNode* translationNode = new TranslationNode();
    QGraphicsProxyWidget* proxyNode = addNode(translationNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* mathInputConnector = new InputConnector(nodeParentWidget, MATH);
    addInputConnector(mathInputConnector, proxyNode, QPointF(-8, 20));

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addPolynomialMatrixNode(QStringList* inputs, int degree)
{
    PolynomialMatrixNode* matrixNode = new PolynomialMatrixNode(inputs, degree);
    QGraphicsProxyWidget* proxyNode = addNode(matrixNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* mathInputConnector = new InputConnector(nodeParentWidget, MATH);
    addInputConnector(mathInputConnector, proxyNode, QPointF(-8, 20));

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addSwitchComponentNode(QStringList* inputs, QStringList* outputs)
{
    SwitchComponentNode* switchComponentNode = new SwitchComponentNode(inputs, outputs);
    QGraphicsProxyWidget* proxyNode = addNode(switchComponentNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    addOutputConnector(outputConnector, proxyNode, QPointF(switchComponentNode->geometry().width() - 7, 20));

    return proxyNode;
}


/// ===========================================================================
/// ============================== PRIVATE SLOTS ==============================
/// ===========================================================================
void WidgetsHandler::connectNode(QGraphicsProxyWidget* proxyNode)
{
    NodeBase* node = (NodeBase*) proxyNode->widget();
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*) proxyNode->parentWidget();

    if (node->getTypeOfNode() == ROOTNODE)
        connect(this, SIGNAL(saveRequested(YAML::Emitter*)), (RootNode*) node, SLOT(save(YAML::Emitter*)));

    connect(node, SIGNAL(addOutputConnectorRequested(QGraphicsProxyWidget*)), this, SLOT(actionAddOutputConnector(QGraphicsProxyWidget*)));
    connect(node, SIGNAL(deleteOutputConnectorRequested(QGraphicsProxyWidget*)), this, SLOT(actionDeleteOutputConnector(QGraphicsProxyWidget*)));

    connect(node, SIGNAL(resized(QRectF)), nodeParentWidget, SLOT(resize(QRectF)));
    connect(node, SIGNAL(nodeContextMenuRequested(QPoint)), this, SLOT(nodeContextMenu(QPoint)));
    nodeScene->setFocusItem(nodeParentWidget);

    if (node->getTypeOfNode() == AFFINEMAPNODE)
    {
        AffineMapNode* affineMapNode = (AffineMapNode*) node;
        connect(affineMapNode, SIGNAL(addMathOutputConnectorRequested(QGraphicsProxyWidget*, QPointF)), this, SLOT(actionAddMathOutputConnector(QGraphicsProxyWidget*, QPointF)));
        connect(affineMapNode, SIGNAL(deleteNodeRequested(QGraphicsProxyWidget*)), this, SLOT(actionDeleteNode(QGraphicsProxyWidget*)));
    }

    if (node->getTypeOfNode() == POLYNOMIALMAPNODE)
    {
        PolynomialMapNode* polynomialMapNode = (PolynomialMapNode*) node;
        connect(polynomialMapNode, SIGNAL(addMathOutputConnectorRequested(QGraphicsProxyWidget*, QPointF)), this, SLOT(actionAddMathOutputConnector(QGraphicsProxyWidget*, QPointF)));
        connect(polynomialMapNode, SIGNAL(deleteNodeRequested(QGraphicsProxyWidget*)), this, SLOT(actionDeleteNode(QGraphicsProxyWidget*)));
    }

    if (node->getTypeOfNode() == SWITCHNODE)
    {
        SwitchNode* switchNode = (SwitchNode*) node;
        connect(switchNode, SIGNAL(deleteNodeRequested(QGraphicsProxyWidget*)), this, SLOT(actionDeleteNode(QGraphicsProxyWidget*)));
    }
}

void WidgetsHandler::nodeContextMenu(QPoint pos)
{
    QMenu menu;
    menu.addAction(deleteNodeAction);
    menu.exec(pos);
}

void WidgetsHandler::actionCreateConnectorLine(ConnectorBase* connector)
{
    createConnectorLine(connector);
}

void WidgetsHandler::actionDeleteConnectorLine(QGraphicsProxyWidget* connectorLineProxy)
{
    deleteConnectorLine(connectorLineProxy);
}

void WidgetsHandler::actionConnectorLineDrawn(ConnectorLine* connectorLine)
{
    saveNewConnectorLine(connectorLine);
}

void WidgetsHandler::actionConnectorLineConnected()
{
    removeNewConnectorLine();
}

void WidgetsHandler::actionCheckIfConnectorNeedsConnection(ConnectorBase* connector)
{
    checkConnectionBetweenConnectorAndLine(connector);
}

OutputConnector* WidgetsHandler::actionAddOutputConnector(QGraphicsProxyWidget* proxyNode)
{
    if (proxyNode == nullptr)
    {
        qDebug() << "Could not add output connector...";
        return nullptr;
    }
    OutputConnector* outputConnector = new OutputConnector((NodeParentWidget*)proxyNode->parentWidget());
    NodeBase* node = (NodeBase*) proxyNode->widget();
    QPointF pos(node->geometry().width() - 7, node->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);
    return outputConnector;
}

void WidgetsHandler::actionDeleteOutputConnector(QGraphicsProxyWidget* outputConnectorProxy)
{
    deleteOutputConnector(outputConnectorProxy);
}

void WidgetsHandler::actionAddMathOutputConnector(QGraphicsProxyWidget* proxyNode, QPointF pos)
{
    OutputConnector* mathOutputConnector = new OutputConnector((NodeParentWidget*) proxyNode->parentWidget(), MATH);
    addOutputConnector(mathOutputConnector, proxyNode, pos);
}

void WidgetsHandler::actionDeleteNode(QGraphicsProxyWidget* proxyNode)
{
    // remove parent to remove node & connectors
    nodeScene->removeItem(proxyNode->parentItem());
    QList<QGraphicsItem*> proxyChildren = proxyNode->parentItem()->childItems();
    while (!proxyChildren.isEmpty())
    {
        deleteProxy((QGraphicsProxyWidget*) proxyChildren.takeLast());
    }
}
