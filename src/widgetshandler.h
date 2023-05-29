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
    WidgetsHandler(EasiGraphicsView* easiGraphicsView, QGraphicsScene* nodeScene);
    ~WidgetsHandler();

    void init();
    void deleteWidget(QGraphicsProxyWidget* proxy);
    void deleteProxy(QGraphicsProxyWidget* proxy);

    // basic functions to handle nodes
    void deleteNode();
    void connectNodes(NodeBase* parentNode, NodeBase* childNode);
    QPointF getPosNextTo(QGraphicsProxyWidget* parentProxyNode, QPointF pos = QPointF(0, 0));
    QGraphicsProxyWidget* getProxyRoot() const;

    // add builder functions
    QGraphicsProxyWidget* addIncludeNode(QPointF pos = QPointF(50000, 2500), QString filePath = "");
    QGraphicsProxyWidget* addLayeredModelNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr, QSharedPointer<QStringList> outputs = nullptr, QMap<double, QList<double>>* values = nullptr, QString interpolation = "");

    // add filter functions
    QGraphicsProxyWidget* addAnyNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr);
    QGraphicsProxyWidget* addAxisAlignedCuboidalDomainFilterNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr, QList<double>* values = nullptr);
    QGraphicsProxyWidget* addSphericalDomainFilterNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr, QList<double>* values = nullptr);
    QGraphicsProxyWidget* addGroupFilterNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr, QList<double>* values = nullptr);
    QGraphicsProxyWidget* addSwitchNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr, QSharedPointer<QStringList> outputs = nullptr, QList<QSharedPointer<QStringList>> values = QList<QSharedPointer<QStringList>>());

    // add map functions
    QGraphicsProxyWidget* addConstantMapNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> outputs = nullptr, QList<double>* values = nullptr);
    QGraphicsProxyWidget* addIdentityMapNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr);
    QGraphicsProxyWidget* addAffineMapNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr, QSharedPointer<QStringList> outputs = nullptr, QMap<QString, QList<double>>* values = nullptr);
    QGraphicsProxyWidget* addPolynomialMapNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr, QSharedPointer<QStringList> outputs = nullptr, QMap<QString, QList<double>>* values = nullptr);
    QGraphicsProxyWidget* addFunctionMapNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr, QSharedPointer<QStringList> outputs = nullptr, QMap<QString, QString>* values = nullptr);
    QGraphicsProxyWidget* addLuaMapNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr, QSharedPointer<QStringList> outputs = nullptr, QString value = "");
    QGraphicsProxyWidget* addASAGINode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> outputs = nullptr, QString filePath = "", QString var = "data", QString interpolation = "linear");
    QGraphicsProxyWidget* addSCECFileNode(QPointF pos = QPointF(50000, 2500), QString filePath = "", QString interpolation = "linear");
    QGraphicsProxyWidget* addEvalModelNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr, QSharedPointer<QStringList> outputs = nullptr);
    QGraphicsProxyWidget* addOptimalStressNode(QPointF pos = QPointF(50000, 2500), double mu_d = 0., double mu_s = 0.,
                                               double strike = 0., double dip = 0.,
                                               double rake = 0., double cohesion = 0.,
                                               double s2ratio = 0., double r = 0.,
                                               double effectiveConfiningStress = 0.);
    QGraphicsProxyWidget* addAndersonianStressNode(QPointF pos = QPointF(50000, 2500), double mu_d = 0, double mu_s = 0,
                                                   double sh_max = 0, double s_v = 0,
                                                   double cohesion = 0, double s2ratio = 0,
                                                   double s = 0, double sig_zz = 0);

    // add other node functions
    QGraphicsProxyWidget* addAffineMatrixNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr);
    QGraphicsProxyWidget* addTranslationNode(QPointF pos = QPointF(50000, 2500));
    QGraphicsProxyWidget* addPolynomialMatrixNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr, int degree = 0);
    QGraphicsProxyWidget* addSwitchComponentNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr, QSharedPointer<QStringList> outputs = nullptr);
    QGraphicsProxyWidget* addFunctionNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr);
    QGraphicsProxyWidget* addLayeredModelNodeNode(QPointF pos = QPointF(50000, 2500), QSharedPointer<QStringList> inputs = nullptr);

private:
    EasiGraphicsView* easiGraphicsView;
    QGraphicsScene* nodeScene;
    QAction* deleteNodeAction;
    QGraphicsProxyWidget* proxyRoot;
    QList<QGraphicsProxyWidget*> nodes;
    ConnectorLine* newConnectorLine;

    void createActions();

    // basic functions to handle nodes
    void addRoot();
    QGraphicsProxyWidget* addNode(NodeBase* node, QPointF pos = QPointF(50000, 2500));
    void connectNode(QGraphicsProxyWidget* proxyNode);

    // basic functions to handle connectors
    void addInputConnector(InputConnector* inputConnector, QGraphicsProxyWidget* proxyNode, QPointF pos);
    void addOutputConnector(OutputConnector* outputConnector, QGraphicsProxyWidget* proxyNode, QPointF pos);
    void connectConnector(ConnectorBase* connector, QGraphicsProxyWidget* connectorProxy);

    // basic functions to handle connector lines
    void addConnectorLineToScene(ConnectorLine* connectorLine);
    void createConnectorLine(ConnectorBase* connector);
    void createConnectorLine(OutputConnector* outputConnector, InputConnector* inputConnector);
    void saveNewConnectorLine(ConnectorLine* connectorLine);
    void removeNewConnectorLine();
    void checkConnectionBetweenConnectorAndLine(ConnectorBase* connector);

signals:
    void saveRequested(YAML::Emitter* out);
    void connectConnectorToLine(ConnectorBase* connector);

private slots:
    void nodeContextMenu(QPoint pos);
    void actionCreateConnectorLine(ConnectorBase* connector);
    void actionConnectorLineDrawn(ConnectorLine* connectorLine);
    void actionConnectorLineConnected();
    void actionCheckIfConnectorNeedsConnection(ConnectorBase* connector);
    OutputConnector* actionAddOutputConnector(QGraphicsProxyWidget* proxyNode);
    void actionAddMathOutputConnector(QGraphicsProxyWidget* proxyNode, QPointF pos);
    void actionAddFunctionOutputConnector(QGraphicsProxyWidget* proxyNode, QPointF pos);
    void actionDeleteWidget(QGraphicsProxyWidget* proxy);
};

#endif // WIDGETSHANDLER_H
