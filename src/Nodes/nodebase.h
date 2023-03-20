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

    int getTypeOfNode();
    QSharedPointer<QStringList> getInputs() const;
    QSharedPointer<QStringList> getOutputs() const;

    QGraphicsProxyWidget* getInputConnector() const;
    void setInputConnector(QGraphicsProxyWidget* newInputConnector);

    QList<QGraphicsProxyWidget*>* getOutputConnectors() const;
    void addOutputConnector(QGraphicsProxyWidget* newOutputConnector);

    QList<QGraphicsProxyWidget*>* getMathOutputConnectors() const;
    void addMathOutputConnector(QGraphicsProxyWidget* newMathOutputConnector);

    QList<QGraphicsProxyWidget*>* getFunctionOutputConnectors() const;
    void addFunctionOutputConnector(QGraphicsProxyWidget* newFunctionOutputConnector);

    virtual OutputConnector* getFirstAvailableOutputConnector();

    virtual void performResize();
    virtual void clearNodes();

    bool event(QEvent* event);
    void setProxyNode(QGraphicsProxyWidget* newProxyNode);
    QGraphicsProxyWidget* getProxyNode() const;

    void setOutputConnectorModel(QGraphicsProxyWidget* newOutputConnectorModel);
    OutputConnector* getOutputConnectorModel() const;

signals:
    void nodeContextMenuRequested(QPoint pos);
    void resized(QRectF rect);
    OutputConnector* addOutputConnectorRequested(QGraphicsProxyWidget* proxyNode, QPointF pos = QPointF(0, 0));
    void deleteOutputConnectorRequested(QGraphicsProxyWidget* outputConnectorProxy);
    void transferOutputsRequested(QSharedPointer<QStringList> outputs);
    void deleteNodeRequested(QGraphicsProxyWidget* proxyNode);

protected:
    int typeOfNode;
    QString localTag = "";
    int removeButtonIndex = -1;
    int dimensionLineEditIndex = -1;
    QSharedPointer<QStringList> inputs;
    QSharedPointer<QStringList> outputs;
    QGraphicsProxyWidget* proxyNode;
    QGraphicsProxyWidget* inputConnector;
    QList<QGraphicsProxyWidget*>* outputConnectors;
    QList<QGraphicsProxyWidget*>* mathOutputConnectors;
    QList<QGraphicsProxyWidget*>* functionOutputConnectors;
    QGraphicsProxyWidget* outputConnectorModel;
    void mousePressEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);

    void inputsChanged();
    void outputsChanged();
    void saveComponents(YAML::Emitter* out);

    // layouts for all nodes
    void createLayout(bool hasTitleSeparatorLine = true, bool hasAddButton = true, bool hasComponentsLayout = true);
    void addTitleLayout(QVBoxLayout* globalLayout, bool hasTitleSeparatorLine);
    void addDimensionLayout(QVBoxLayout* globalLayout, bool hasAddButton);
    void addComponentsLayout(QVBoxLayout* globalLayout);
    void addRemoveButton(QLayout* layout, int index);
    void removeLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void addSeparatorLineInLayout(QLayout* layout);
    QLabel* addLabel(QLayout* layout, QString text);
    QDoubleSpinBox* addDoubleSpinBox(QLayout* layout);

    void clearLayout(QLayout* layout, bool deleteWidgets = true);

    virtual void deleteOutputConnector();

    virtual void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    virtual void updateLayout();
    virtual void saveNodeContent(YAML::Emitter* out);
    virtual void saveValues(YAML::Emitter* out);

protected slots:
    virtual void addDimensionsLayoutRowRequested(bool clicked);
    virtual void removeDimensionsLayoutRowRequested(bool clicked);
    void dimensionNameChanged(QString newOutput);
    void addOutputConnectorButtonClicked(bool clicked);
    void deleteOutputConnectorButtonClicked(bool clicked);
    QPushButton* getRemoveButtonAtIndex(QVBoxLayout* dimensionsLayout, int index);
    void renameNextRemoveButtons(QVBoxLayout* dimensionsLayout, int index);
    QLineEdit* getDimensionLineEditAtIndex(QVBoxLayout* dimensionsLayout, int index);
    void renameNextDimensionLineEdit(QVBoxLayout* dimensionsLayout, int index);
    void transferOutputsReceived(QSharedPointer<QStringList> newInputs);
    void save(YAML::Emitter* out);
};

#endif // NODE_H
