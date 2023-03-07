#ifndef WIDGETSHANDLER_H
#define WIDGETSHANDLER_H

#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QMap>
#include <QStringList>
#include "src/Connectors/connectorline.h"
#include "src/Nodes/nodebase.h"
#include "src/Connectors/connectorbase.h"

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
    void moveNodeNextTo(QGraphicsProxyWidget* parentProxyNode, QGraphicsProxyWidget* childProxyNode, QPointF pos = QPointF(0, 0));
    void connectNodes(NodeBase* parentNode, NodeBase* childNode);

    // basic functions to handle connectors
    void addInputConnector(InputConnector* inputConnector, QGraphicsProxyWidget* proxyNode, QPointF pos);
    void addOutputConnector(OutputConnector* outputConnector, QGraphicsProxyWidget* proxyNode, QPointF pos);
    void connectConnector(ConnectorBase* connector, QGraphicsProxyWidget* connectorProxy);
    bool deleteOutputConnector(QGraphicsProxyWidget* outputConnectorProxy);

    // basic functions to handle connector lines
    void addConnectorLineToScene(ConnectorLine* connectorLine);
    void createConnectorLine(ConnectorBase* connector);
    void createConnectorLine(OutputConnector* outputConnector, InputConnector* inputConnector);
    void deleteConnectorLine(QGraphicsProxyWidget* connectorLineProxy);
    void saveNewConnectorLine(ConnectorLine* connectorLine);
    void removeNewConnectorLine();
    void checkConnectionBetweenConnectorAndLine(ConnectorBase* connector);

    // add builder functions
    QGraphicsProxyWidget* addIncludeNode(QString filePath = "");
    QGraphicsProxyWidget* addLayeredModelNode(QStringList* inputs = nullptr, QStringList* outputs = nullptr, QMap<double, QList<double>>* values = nullptr, QString interpolation = "");

    // add filter functions
    QGraphicsProxyWidget* addAnyNode(QStringList* inputs = nullptr);
    QGraphicsProxyWidget* addAxisAlignedCuboidalDomainFilterNode(QStringList* inputs = nullptr, QList<double>* values = nullptr);
    QGraphicsProxyWidget* addSphericalDomainFilterNode(QStringList* inputs = nullptr, QList<double>* values = nullptr);
    QGraphicsProxyWidget* addGroupFilterNode(QStringList* inputs = nullptr, QList<double>* values = nullptr);
    QGraphicsProxyWidget* addSwitchNode(QStringList* inputs = nullptr, QStringList* outputs = nullptr, QList<QStringList*> values = QList<QStringList*>());

    // add map functions
    QGraphicsProxyWidget* addConstantMapNode(QStringList* outputs = nullptr, QList<double>* values = nullptr);
    QGraphicsProxyWidget* addIdentityMapNode(QStringList* inputs = nullptr);
    QGraphicsProxyWidget* addAffineMapNode(QStringList* inputs = nullptr, QStringList* outputs = nullptr, QMap<QString, QList<double>>* values = nullptr);
    QGraphicsProxyWidget* addPolynomialMapNode(QStringList* inputs = nullptr, QStringList* outputs = nullptr, QMap<QString, QList<double>>* values = nullptr);
    QGraphicsProxyWidget* addFunctionMapNode(QStringList* inputs = nullptr, QStringList* outputs = nullptr, QMap<QString, QString>* values = nullptr);
    QGraphicsProxyWidget* addLuaMapNode(QStringList* inputs = nullptr, QStringList* outputs = nullptr, QString value = "");
    QGraphicsProxyWidget* addASAGINode(QStringList* outputs = nullptr, QString filePath = "", QString var = "data", QString interpolation = "linear");
    QGraphicsProxyWidget* addSCECFileNode(QString filePath = "", QString interpolation = "linear");
    QGraphicsProxyWidget* addEvalModelNode(QStringList* inputs = nullptr, QStringList* outputs = nullptr);
    QGraphicsProxyWidget* addOptimalStressNode(double mu_d = 0., double mu_s = 0.,
                                               double strike = 0., double dip = 0.,
                                               double rake = 0., double cohesion = 0.,
                                               double s2ratio = 0., double r = 0.,
                                               double effectiveConfiningStress = 0.);
    QGraphicsProxyWidget* addAndersonianStressNode(double mu_d = 0, double mu_s = 0,
                                                   double sh_max = 0, double s_v = 0,
                                                   double cohesion = 0, double s2ratio = 0,
                                                   double s = 0, double sig_zz = 0);
    QGraphicsProxyWidget* addSpecialMapNode();

    // add other node functions
    QGraphicsProxyWidget* addAffineMatrixNode(QStringList* inputs = nullptr);
    QGraphicsProxyWidget* addTranslationNode();
    QGraphicsProxyWidget* addPolynomialMatrixNode(QStringList* inputs = nullptr, int degree = 0);
    QGraphicsProxyWidget* addSwitchComponentNode(QStringList* inputs = nullptr, QStringList* outputs = nullptr);
    QGraphicsProxyWidget* addFunctionNode(QStringList* inputs = nullptr);

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
    void actionAddFunctionOutputConnector(QGraphicsProxyWidget* proxyNode, QPointF pos);
    void actionDeleteNode(QGraphicsProxyWidget* proxyNode);
};

#endif // WIDGETSHANDLER_H
