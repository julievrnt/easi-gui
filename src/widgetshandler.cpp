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
#include "src/Nodes/Maps/luamapnode.h"
#include "src/Nodes/Maps/asaginode.h"
#include "src/Nodes/Maps/scecfilenode.h"
#include "src/Nodes/Maps/evalmodelnode.h"
#include "src/Nodes/Maps/optimalstressnode.h"
#include "src/Nodes/Maps/andersonianstressnode.h"
#include "src/Nodes/Extra/affinematrixnode.h"
#include "src/Nodes/Extra/translationnode.h"
#include "src/Nodes/Extra/polynomialmatrixnode.h"
#include "src/Nodes/Extra/switchcomponentnode.h"
#include "src/Nodes/Extra/functionnode.h"
#include "src/Nodes/Extra/layeredmodelnodenode.h"
#include "src/Nodes/rootnode.h"
#include <QMenu>

WidgetsHandler::WidgetsHandler(EasiGraphicsView* easiGraphicsView, QGraphicsScene* nodeScene)
{
    this->easiGraphicsView = easiGraphicsView;
    this->nodeScene = nodeScene;
    newConnectorLine = nullptr;
    nodes = QList<QGraphicsProxyWidget*>();
    createActions();
    init();
}

WidgetsHandler::~WidgetsHandler()
{
    // easiGraphicsView deleted in mainwindow
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
    connect(rootNode, SIGNAL(transferOutputsRequested(QSharedPointer<QStringList>)), outputConnector, SLOT(transferOutputs(QSharedPointer<QStringList>)));
    emit rootNode->transferOutputsRequested(rootNode->getOutputs());
    connectConnector(outputConnector, proxyRoot);
}

QGraphicsProxyWidget* WidgetsHandler::addNode(NodeBase* node, QPointF pos)
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

    nodeParentWidget->move(pos.x(), pos.y());

    return proxyNode;
}

void WidgetsHandler::deleteNode()
{
    QGraphicsProxyWidget* nodeToRemove = (QGraphicsProxyWidget*) nodeScene->focusItem();
    if (nodeToRemove == nullptr)
    {
        qDebug() << "Could not delete node, no node selected";
        return;
    }

    deleteWidget(nodeToRemove);
}

QPointF WidgetsHandler::getPosNextTo(QGraphicsProxyWidget* parentProxyNode, QPointF pos)
{
    QRectF parentGeometry = ((NodeParentWidget*) parentProxyNode->parentWidget())->geometry();
    int right = parentGeometry.right() + 100 + pos.x();
    int top = parentGeometry.top() + pos.y();
    return QPointF(right, top);
}

void WidgetsHandler::connectNodes(NodeBase* parentNode, NodeBase* childNode)
{
    OutputConnector* outputConnector ;
    if ((parentNode->getTypeOfNode() == EVALMODELNODE || parentNode->getTypeOfNode() == LAYEREDMODELNODE) && !parentNode->getOutputConnectorModel()->getConnectorLineConnected())
    {
        // assume that model is set before components
        outputConnector = parentNode->getOutputConnectorModel();
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
    if (outputConnector->getSubtypeOfConnector() == NONE)
    {
        connect(node, SIGNAL(transferOutputsRequested(QSharedPointer<QStringList>)), outputConnector, SLOT(transferOutputs(QSharedPointer<QStringList>)));
        emit node->transferOutputsRequested(node->getOutputs());
    }
    else if (outputConnector->getSubtypeOfConnector() == SPECIALCOMPONENT && node->getTypeOfNode() != FUNCTIONMAPNODE && node->getTypeOfNode() != LUAMAPNODE)
    {
        connect(node, SIGNAL(transferOutputsRequested(QSharedPointer<QStringList>)), outputConnector, SLOT(transferOutputs(QSharedPointer<QStringList>)));
        emit node->transferOutputsRequested(node->getOutputs());
    }
    else if (outputConnector->getSubtypeOfConnector() == MATH && node->getTypeOfNode() == LAYEREDMODELNODE)
    {
        connect(node, SIGNAL(transferOutputsRequested(QSharedPointer<QStringList>)), outputConnector, SLOT(transferOutputs(QSharedPointer<QStringList>)));
        emit node->transferOutputsRequested(node->getOutputs());
    }
    else if (outputConnector->getSubtypeOfConnector() == EVAL)
    {
        if (node->getTypeOfNode() == EVALMODELNODE)
        {
            connect((EvalModelNode*) node, SIGNAL(transferInputsRequested(QSharedPointer<QStringList>)), outputConnector, SLOT(transferOutputs(QSharedPointer<QStringList>)));
            emit ((EvalModelNode*) node)->transferInputsRequested(node->getOutputs());
        }
        else
        {
            connect((LayeredModelNode*) node, SIGNAL(transferInputsRequested(QSharedPointer<QStringList>)), outputConnector, SLOT(transferOutputs(QSharedPointer<QStringList>)));
            emit ((LayeredModelNode*) node)->transferInputsRequested(node->getOutputs());
        }
    }

    outputConnector->setGeometry(pos.x(), pos.y(), 15, 15);

    QGraphicsProxyWidget* outputConnectorProxy = nodeScene->addWidget(outputConnector);
    outputConnectorProxy->setParentItem(proxyNode->parentItem());
    if (outputConnector->getSubtypeOfConnector() == MATH)
        node->addMathOutputConnector(outputConnectorProxy);
    else if (outputConnector->getSubtypeOfConnector() ==  EVAL)
    {
        if (node->getTypeOfNode() == EVALMODELNODE)
            ((EvalModelNode*) node)->setOutputConnectorModel(outputConnectorProxy);
        else
            ((LayeredModelNode*) node)->setOutputConnectorModel(outputConnectorProxy);
    }
    else if (outputConnector->getSubtypeOfConnector() == SPECIALCOMPONENT)
    {
        if (node->getTypeOfNode() == FUNCTIONMAPNODE)
            ((FunctionMapNode*) node)->addFunctionOutputConnector(outputConnectorProxy);
        else if (node->getTypeOfNode() == LUAMAPNODE)
            ((LuaMapNode*) node)->addFunctionOutputConnector(outputConnectorProxy);
        else if (node->getTypeOfNode() == SWITCHNODE)
            node->addOutputConnector(outputConnectorProxy);
    }
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
                QGraphicsProxyWidget* affineMatrixProxyNode = addAffineMatrixNode(getPosNextTo(proxyNode, QPointF(0, pos.y())), affineMapNode->getInputs());

                InputConnector* affineMatrixInputConnector = (InputConnector*) ((NodeBase*) affineMatrixProxyNode->widget())->getInputConnector()->widget();
                OutputConnector* affineMatrixOutputConnector = (OutputConnector*) affineMapNode->getMathOutputConnectors()->at(numberOfOutputConnectors - 1)->widget();
                affineMapNode->addAffineMatrixProxy(affineMatrixProxyNode);
                createConnectorLine(affineMatrixOutputConnector, affineMatrixInputConnector);
            }
            else
            {
                QGraphicsProxyWidget* translationProxyNode = addTranslationNode(getPosNextTo(proxyNode, QPointF(0, pos.y())));

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
            QGraphicsProxyWidget* polynomialMatrixProxyNode = addPolynomialMatrixNode(getPosNextTo(proxyNode, QPointF(0, pos.y())), polynomialMapNode->getInputs(), polynomialMapNode->getDegree());

            InputConnector* polynomialMatrixInputConnector = (InputConnector*) ((NodeBase*) polynomialMatrixProxyNode->widget())->getInputConnector()->widget();
            OutputConnector* polynomialMatrixOutputConnector = (OutputConnector*) polynomialMapNode->getMathOutputConnectors()->back()->widget();
            polynomialMapNode->addPolynomialMatrixProxy(polynomialMatrixProxyNode);
            createConnectorLine(polynomialMatrixOutputConnector, polynomialMatrixInputConnector);
        }
        else if (node->getTypeOfNode() == LAYEREDMODELNODE)
        {
            LayeredModelNode* layeredModelNode = (LayeredModelNode*) node;
            QGraphicsProxyWidget* matrixProxyNode = addLayeredModelNodeNode(getPosNextTo(proxyNode, QPointF(0, pos.y())), layeredModelNode->getOutputs());

            InputConnector* matrixInputConnector = (InputConnector*) ((NodeBase*) matrixProxyNode->widget())->getInputConnector()->widget();
            OutputConnector* matrixOutputConnector = (OutputConnector*) layeredModelNode->getMathOutputConnectors()->back()->widget();
            layeredModelNode->addNodeProxy(matrixProxyNode);
            createConnectorLine(matrixOutputConnector, matrixInputConnector);
        }
    }
    else  // if switch node, add switch component node
    {
        if (node->getTypeOfNode() == SWITCHNODE)
        {
            SwitchNode* switchNode = (SwitchNode*) node;
            QGraphicsProxyWidget* switchComponentProxy = addSwitchComponentNode(getPosNextTo(proxyNode, QPointF(0, pos.y())), switchNode->getInputs(), switchNode->getOutputs());

            InputConnector* switchComponentInputConnector = (InputConnector*) ((NodeBase*) switchComponentProxy->widget())->getInputConnector()->widget();
            OutputConnector* switchComponentOutputConnector = (OutputConnector*) switchNode->getOutputConnectors()->back()->widget();
            switchNode->addSwitchComponentProxy(switchComponentProxy);
            createConnectorLine(switchComponentOutputConnector, switchComponentInputConnector);
        }
        else if (outputConnector->getSubtypeOfConnector() == SPECIALCOMPONENT && node->getTypeOfNode() == FUNCTIONMAPNODE)
        {
            FunctionMapNode* functionMapNode = (FunctionMapNode*) node;
            QGraphicsProxyWidget* functionNodeProxy = addFunctionNode(getPosNextTo(proxyNode, QPointF(0, pos.y())), functionMapNode->getInputs());

            InputConnector* functionInputConnector = (InputConnector*) ((NodeBase*) functionNodeProxy->widget())->getInputConnector()->widget();
            OutputConnector* functionOutputConnector = (OutputConnector*) functionMapNode->getFunctionOutputConnectors()->back()->widget();
            functionMapNode->addFunctionNodeProxy(functionNodeProxy);
            createConnectorLine(functionOutputConnector, functionInputConnector);
        }
        else if (outputConnector->getSubtypeOfConnector() == SPECIALCOMPONENT && node->getTypeOfNode() == LUAMAPNODE)
        {
            LuaMapNode* luaMapNode = (LuaMapNode*) node;
            QGraphicsProxyWidget* functionNodeProxy = addFunctionNode(getPosNextTo(proxyNode, QPointF(0, pos.y())), luaMapNode->getInputs());

            InputConnector* functionInputConnector = (InputConnector*) ((NodeBase*) functionNodeProxy->widget())->getInputConnector()->widget();
            OutputConnector* functionOutputConnector = (OutputConnector*) luaMapNode->getFunctionOutputConnectors()->back()->widget();
            luaMapNode->setFunctionNodeProxy(functionNodeProxy);
            createConnectorLine(functionOutputConnector, functionInputConnector);
        }
    }
}

void WidgetsHandler::connectConnector(ConnectorBase* connector, QGraphicsProxyWidget* proxyNode)
{
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*) proxyNode->parentItem();
    if (connector->getTypeOfConnector() == INPUTCONNECTOR)
    {
        NodeBase* node = (NodeBase*) proxyNode->widget();
        connect((InputConnector*) connector, SIGNAL(transferOutputsRequested(QSharedPointer<QStringList>)), node, SLOT(transferOutputsReceived(QSharedPointer<QStringList>)));
        connect((InputConnector*) connector, SIGNAL(saveRequested(YAML::Emitter*)), node, SLOT(save(YAML::Emitter*)));
    }
    connect(connector, SIGNAL(createConnectorLineResquested(ConnectorBase*)), this, SLOT(actionCreateConnectorLine(ConnectorBase*)));
    connect(nodeParentWidget, SIGNAL(hasMoved()), connector, SLOT(nodeParentWidgetHasMoved()));
    connect(connector, SIGNAL(isConnectorLineOnMe(ConnectorBase*)), this, SLOT(actionCheckIfConnectorNeedsConnection(ConnectorBase*)));
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

    connect(nodeScene, SIGNAL(sceneRectChanged(QRectF)), connectorLine, SLOT(resize(QRectF)));
    connect(this, SIGNAL(connectConnectorToLine(ConnectorBase*)), connectorLine, SLOT(connectTo(ConnectorBase*)));
    connect(connectorLine, SIGNAL(drawnIsDone(ConnectorLine*)), this, SLOT(actionConnectorLineDrawn(ConnectorLine*)));
    connect(connectorLine, SIGNAL(connectorLineConnected()), this, SLOT(actionConnectorLineConnected()));
    connect(connectorLine, SIGNAL(deleteConnectorLineRequested(QGraphicsProxyWidget*)), this, SLOT(actionDeleteWidget(QGraphicsProxyWidget*)));
}

void WidgetsHandler::createConnectorLine(ConnectorBase* connector)
{
    ConnectorLine* connectorLine = new ConnectorLine(connector, easiGraphicsView);
    addConnectorLineToScene(connectorLine);
}

void WidgetsHandler::createConnectorLine(OutputConnector* outputConnector, InputConnector* inputConnector)
{
    ConnectorLine* connectorLine = new ConnectorLine(outputConnector, inputConnector, easiGraphicsView);
    addConnectorLineToScene(connectorLine);
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

void WidgetsHandler::deleteWidget(QGraphicsProxyWidget* proxy)
{
    qDebug() << "delete widget: " << proxy;
    if (proxy->widget()->objectName() == "Node")
    {
        if (((NodeBase*) proxy->widget())->getTypeOfNode() == ROOTNODE)
            return;

        // remove other nodes
        ((NodeBase*) proxy->widget())->clearNodes();
        nodeScene->removeItem(proxy->parentItem());

        QList<QGraphicsItem*> proxyChildren = proxy->parentItem()->childItems();
        while (!proxyChildren.isEmpty())
        {
            deleteProxy((QGraphicsProxyWidget*) proxyChildren.takeLast());
        }
    }
    else
    {
        nodeScene->removeItem(proxy);
        deleteProxy(proxy);
    }
}

void WidgetsHandler::deleteProxy(QGraphicsProxyWidget* proxy)
{
    qDebug() << "delete proxy: " << proxy;
    if (proxy->widget()->objectName() == "Connector")
    {
        ((ConnectorBase*) proxy->widget())->deleteLine();
    }
    else if (proxy->widget()->objectName() == "Line")
    {
        removeNewConnectorLine();
    }

    delete proxy;
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

QGraphicsProxyWidget* WidgetsHandler::addIncludeNode(QPointF pos, QString filePath)
{
    IncludeNode* includeNode = new IncludeNode(filePath);
    QGraphicsProxyWidget* proxyNode = addNode(includeNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addLayeredModelNode(QPointF pos, QSharedPointer<QStringList> inputs, QSharedPointer<QStringList> outputs, QMap<double, QList<double>>* values, QString interpolation)
{
    LayeredModelNode* layeredModelNode = new LayeredModelNode(inputs, outputs, interpolation);
    QGraphicsProxyWidget* proxyNode = addNode(layeredModelNode, pos);
    layeredModelNode->setValues(values);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add output connector for map
    OutputConnector* modelOutputConnector = new OutputConnector(nodeParentWidget, EVAL);
    addOutputConnector(modelOutputConnector, proxyNode, QPointF(layeredModelNode->geometry().width() - 7, 58));

    return proxyNode;
}


/// ===========================================================================
/// =========================== ADD FILTER FUNCTIONS ==========================
/// ===========================================================================

QGraphicsProxyWidget* WidgetsHandler::addAnyNode(QPointF pos, QSharedPointer<QStringList> inputs)
{
    AnyNode* anyNode = new AnyNode(inputs);
    QGraphicsProxyWidget* proxyNode = addNode(anyNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    addOutputConnector(outputConnector, proxyNode, QPointF(anyNode->geometry().width() - 7, 20));

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addAxisAlignedCuboidalDomainFilterNode(QPointF pos, QSharedPointer<QStringList> inputs, QList<double>* values)
{
    AxisAlignedCuboidalDomainFilterNode* axisAlignedCuboidalDomainFilterNode = new AxisAlignedCuboidalDomainFilterNode(inputs, values);
    QGraphicsProxyWidget* proxyNode = addNode(axisAlignedCuboidalDomainFilterNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF connectorPos(axisAlignedCuboidalDomainFilterNode->geometry().width() - 7, axisAlignedCuboidalDomainFilterNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, connectorPos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addSphericalDomainFilterNode(QPointF pos, QSharedPointer<QStringList> inputs, QList<double>* values)
{
    SphericalDomainFilterNode* sphericalDomainFilterNode = new SphericalDomainFilterNode(inputs, values);
    QGraphicsProxyWidget* proxyNode = addNode(sphericalDomainFilterNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF connectorPos(sphericalDomainFilterNode->geometry().width() - 7, sphericalDomainFilterNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, connectorPos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addGroupFilterNode(QPointF pos, QSharedPointer<QStringList> inputs, QList<double>* values)
{
    GroupFilterNode* groupFilterNode = new GroupFilterNode(inputs, values);
    QGraphicsProxyWidget* proxyNode = addNode(groupFilterNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF connectorPos(groupFilterNode->geometry().width() - 7, groupFilterNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, connectorPos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addSwitchNode(QPointF pos, QSharedPointer<QStringList> inputs, QSharedPointer<QStringList> outputs, QList<QStringList*> values)
{
    SwitchNode* switchNode = new SwitchNode(inputs, outputs);
    QGraphicsProxyWidget* proxyNode = addNode(switchNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget, SPECIALCOMPONENT, false);
    QPointF connectorPos(switchNode->geometry().width() - 7, switchNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, connectorPos);

    switchNode->setValues(values);

    return proxyNode;
}


/// ===========================================================================
/// ============================ ADD MAP FUNCTIONS ============================
/// ===========================================================================

QGraphicsProxyWidget* WidgetsHandler::addConstantMapNode(QPointF pos, QSharedPointer<QStringList> outputs, QList<double>* values)
{
    ConstantMapNode* constantMapNode = new ConstantMapNode(outputs, values);
    QGraphicsProxyWidget* proxyNode = addNode(constantMapNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF connectorPos(constantMapNode->geometry().width() - 7, constantMapNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, connectorPos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addIdentityMapNode(QPointF pos, QSharedPointer<QStringList> inputs)
{
    IdentityMapNode* identityMapNode = new IdentityMapNode(inputs);
    QGraphicsProxyWidget* proxyNode = addNode(identityMapNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    addOutputConnector(outputConnector, proxyNode, QPointF(identityMapNode->geometry().width() - 7, 20));

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addAffineMapNode(QPointF pos, QSharedPointer<QStringList> inputs, QSharedPointer<QStringList> outputs, QMap<QString, QList<double>>* values)
{
    AffineMapNode* affineMapNode = new AffineMapNode(inputs, outputs);
    QGraphicsProxyWidget* proxyNode = addNode(affineMapNode, pos);
    affineMapNode->setValues(values);

    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF connectorPos(affineMapNode->geometry().width() - 7, affineMapNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, connectorPos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addPolynomialMapNode(QPointF pos, QSharedPointer<QStringList> inputs, QSharedPointer<QStringList> outputs, QMap<QString, QList<double>>* values)
{
    PolynomialMapNode* polynomialMapNode = new PolynomialMapNode(inputs, outputs);
    QGraphicsProxyWidget* proxyNode = addNode(polynomialMapNode, pos);
    polynomialMapNode->setValues(values);

    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF connectorPos(polynomialMapNode->geometry().width() - 7, polynomialMapNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, connectorPos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addFunctionMapNode(QPointF pos, QSharedPointer<QStringList> inputs, QSharedPointer<QStringList> outputs, QMap<QString, QString>* values)
{
    FunctionMapNode* functionMapNode = new FunctionMapNode(inputs, outputs);
    QGraphicsProxyWidget* proxyNode = addNode(functionMapNode, pos);
    functionMapNode->setValues(values);

    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF connectorPos(functionMapNode->geometry().width() - 7, functionMapNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, connectorPos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addLuaMapNode(QPointF pos, QSharedPointer<QStringList> inputs, QSharedPointer<QStringList> outputs, QString value)
{
    LuaMapNode* luaMapNode = new LuaMapNode(inputs, outputs);
    QGraphicsProxyWidget* proxyNode = addNode(luaMapNode, pos);

    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add function output connector
    OutputConnector* functionConnector = new OutputConnector(nodeParentWidget, SPECIALCOMPONENT, false);
    addOutputConnector(functionConnector, proxyNode, QPointF(luaMapNode->geometry().width() - 7, 20));

    // set value when opening node
    luaMapNode->setValue(value);

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF connectorPos(luaMapNode->geometry().width() - 7, luaMapNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, connectorPos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addASAGINode(QPointF pos, QSharedPointer<QStringList> outputs, QString filePath, QString var, QString interpolation)
{
    ASAGINode* asagiNode = new ASAGINode(outputs, filePath, var, interpolation);
    QGraphicsProxyWidget* proxyNode = addNode(asagiNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF connectorPos(asagiNode->geometry().width() - 7, asagiNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, connectorPos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addSCECFileNode(QPointF pos, QString filePath, QString interpolation)
{
    SCECFileNode* scecFileNode = new SCECFileNode(filePath, interpolation);
    QGraphicsProxyWidget* proxyNode = addNode(scecFileNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF connectorPos(scecFileNode->geometry().width() - 7, scecFileNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, connectorPos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addEvalModelNode(QPointF pos, QSharedPointer<QStringList> inputs, QSharedPointer<QStringList> outputs)
{
    EvalModelNode* evalModelNode = new EvalModelNode(inputs, outputs);
    QGraphicsProxyWidget* proxyNode = addNode(evalModelNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add output connector for model
    OutputConnector* modelOutputConnector = new OutputConnector(nodeParentWidget, EVAL);
    addOutputConnector(modelOutputConnector, proxyNode, QPointF(evalModelNode->geometry().width() - 7, 58));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF connectorPos(evalModelNode->geometry().width() - 7, evalModelNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, connectorPos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addOptimalStressNode(QPointF pos, double mu_d, double mu_s, double strike, double dip, double rake, double cohesion, double s2ratio, double r, double effectiveConfiningStress)
{
    OptimalStressNode* optimalStressNode = new OptimalStressNode(mu_d, mu_s, strike, dip, rake, cohesion, s2ratio, r, effectiveConfiningStress);
    QGraphicsProxyWidget* proxyNode = addNode(optimalStressNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF connectorPos(optimalStressNode->geometry().width() - 7, optimalStressNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, connectorPos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addAndersonianStressNode(QPointF pos, double mu_d, double mu_s, double sh_max, double s_v, double cohesion, double s2ratio, double s, double sig_zz)
{
    AndersonianStressNode* andersonianStressNode = new AndersonianStressNode(mu_d, mu_s, sh_max, s_v, cohesion, s2ratio, s, sig_zz);
    QGraphicsProxyWidget* proxyNode = addNode(andersonianStressNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    QPointF connectorPos(andersonianStressNode->geometry().width() - 7, andersonianStressNode->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, connectorPos);

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addAffineMatrixNode(QPointF pos, QSharedPointer<QStringList> inputs)
{
    AffineMatrixNode* matrixNode = new AffineMatrixNode(inputs);
    QGraphicsProxyWidget* proxyNode = addNode(matrixNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* mathInputConnector = new InputConnector(nodeParentWidget, MATH, false);
    addInputConnector(mathInputConnector, proxyNode, QPointF(-8, 20));

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addTranslationNode(QPointF pos)
{
    TranslationNode* translationNode = new TranslationNode();
    QGraphicsProxyWidget* proxyNode = addNode(translationNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* mathInputConnector = new InputConnector(nodeParentWidget, MATH, false);
    addInputConnector(mathInputConnector, proxyNode, QPointF(-8, 20));

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addPolynomialMatrixNode(QPointF pos, QSharedPointer<QStringList> inputs, int degree)
{
    PolynomialMatrixNode* matrixNode = new PolynomialMatrixNode(inputs, degree);
    QGraphicsProxyWidget* proxyNode = addNode(matrixNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* mathInputConnector = new InputConnector(nodeParentWidget, MATH, false);
    addInputConnector(mathInputConnector, proxyNode, QPointF(-8, 20));

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addSwitchComponentNode(QPointF pos, QSharedPointer<QStringList> inputs, QSharedPointer<QStringList> outputs)
{
    SwitchComponentNode* switchComponentNode = new SwitchComponentNode(inputs, outputs);
    QGraphicsProxyWidget* proxyNode = addNode(switchComponentNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* inputConnector = new InputConnector(nodeParentWidget, SPECIALCOMPONENT, false);
    addInputConnector(inputConnector, proxyNode, QPointF(-8, 20));

    // Add one output connector
    OutputConnector* outputConnector = new OutputConnector(nodeParentWidget);
    addOutputConnector(outputConnector, proxyNode, QPointF(switchComponentNode->geometry().width() - 7, 20));

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addFunctionNode(QPointF pos, QSharedPointer<QStringList> inputs)
{
    FunctionNode* functionNode = new FunctionNode(inputs);
    QGraphicsProxyWidget* proxyNode = addNode(functionNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* functionInputConnector = new InputConnector(nodeParentWidget, SPECIALCOMPONENT, false);
    addInputConnector(functionInputConnector, proxyNode, QPointF(-8, 20));

    return proxyNode;
}

QGraphicsProxyWidget* WidgetsHandler::addLayeredModelNodeNode(QPointF pos, QSharedPointer<QStringList> inputs)
{
    LayeredModelNodeNode* layeredModelNodeNode = new LayeredModelNodeNode(inputs);
    QGraphicsProxyWidget* proxyNode = addNode(layeredModelNodeNode, pos);
    NodeParentWidget* nodeParentWidget = (NodeParentWidget*)proxyNode->parentWidget();

    // Add one input connector
    InputConnector* mathInputConnector = new InputConnector(nodeParentWidget, MATH, false);
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
    connect(node, SIGNAL(deleteOutputConnectorRequested(QGraphicsProxyWidget*)), this, SLOT(actionDeleteWidget(QGraphicsProxyWidget*)));

    connect(node, SIGNAL(resized(QRectF)), nodeParentWidget, SLOT(resize(QRectF)));
    connect(node, SIGNAL(nodeContextMenuRequested(QPoint)), this, SLOT(nodeContextMenu(QPoint)));
    nodeScene->setFocusItem(nodeParentWidget);

    if (node->getTypeOfNode() == AFFINEMAPNODE)
    {
        AffineMapNode* affineMapNode = (AffineMapNode*) node;
        connect(affineMapNode, SIGNAL(addMathOutputConnectorRequested(QGraphicsProxyWidget*, QPointF)), this, SLOT(actionAddMathOutputConnector(QGraphicsProxyWidget*, QPointF)));
        connect(affineMapNode, SIGNAL(deleteNodeRequested(QGraphicsProxyWidget*)), this, SLOT(actionDeleteWidget(QGraphicsProxyWidget*)));
    }

    if (node->getTypeOfNode() == POLYNOMIALMAPNODE)
    {
        PolynomialMapNode* polynomialMapNode = (PolynomialMapNode*) node;
        connect(polynomialMapNode, SIGNAL(addMathOutputConnectorRequested(QGraphicsProxyWidget*, QPointF)), this, SLOT(actionAddMathOutputConnector(QGraphicsProxyWidget*, QPointF)));
        connect(polynomialMapNode, SIGNAL(deleteNodeRequested(QGraphicsProxyWidget*)), this, SLOT(actionDeleteWidget(QGraphicsProxyWidget*)));
    }

    if (node->getTypeOfNode() == SWITCHNODE)
    {
        SwitchNode* switchNode = (SwitchNode*) node;
        connect(switchNode, SIGNAL(deleteNodeRequested(QGraphicsProxyWidget*)), this, SLOT(actionDeleteWidget(QGraphicsProxyWidget*)));
    }

    if (node->getTypeOfNode() == FUNCTIONMAPNODE)
    {
        FunctionMapNode* functionMapNode = (FunctionMapNode*) node;
        connect(functionMapNode, SIGNAL(addFunctionOutputConnectorRequested(QGraphicsProxyWidget*, QPointF)), this, SLOT(actionAddFunctionOutputConnector(QGraphicsProxyWidget*, QPointF)));
        connect(functionMapNode, SIGNAL(deleteNodeRequested(QGraphicsProxyWidget*)), this, SLOT(actionDeleteWidget(QGraphicsProxyWidget*)));
    }

    if (node->getTypeOfNode() == LUAMAPNODE)
    {
        LuaMapNode* luaMapNode = (LuaMapNode*) node;
        connect(luaMapNode, SIGNAL(deleteNodeRequested(QGraphicsProxyWidget*)), this, SLOT(actionDeleteWidget(QGraphicsProxyWidget*)));
    }

    if (node->getTypeOfNode() == LAYEREDMODELNODE)
    {
        LayeredModelNode* layeredModelNode = (LayeredModelNode*) node;
        connect(layeredModelNode, SIGNAL(addMathOutputConnectorRequested(QGraphicsProxyWidget*, QPointF)), this, SLOT(actionAddMathOutputConnector(QGraphicsProxyWidget*, QPointF)));
        connect(layeredModelNode, SIGNAL(deleteNodeRequested(QGraphicsProxyWidget*)), this, SLOT(actionDeleteWidget(QGraphicsProxyWidget*)));
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
    NodeBase* node = (NodeBase*) proxyNode->widget();
    OutputConnector* outputConnector;
    if (node->getTypeOfNode() == SWITCHNODE)
        outputConnector = new OutputConnector((NodeParentWidget*)proxyNode->parentWidget(), SPECIALCOMPONENT, false);
    else
        outputConnector = new OutputConnector((NodeParentWidget*)proxyNode->parentWidget());
    QPointF pos(node->geometry().width() - 7, node->geometry().height() - 58);
    addOutputConnector(outputConnector, proxyNode, pos);
    return outputConnector;
}

void WidgetsHandler::actionAddMathOutputConnector(QGraphicsProxyWidget* proxyNode, QPointF pos)
{
    OutputConnector* mathOutputConnector = new OutputConnector((NodeParentWidget*) proxyNode->parentWidget(), MATH, false);
    addOutputConnector(mathOutputConnector, proxyNode, pos);
}

void WidgetsHandler::actionAddFunctionOutputConnector(QGraphicsProxyWidget* proxyNode, QPointF pos)
{
    OutputConnector* mathOutputConnector = new OutputConnector((NodeParentWidget*) proxyNode->parentWidget(), SPECIALCOMPONENT, false);
    addOutputConnector(mathOutputConnector, proxyNode, pos);
}

void WidgetsHandler::actionDeleteWidget(QGraphicsProxyWidget* proxy)
{
    deleteWidget(proxy);
}
