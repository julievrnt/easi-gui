#ifndef NODE_H
#define NODE_H

#include <QWidget>
#include <QPoint>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QGraphicsProxyWidget>
#include "qevent.h"
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include "yaml-cpp/yaml.h"
#include "src/Connectors/outputconnector.h"

#define NODE 0
#define ROOTNODE 1

// builders
#define INCLUDENODE 2
#define LAYEREDMODELNODE 3

// filters
#define ANYNODE 4
#define AXISALIGNEDCUBOIDALDOMAINFILTERNODE 5
#define SPHERICALDOMAINFILTERNODE 6
#define GROUPFILTERNODE 7
#define SWITCHNODE 8

// maps
#define CONSTANTMAPNODE 9
#define IDENTITYMAPNODE 10
#define AFFINEMAPNODE 11
#define POLYNOMIALMAPNODE 12
#define FUNCTIONMAPNODE 13
#define LUAMAPNODE 14
#define ASAGINODE 15
#define SCECFILENODE 16
#define EVALMODELNODE 17
#define OPTIMALSTRESSNODE 18
#define ANDERSONIANSTRESSNODE 19
#define SPECIALMAPNODE 20

// other nodes
#define AFFINEMATHNODE 21
#define POLYNOMIALMATHNODE 22
#define SWITCHCOMPONENTNODE 23
#define FUNCTIONNODE 24
#define LAYEREDMODELNODENODE 25


class NodeBase : public QWidget
{
    Q_OBJECT
public:
    explicit NodeBase(QSharedPointer<QStringList> inputs = nullptr, QSharedPointer<QStringList> outputs = nullptr, QWidget* parent = nullptr);
    ~NodeBase();

    // getters and setters
    int getTypeOfNode();
    QGraphicsProxyWidget* getProxyNode() const;
    void setProxyNode(QGraphicsProxyWidget* newProxyNode);

    QSharedPointer<QStringList> getInputs() const;
    QSharedPointer<QStringList> getOutputs() const;

    bool getHasMenu() const;

    QGraphicsProxyWidget* getInputConnector() const;
    void setInputConnector(QGraphicsProxyWidget* newInputConnector);

    QList<QGraphicsProxyWidget*>* getOutputConnectors() const;
    virtual OutputConnector* getFirstAvailableOutputConnector();
    void addOutputConnector(QGraphicsProxyWidget* newOutputConnector);

    QList<QGraphicsProxyWidget*>* getMathOutputConnectors() const;
    void addMathOutputConnector(QGraphicsProxyWidget* newMathOutputConnector);

    QList<QGraphicsProxyWidget*>* getFunctionOutputConnectors() const;
    void addFunctionOutputConnector(QGraphicsProxyWidget* newFunctionOutputConnector);

    OutputConnector* getOutputConnectorModel() const;
    void setOutputConnectorModel(QGraphicsProxyWidget* newOutputConnectorModel);

    // inherited from QWidget
    bool event(QEvent* event);

    virtual void performResize();
    virtual void clearNodes();

signals:
    void nodeContextMenuRequested(QPoint pos);
    void resized(QRectF rect);
    OutputConnector* addOutputConnectorRequested(QGraphicsProxyWidget* proxyNode, QPointF pos = QPointF(0, 0));
    void deleteOutputConnectorRequested(QGraphicsProxyWidget* outputConnectorProxy);
    void transferOutputsRequested(QSharedPointer<QStringList> outputs);
    void deleteNodeRequested(QGraphicsProxyWidget* proxyNode);

protected:
    int typeOfNode;
    // tag of the node for yaml
    QString localTag = "";
    // proxy of the node
    QGraphicsProxyWidget* proxyNode;
    // list of the names of the inputs
    QSharedPointer<QStringList> inputs;
    // list of the names of the outputs
    QSharedPointer<QStringList> outputs;
    // if the node can be deleted, a menu will be displayed when right clicked on the node
    // extra nodes and the root node cannot be deleted
    bool hasMenu = true;

    // input connector of the node
    QGraphicsProxyWidget* inputConnector;
    // basic output connectors (green connectors)
    QList<QGraphicsProxyWidget*>* outputConnectors;
    // output connector for the maths of AffineMapNode, PolynomialMapNode and LayeredModelNode
    QList<QGraphicsProxyWidget*>* mathOutputConnectors;
    // output connector for the functions of FunctionMapNode and LuaMapNode
    QList<QGraphicsProxyWidget*>* functionOutputConnectors;
    // output connector for the model of EvalModelNode and LayeredModelNode
    QGraphicsProxyWidget* outputConnectorModel;

    // this index is set if a node has a remove button on each row (e.g. ConstantMapNode)
    // it is used to know which row should be deleted
    int removeButtonIndex = -1;
    // when a row is deleted, the placeholders of the line edits of the next rows need to be updated
    // this index gives the index of the line edit on each row
    int dimensionLineEditIndex = -1;

    // inherited from QWidget
    void mousePressEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);

    void inputsChanged();
    void outputsChanged();
    virtual void deleteOutputConnector();

    // layouts for all nodes
    void createLayout(bool hasTitleSeparatorLine = true, bool hasAddButton = true, bool hasComponentsLayout = true);
    void addTitleLayout(QVBoxLayout* globalLayout, bool hasTitleSeparatorLine);
    void addDimensionLayout(QVBoxLayout* globalLayout, bool hasAddButton);
    void addComponentsLayout(QVBoxLayout* globalLayout);
    virtual void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void removeLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void addSeparatorLineInLayout(QLayout* layout);
    void addRemoveButton(QLayout* layout, int index);
    QLabel* addLabel(QLayout* layout, QString text);
    QDoubleSpinBox* addDoubleSpinBox(QLayout* layout);
    virtual void updateLayout();
    void clearLayout(QLayout* layout, bool deleteWidgets = true);

    // save functions
    virtual void saveNodeContent(YAML::Emitter* out);
    virtual void saveValues(YAML::Emitter* out);
    void saveComponents(YAML::Emitter* out);

protected slots:
    virtual void addDimensionsLayoutRowRequested(bool clicked);
    virtual void removeDimensionsLayoutRowRequested(bool clicked);

    void addOutputConnectorButtonClicked(bool clicked);
    void deleteOutputConnectorButtonClicked(bool clicked);

    void dimensionNameChanged(QString newOutput);
    QPushButton* getRemoveButtonAtIndex(QVBoxLayout* dimensionsLayout, int index);
    void renameNextRemoveButtons(QVBoxLayout* dimensionsLayout, int index);
    QLineEdit* getDimensionLineEditAtIndex(QVBoxLayout* dimensionsLayout, int index);
    void renameNextDimensionLineEdit(QVBoxLayout* dimensionsLayout, int index);

    void transferOutputsReceived(QSharedPointer<QStringList> newInputs);
    void save(YAML::Emitter* out);
};

#endif // NODE_H
