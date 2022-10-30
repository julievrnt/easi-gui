#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QMap>
#include <QStringList>
#include "src/Nodes/Maps/constantmapnode.h"
#include "src/Nodes/nodebase.h"
#include "yaml-cpp/emitter.h"

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
    void outputParameterAdded(QList<int> indexes);

private:
    Ui::MainWindow* ui;
    QString fileName;
    QGraphicsScene* nodeScene;
    QStringList* outputParameters;
    QAction* deleteNodeAction;
    QGraphicsProxyWidget* proxyRoot;
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
    void addNewOutputParameter();
    QGraphicsProxyWidget* addNode(NodeBase* node);
    void deleteNode();
    void deleteProxy(QGraphicsItem* proxy);
    void addOutputConnector();
    bool deleteOutputConnector();

    // add map functions
    void addConstantMapNode();

private slots:
    void nodeContextMenu(QPoint pos);
    void getNewFocusItem(QGraphicsItem* newFocusItem, QGraphicsItem* oldFocusItem, Qt::FocusReason reason);
    void stateChanged();
    void actionAddOutputConnector();
    void actionDeleteOutputConnector();
    void actionAddConstantMap();
    void actionNewOutputParameter();
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
