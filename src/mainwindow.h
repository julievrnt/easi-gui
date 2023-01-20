#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QMap>
#include <QStringList>
#include "src/Nodes/nodebase.h"
#include "widgetshandler.h"

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

private:
    Ui::MainWindow* ui;
    QString fileName;
    QGraphicsScene* nodeScene;
    WidgetsHandler* widgetsHandler;
    bool notSaved = false;

    // connect functions
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

    // open node functions
    void openConstantMapNode(NodeBase* parentNode, YAML::Node* node, QStringList* outputs);

private slots:
    //void nodeContextMenu(QPoint pos);
    void getNewFocusItem(QGraphicsItem* newFocusItem, QGraphicsItem* oldFocusItem, Qt::FocusReason reason);
    void stateChanged();
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
