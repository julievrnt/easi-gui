#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QMap>
#include <QStringList>
#include "widgetshandler.h"
#include "src/easigraphicsview.h"

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
    EasiGraphicsView* easiGraphicsView;
    QGraphicsScene* nodeScene;
    WidgetsHandler* widgetsHandler;
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

    // open node functions
    void openNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);
    void openComponents(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);

    // open builder nodes
    void openIncludeNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);
    void openLayeredModelNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);

    // open filter nodes
    void openAnyNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);
    void openAxisAlignedCuboidalDomainFilterNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);
    void openSphericalDomainFilterNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);
    void openGroupFilterNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);
    void openSwitchNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);

    // open map nodes
    void openConstantMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node);
    void openIdentityMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);
    void openAffineMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);
    void openPolynomialMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);
    void openFunctionMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);
    void openLuaMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);
    void openASAGINode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);
    void openSCECFileNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);
    void openEvalModelNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);
    void openOptimalStressNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);
    void openAndersonianStressNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QSharedPointer<QStringList> inputs);

private slots:
    void getNewFocusItem(QGraphicsItem* newFocusItem, QGraphicsItem* oldFocusItem, Qt::FocusReason reason);
    void stateChanged();

    // action add builders
    void actionAddInclude();
    void actionAddLayeredModel();

    // action add filters
    void actionAddAny();
    void actionAddAxisAlignedCuboidalDomainFilter();
    void actionAddSphericalDomainFilter();
    void actionAddGroupFilter();
    void actionAddSwitch();

    // action add maps
    void actionAddConstantMap();
    void actionAddIdentityMap();
    void actionAddAffineMap();
    void actionAddPolynomialMap();
    void actionAddFunctionMap();
    void actionAddLuaMap();
    void actionAddASAGI();
    void actionAddSCECFile();
    void actionAddEvalModel();
    void actionAddOptimalStress();
    void actionAddAndersonianStress();

    // basic software actions
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
