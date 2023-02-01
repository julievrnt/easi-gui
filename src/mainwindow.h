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
    void openNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);
    void openComponents(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);

    // open filter nodes
    void openAnyNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);
    void openAxisAlignedCuboidalDomainFilterNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);
    void openSphericalDomainFilterNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);
    void openGroupFilterNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);
    void openSwitchNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);

    // open map nodes
    void openConstantMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node);
    void openIdentityMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);
    void openAffineMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);
    void openPolynomialMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);
    void openFunctionMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);
    void openASAGINode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);
    void openSCECFileNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);
    void openEvalModelNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);
    void openOptimalStressNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);
    void openAndersonianStressNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);
    void openSpecialMapNode(QGraphicsProxyWidget* parentProxyNode, YAML::Node* node, QStringList* inputs);

private slots:
    void getNewFocusItem(QGraphicsItem* newFocusItem, QGraphicsItem* oldFocusItem, Qt::FocusReason reason);
    void stateChanged();

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
    void actionAddASAGI();
    void actionAddSCECFile();
    void actionAddEvalModel();
    void actionAddOptimalStress();
    void actionAddAndersonianStress();
    void actionAddSpecialMap();

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
