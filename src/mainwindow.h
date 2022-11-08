#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QMap>
#include <QStringList>
#include "src/Connectors/connectorline.h"
#include "src/Nodes/Maps/constantmapnode.h"
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

private:
    Ui::MainWindow* ui;
    QString fileName;
    QGraphicsScene* nodeScene;
    QStringList* outputParameters;
    QAction* deleteNodeAction;
    QGraphicsProxyWidget* proxyRoot;
    ConnectorLine* newConnectorLine;
    bool notSaved;

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

    // open functions for each node type
    void openConstantMapNode();

    // save functions for each node type
    void saveConstantMapNode(YAML::Emitter* out, ConstantMapNode* node);

    // basic functions to handle nodes
    void createActions();
    QGraphicsProxyWidget* addNode(NodeBase* node);
    void deleteNode();
    void deleteProxy(QGraphicsProxyWidget* proxy);

    // basic functions to handle connectors
    void addOutputConnector();
    bool deleteOutputConnector();
    void connectConnectors(ConnectorBase* connector, NodeParentWidget* nodeParentWidget);
    void createConnectorLine(ConnectorBase* connector);
    void deleteConnectorLine(QGraphicsProxyWidget* connectorLineProxy);
    void saveNewConnectorLine(ConnectorLine* connector);
    void checkConnectionBetweenConnectorAndLine(ConnectorBase* connector);

    // add map functions
    void addConstantMapNode();

private slots:
    void nodeContextMenu(QPoint pos);
    void getNewFocusItem(QGraphicsItem* newFocusItem, QGraphicsItem* oldFocusItem, Qt::FocusReason reason);
    void stateChanged();
    void actionCreateConnectorLine(ConnectorBase* connector);
    void actionDeleteConnectorLine(QGraphicsProxyWidget* connectorLineProxy);
    void actionConnectorLineDrawn(ConnectorLine* connectorLine);
    void actionCheckIfConnectorNeedsConnection(ConnectorBase* connector);
    void actionAddOutputConnector();
    void actionDeleteOutputConnector();
    void actionAddConstantMap();
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
