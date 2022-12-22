#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QMap>
#include <QStringList>
#include "src/Connectors/connectorline.h"
#include "src/Nodes/nodebase.h"
#include "src/Nodes/nodeparentwidget.h"
#include "yaml-cpp/emitter.h"
#include "src/Connectors/connectorbase.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define UNTITLED "Untitled"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

signals:
    void connectConnectorToLine(ConnectorBase* connector);
    void saveRequested(YAML::Emitter* out);

private:
    Ui::MainWindow* ui;
    QString fileName;
    QGraphicsScene* nodeScene;
    QAction* deleteNodeAction;
    QGraphicsProxyWidget* proxyRoot;
    QList<QGraphicsProxyWidget*> nodes;
    ConnectorLine* newConnectorLine;
    bool notSaved = false;

    void connectActions();

    // basic functions in editor
    void clearAll();
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void save();
    bool saveDiscardOrCancelBeforeOpenOrNew();
    void enableDisableIcons(bool enable);

    // basic functions to handle nodes
    void createActions();
    void addRoot(QStringList* outputs = nullptr);
    void updateRoot();
    QGraphicsProxyWidget* addNode(NodeBase* node);
    void deleteNode();
    void deleteProxy(QGraphicsProxyWidget* proxy);

    // basic functions to handle connectors
    void addOutputConnector();
    bool deleteOutputConnector();
    void connectConnector(ConnectorBase* connector, NodeParentWidget* nodeParentWidget);
    void addOutputConnectorAtPos(QPointF pos);
    void deleteOutputConnectorAtPos(QPointF pos);

    // basic functions to handle connector lines
    void addConnectorLineToScene(ConnectorLine* connectorLine);
    void createConnectorLine(ConnectorBase* connector);
    void createConnectorLine(OutputConnector* outputConnector, InputConnector* inputConnector);
    void deleteConnectorLine(QGraphicsProxyWidget* connectorLineProxy);
    void saveNewConnectorLine(ConnectorLine* connector);
    void removeNewConnectorLine();
    void checkConnectionBetweenConnectorAndLine(ConnectorBase* connector);

    // open node functions
    void openConstantMapNode(NodeBase* parentNode, YAML::Node* node, QStringList* outputs);

    // add map functions
    QGraphicsProxyWidget* addConstantMapNode(QStringList* outputs = nullptr, QList<double>* values = nullptr);
    QGraphicsProxyWidget* addAffineMapNode(QStringList* outputs = nullptr, QList<double>* values = nullptr);

private slots:
    void nodeContextMenu(QPoint pos);
    void getNewFocusItem(QGraphicsItem* newFocusItem, QGraphicsItem* oldFocusItem, Qt::FocusReason reason);
    void stateChanged();
    void actionCreateConnectorLine(ConnectorBase* connector);
    void actionDeleteConnectorLine(QGraphicsProxyWidget* connectorLineProxy);
    void actionConnectorLineDrawn(ConnectorLine* connectorLine);
    void actionConnectorLineConnected();
    void actionCheckIfConnectorNeedsConnection(ConnectorBase* connector);
    void actionAddOutputConnector();
    void actionDeleteOutputConnector();
    void actionAddConstantMap();
    void actionAddAffineMap();
    void actionNew();
    void actionOpen();
    void actionClose();
    void actionSave();
    void actionSaveAs();
    void actionDelete();
    void actionCut();
    void actionCopy();
    void actionPaste();
    void actionUndo();
    void actionRedo();
};

#endif // MAINWINDOW_H
