#ifndef WIDGETSHANDLER_H
#define WIDGETSHANDLER_H

#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QMap>
#include <QStringList>
#include "src/Connectors/connectorline.h"
#include "src/Nodes/nodebase.h"
#include "src/Connectors/connectorbase.h"
#include "src/Connectors/inputs/mathinputconnector.h"
#include "src/Connectors/outputs/mathoutputconnector.h"

class WidgetsHandler : public QObject
{
    Q_OBJECT

public:
    WidgetsHandler(QGraphicsScene* nodeScene);
    ~WidgetsHandler();

    void init();
    QGraphicsProxyWidget* addNode(NodeBase* node);
    QGraphicsProxyWidget* getProxyRoot() const;

    // basic functions to handle nodes
    void addRoot();
    void deleteNode();
    void deleteProxy(QGraphicsProxyWidget* proxy);
    void moveNodeNextTo(QGraphicsProxyWidget* parentProxyNode, QGraphicsProxyWidget* childProxyNode);
    void connectNodes(NodeBase* parentNode, NodeBase* childNode);

    // basic functions to handle connectors
    void addInputConnector(InputConnector* inputConnector, QGraphicsProxyWidget* proxyNode, QPointF pos);
    void addOutputConnector(OutputConnector* outputConnector, QGraphicsProxyWidget* proxyNode, QPointF pos);
    void connectConnector(ConnectorBase* connector, QGraphicsProxyWidget* connectorProxy);
    bool deleteOutputConnector(QGraphicsProxyWidget* outputConnectorProxy);

    // special types of connectors
    void addMathInputConnector(MathInputConnector* mathIutputConnector, QGraphicsProxyWidget* proxyNode, QPointF pos);
    void addMathOutputConnector(MathOutputConnector* mathOutputConnector, QGraphicsProxyWidget* proxyNode, QPointF pos);

    // basic functions to handle connector lines
    void addConnectorLineToScene(ConnectorLine* connectorLine);
    void createConnectorLine(ConnectorBase* connector);
    void createConnectorLine(OutputConnector* outputConnector, InputConnector* inputConnector);
    void deleteConnectorLine(QGraphicsProxyWidget* connectorLineProxy);
    void saveNewConnectorLine(ConnectorLine* connectorLine);
    void removeNewConnectorLine();
    void checkConnectionBetweenConnectorAndLine(ConnectorBase* connector);

    // add filter functions
    QGraphicsProxyWidget* addAnyNode(QStringList* inputs = nullptr);
    QGraphicsProxyWidget* addAxisAlignedCuboidalDomainFilterNode(QStringList* inputs = nullptr, QList<double>* values = nullptr);
    QGraphicsProxyWidget* addSphericalDomainFilterNode();
    QGraphicsProxyWidget* addGroupFilterNode();
    QGraphicsProxyWidget* addSwitchNode();

    // add map functions
    QGraphicsProxyWidget* addConstantMapNode(QStringList* outputs = nullptr, QList<double>* values = nullptr);
    QGraphicsProxyWidget* addIdentityMapNode(QStringList* inputs = nullptr);
    QGraphicsProxyWidget* addAffineMapNode(QStringList* inputs = nullptr, QMap<QString, QList<double>>* values = nullptr);
    QGraphicsProxyWidget* addPolynomialMapNode();
    QGraphicsProxyWidget* addFunctionMapNode();
    QGraphicsProxyWidget* addASAGINode();
    QGraphicsProxyWidget* addSCECFileNode();
    QGraphicsProxyWidget* addEvalModelNode();
    QGraphicsProxyWidget* addOptimalStressNode();
    QGraphicsProxyWidget* addAndersonianStressNode();
    QGraphicsProxyWidget* addSpecialMapNode();

    // add math functions
    QGraphicsProxyWidget* addMatrixNode(QStringList* inputs = nullptr);
    QGraphicsProxyWidget* addTranslationNode();

signals:
    void saveRequested(YAML::Emitter* out);
    void connectConnectorToLine(ConnectorBase* connector);

private:
    QGraphicsScene* nodeScene;
    QAction* deleteNodeAction;
    QGraphicsProxyWidget* proxyRoot;
    QList<QGraphicsProxyWidget*> nodes;
    ConnectorLine* newConnectorLine;

    void createActions();

    void connectNode(QGraphicsProxyWidget* proxyNode);

private slots:
    void nodeContextMenu(QPoint pos);
    void actionCreateConnectorLine(ConnectorBase* connector);
    void actionDeleteConnectorLine(QGraphicsProxyWidget* connectorLineProxy);
    void actionConnectorLineDrawn(ConnectorLine* connectorLine);
    void actionConnectorLineConnected();
    void actionCheckIfConnectorNeedsConnection(ConnectorBase* connector);
    OutputConnector* actionAddOutputConnector(QGraphicsProxyWidget* proxyNode);
    void actionDeleteOutputConnector(QGraphicsProxyWidget* outputConnectorProxy);
    void actionAddMathOutputConnector(QGraphicsProxyWidget* proxyNode, QPointF pos);
    void actionDeleteNode(QGraphicsProxyWidget* proxyNode);
};

#endif // WIDGETSHANDLER_H
