#include "widgetshandler.h"
#include "src/Connectors/connectorlineparentwidget.h"
#include "src/Connectors/outputs/mathoutputconnector.h"
#include "src/Nodes/Filters/anynode.h"
#include "src/Nodes/Filters/axisalignedcuboidaldomainfilternode.h"
#include "src/Nodes/Filters/sphericaldomainfilternode.h"
#include "src/Nodes/Filters/groupfilternode.h"
#include "src/Nodes/Filters/swtichnode.h"
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
#include "src/Nodes/Math/matrixnode.h"
#include "src/Nodes/Math/translationnode.h"
#include "src/Nodes/rootnode.h"
#include "src/helpers.h"
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

void WidgetsHandler::moveNodeNextTo(QGraphicsProxyWidget* parentProxyNode, QGraphicsProxyWidget* childProxyNode)
{
    QRectF parentGeometry = ((NodeParentWidget*) parentProxyNode->parentWidget())->geometry();
    int right = parentGeometry.right() + 100;
    int top = parentGeometry.top();
    ((NodeParentWidget*) childProxyNode->parentWidget())->move(right, top);
}

void WidgetsHandler::connectNodes(NodeBase* parentNode, NodeBase* childNode)
{
    OutputConnector* outputConnector = parentNode->getFirstAvailableOutputConnector();
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
    connect(node, SIGNAL(transferOutputsRequested(QStringList*)), outputConnector, SLOT(transferOutputs(QStringList*)));
    emit node->transferOutputsRequested(node->getOutputs());

    outputConnector->setGeometry(pos.x(), pos.y(), 15, 15);

    QGraphicsProxyWidget* outputConnectorProxy = nodeScene->addWidget(outputConnector);
    outputConnectorProxy->setParentItem(proxyNode->parentItem());
    node->addOutputConnector(outputConnectorProxy);
    node->performResize();

    connectConnector(outputConnector, proxyNode);
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

void WidgetsHandler::addMathInputConnector(MathInputConnector* inputConnector, QGraphicsProxyWidget* proxyNode, QPointF pos)
{
    UNUSED(inputConnector);
    UNUSED(proxyNode);
    UNUSED(pos);
}

void WidgetsHandler::addMathOutputConnector(MathOutputConnector* mathOutputConnector, QGraphicsProxyWidget* proxyNode, QPointF pos)
{
    NodeBase* node = (NodeBase*) proxyNode->widget();
    /// TODO: to delete?
    if (node->getTypeOfNode() != AFFINEMAPNODE)
    {
        connect(node, SIGNAL(transferOutputsRequested(QStringList*)), mathOutputConnector, SLOT(transferOutputs(QStringList*)));
        emit node->transferOutputsRequested(node->getOutputs());
    }

    QGraphicsProxyWidget* mathOutputConnectorProxy = nodeScene->addWidget(mathOutputConnector);
    mathOutputConnectorProxy->setParentItem(proxyNode->parentItem());
    node->addMathOutputConnector(mathOutputConnectorProxy);
    node->performResize();

    mathOutputConnector->setGeometry(pos.x(), pos.y(), 15, 15);

    connectConnector(mathOutputConnector, proxyNode);

    // automatically add matrix node for affine map nodes
    if (node->getTypeOfNode() == AFFINEMAPNODE)
    {
        AffineMapNode* affineMapNode = (AffineMapNode*) node;
        int numberOfMathOutputConnectors = affineMapNode->getMathOutputConnectors()->size();
        if (numberOfMathOutputConnectors % 2 == 1)
        {
            QGraphicsProxyWidget* matrixProxyNode = addMatrixNode(affineMapNode->getInputs());

            // move node next to affine node
            NodeParentWidget* nodeParentWidget = (NodeParentWidget*) proxyNode->parentWidget();
            int right = nodeParentWidget->geometry().right();
            int top = nodeParentWidget->geometry().top() + pos.y();
            ((NodeParentWidget*) matrixProxyNode->parentWidget())->move(right + 100, top - 20);

            MathInputConnector* matrixInputConnector = (MathInputConnector*) ((NodeBase*) matrixProxyNode->widget())->getInputConnector()->widget();
            MathOutputConnector* matrixOutputConnector = (MathOutputConnector*) affineMapNode->getMathOutputConnectors()->at(numberOfMathOutputConnectors - 1)->widget();
            affineMapNode->addMatrixProxy(matrixProxyNode);
            createConnectorLine(matrixOutputConnector, matrixInputConnector);
        }
        else
        {
            QGraphicsProxyWidget* translationProxyNode = addTranslationNode();

            // move node next to affine node
            NodeParentWidget* nodeParentWidget = (NodeParentWidget*) proxyNode->parentWidget();
            int right = nodeParentWidget->geometry().right();
            int top = nodeParentWidget->geometry().top() + pos.y();
            ((NodeParentWidget*) translationProxyNode->parentWidget())->move(right + 100, top - 20);

            MathInputConnector* matrixInputConnector = (MathInputConnector*) ((NodeBase*) translationProxyNode->widget())->getInputConnector()->widget();
            MathOutputConnector* matrixOutputConnector = (MathOutputConnector*) affineMapNode->getMathOutputConnectors()->at(numberOfMathOutputConnectors - 1)->widget();
            affineMapNode->addTranslationProxy(translationProxyNode);
            createConnectorLine(matrixOutputConnector, matrixInputConnector);
        }
    }
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

QGraphicsProxyWidget* WidgetsHandler::addSwitchNode()
{
    SwtichNode* swtichNode = new SwtichNode();
    QGraphicsProxyWidget* proxyNode = addNode(swtichNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(swtichNode->geometry().width() - 7, swtichNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

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

QGraphicsProxyWidget* WidgetsHandler::addAffineMapNode(QStringList* inputs, QMap<QString, QList<double>>* values)
{
    AffineMapNode* affineMapNode = new AffineMapNode(inputs);

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

QGraphicsProxyWidget* WidgetsHandler::addPolynomialMapNode()
{
    PolynomialMapNode* polynomialMapNode = new PolynomialMapNode();
    QGraphicsProxyWidget* proxyNode = addNode(polynomialMapNode);
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

QGraphicsProxyWidget* WidgetsHandler::addASAGINode()
{
    ASAGINode* asagiNode = new ASAGINode();
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

QGraphicsProxyWidget* WidgetsHandler::addSCECFileNode()
{
    SCECFileNode* scecFileNode = new SCECFileNode();
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

QGraphicsProxyWidget* WidgetsHandler::addEvalModelNode()
{
    EvalModelNode* evalModelNode = new EvalModelNode();
    QGraphicsProxyWidget* proxyNode = addNode(evalModelNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF pos(evalModelNode->geometry().width() - 7, evalModelNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addOptimalStressNode()
{
    OptimalStressNode* optimalStressNode = new OptimalStressNode();
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

QGraphicsProxyWidget* WidgetsHandler::addAndersonianStressNode()
{
    AndersonianStressNode* andersonianStressNode = new AndersonianStressNode();
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

QGraphicsProxyWidget* WidgetsHandler::addMatrixNode(QStringList* inputs)
{
    MatrixNode* matrixNode = new MatrixNode(inputs);
    QGraphicsProxyWidget* proxyNode = addNode(matrixNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    MathInputConnector* mathInputConnector = new MathInputConnector(nodeParentWidget);
    addInputConnector(mathInputConnector, proxyNode, QPointF(-8, 20));

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addTranslationNode()
{
    TranslationNode* translationNode = new TranslationNode();
    QGraphicsProxyWidget* proxyNode = addNode(translationNode);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    MathInputConnector* mathInputConnector = new MathInputConnector(nodeParentWidget);
    addInputConnector(mathInputConnector, proxyNode, QPointF(-8, 20));

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
    MathOutputConnector* mathOutputConnector = new MathOutputConnector((NodeParentWidget*) proxyNode->parentWidget());
    addMathOutputConnector(mathOutputConnector, proxyNode, pos);
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
