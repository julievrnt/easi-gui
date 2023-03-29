#include "nodebase.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include "float.h"

NodeBase::NodeBase(QSharedPointer<QStringList> inputs, QSharedPointer<QStringList> outputs, QWidget* parent)
    : QWidget{parent}
{
    typeOfNode = NODE;

    this->inputs = inputs;
    if (this->inputs == nullptr)
        this->inputs = QSharedPointer<QStringList>(new QStringList);

    this->outputs = outputs;
    if (this->outputs == nullptr)
        this->outputs = QSharedPointer<QStringList>(new QStringList);

    this->setWindowTitle("Node Base");
    outputConnectors = new QList<QGraphicsProxyWidget*>();
    mathOutputConnectors = new QList<QGraphicsProxyWidget*>();
    functionOutputConnectors = new QList<QGraphicsProxyWidget*>();
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("Node");
}

NodeBase::~NodeBase()
{
    // proxyNode deleted in widgetshandler
    // inputConnector deleted in widgetshandler
    // outputConnectors content deleted in widgetshandler
    // mathOutputConnectors content deleted in widgetshandler
    // functionOutputConnectors content deleted in widgetshandler
    // outputConnectorModel deleted in widgetshandler

    delete outputConnectors;
    outputConnectors = nullptr;
    delete mathOutputConnectors;
    mathOutputConnectors = nullptr;
    delete functionOutputConnectors;
    functionOutputConnectors = nullptr;
}

int NodeBase::getTypeOfNode()
{
    return typeOfNode;
}

QSharedPointer<QStringList> NodeBase::getOutputs() const
{
    return outputs;
}

QGraphicsProxyWidget* NodeBase::getInputConnector() const
{
    return inputConnector;
}

void NodeBase::setInputConnector(QGraphicsProxyWidget* newInputConnector)
{
    inputConnector = newInputConnector;
}

QList<QGraphicsProxyWidget*>* NodeBase::getOutputConnectors() const
{
    return outputConnectors;
}

void NodeBase::addOutputConnector(QGraphicsProxyWidget* newOutputConnector)
{
    outputConnectors->append(newOutputConnector);
}

QList<QGraphicsProxyWidget*>* NodeBase::getMathOutputConnectors() const
{
    return mathOutputConnectors;
}

void NodeBase::addMathOutputConnector(QGraphicsProxyWidget* newMathOutputConnector)
{
    mathOutputConnectors->append(newMathOutputConnector);
}

QList<QGraphicsProxyWidget*>* NodeBase::getFunctionOutputConnectors() const
{
    return functionOutputConnectors;
}

void NodeBase::addFunctionOutputConnector(QGraphicsProxyWidget* newFunctionOutputConnector)
{
    functionOutputConnectors->append(newFunctionOutputConnector);
}

OutputConnector* NodeBase::getFirstAvailableOutputConnector()
{
    foreach (QGraphicsProxyWidget* proxy, *outputConnectors)
    {
        OutputConnector* outputConnector = (OutputConnector*) proxy->widget();
        if (outputConnector->isFree())
            return outputConnector;
    }
    return (emit addOutputConnectorRequested(this->getProxyNode()));
}

void NodeBase::performResize()
{
    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height() + (outputConnectors->size() - 1) * 20));
    emit resized(this->geometry());
    for (int i = 0; i < outputConnectors->size(); i++)
    {
        QGraphicsProxyWidget* outputConnector = outputConnectors->at(i);
        int x = outputConnector->parentWidget()->geometry().width() - 7;
        int y = outputConnector->parentWidget()->geometry().height() - 58 - (outputConnectors->size() - 1 - i) * 20;
        outputConnector->setX(x);
        outputConnector->setY(y);
    }
}

void NodeBase::clearNodes()
{
    // do nothing
}

QSharedPointer<QStringList> NodeBase::getInputs() const
{
    return inputs;
}

QGraphicsProxyWidget* NodeBase::getProxyNode() const
{
    return proxyNode;
}

void NodeBase::setOutputConnectorModel(QGraphicsProxyWidget* newOutputConnectorModel)
{
    outputConnectorModel = newOutputConnectorModel;
}

OutputConnector* NodeBase::getOutputConnectorModel() const
{
    if (outputConnectorModel == nullptr)
        return nullptr;
    return (OutputConnector*) outputConnectorModel->widget();
}

bool NodeBase::getHasMenu() const
{
    return hasMenu;
}

void NodeBase::setProxyNode(QGraphicsProxyWidget* newProxyNode)
{
    proxyNode = newProxyNode;
}

void NodeBase::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton && hasMenu)
    {
        emit nodeContextMenuRequested(event->globalPosition().toPoint());
    }
    else
    {
        QWidget::mousePressEvent(event);
    }
}

void NodeBase::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(Qt::darkGray)); // visible color of background
    painter.setPen(Qt::transparent); // thin border color

    // Change border radius
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter.drawRoundedRect(rect, 20, 20);

    QWidget::paintEvent(event);
}

void NodeBase::inputsChanged()
{
    if (inputs == nullptr)
        return;
    updateLayout();
}

void NodeBase::outputsChanged()
{
    emit transferOutputsRequested(outputs);
}

void NodeBase::saveComponents(YAML::Emitter* out)
{
    if (!((OutputConnector*) outputConnectors->at(0)->widget())->getConnectorLineConnected())
        return;
    *out << YAML::Key << "components";
    if (outputConnectors->size() > 1 && ((OutputConnector*) outputConnectors->at(1)->widget())->getConnectorLineConnected())
        *out << YAML::BeginSeq;
    for (int i = 0; i < outputConnectors->size(); i++)
    {
        *out << YAML::Value;
        ((OutputConnector*) outputConnectors->at(i)->widget())->saveComponent(out);
    }
    if (outputConnectors->size() > 1 && ((OutputConnector*) outputConnectors->at(1)->widget())->getConnectorLineConnected())
        *out << YAML::EndSeq;
}

void NodeBase::createLayout(bool hasTitleSeparatorLine, bool hasAddButton, bool hasComponentsLayout)
{
    QVBoxLayout* globalLayout = new QVBoxLayout(this);
    addTitleLayout(globalLayout, hasTitleSeparatorLine);
    addDimensionLayout(globalLayout, hasAddButton);
    if (hasComponentsLayout)
        addComponentsLayout(globalLayout);
    setLayout(globalLayout);
}

void NodeBase::addTitleLayout(QVBoxLayout* globalLayout, bool hasTitleSeparatorLine)
{
    QVBoxLayout* titleLayout = new QVBoxLayout();
    titleLayout->setObjectName("titleLayout");

    QLabel* title = addLabel(titleLayout, windowTitle());
    title->setStyleSheet("QLabel { background-color : rgb(70,70,70);}");
    title->setFixedHeight(30);

    // add line to separate title from the rest
    if (hasTitleSeparatorLine)
        addSeparatorLineInLayout(titleLayout);

    globalLayout->addLayout(titleLayout);
}

void NodeBase::addDimensionLayout(QVBoxLayout* globalLayout, bool hasAddButton)
{
    // add an empty layout that will contain the dimensions
    QVBoxLayout* dimensionsLayout = new QVBoxLayout();
    dimensionsLayout->setObjectName("dimensionsLayout");
    if (outputs != nullptr)
    {
        for (int i = 0; i < outputs->size(); i++)
        {
            addNewDimensionsLayoutRow(dimensionsLayout, i);
        }
    }
    globalLayout->addLayout(dimensionsLayout);

    if (!hasAddButton)
        return;

    // add button to add dimension
    QPushButton* addButton = new QPushButton("+");
    connect(addButton, SIGNAL(clicked(bool)), this, SLOT(addDimensionsLayoutRowRequested(bool)));

    globalLayout->addWidget(addButton);
}

void NodeBase::addComponentsLayout(QVBoxLayout* globalLayout)
{
    // add line to separate components from the rest
    addSeparatorLineInLayout(globalLayout);

    QLabel* components = addLabel(globalLayout, "Components");
    components->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

    // add buttons
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QPushButton* addButton = new QPushButton("Add");
    QPushButton* deleteButton = new QPushButton("Delete");

    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(deleteButton);

    // connect buttons
    connect(addButton, SIGNAL(clicked(bool)), this, SLOT(addOutputConnectorButtonClicked(bool)));
    connect(deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteOutputConnectorButtonClicked(bool)));

    globalLayout->addLayout(buttonsLayout);
}

void NodeBase::addRemoveButton(QLayout* layout, int index)
{
    // add button to remove dimension
    QPushButton* removeButton = new QPushButton("-");
    removeButton->setObjectName(QString::number(index));
    connect(removeButton, SIGNAL(clicked(bool)), this, SLOT(removeDimensionsLayoutRowRequested(bool)));
    layout->addWidget(removeButton);
}

void NodeBase::clearLayout(QLayout* layout, bool deleteWidgets)
{
    while (QLayoutItem* item = layout->takeAt(0))
    {
        if (deleteWidgets)
        {
            if (QWidget* widget = item->widget())
                widget->deleteLater();
        }
        if (QLayout* childLayout = item->layout())
            clearLayout(childLayout, deleteWidgets);
        delete item;
    }
}

void NodeBase::deleteOutputConnector()
{
    if (outputConnectors->size() > 1)
    {
        emit deleteOutputConnectorRequested(outputConnectors->takeLast());
        performResize();
    }
}

void NodeBase::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    // needs to be implemented in subclasses
    Q_UNUSED(dimensionsLayout);
    Q_UNUSED(index);
}

void NodeBase::removeLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    QObjectList children = dimensionsLayout->children();
    QLayout* layoutToRemove = (QLayout*) children.at(index);
    clearLayout(layoutToRemove, true);
    dimensionsLayout->removeItem(layoutToRemove);
    delete layoutToRemove;
}

void NodeBase::addSeparatorLineInLayout(QLayout* layout)
{
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(line);
}

QLabel* NodeBase::addLabel(QLayout* layout, QString text)
{
    QLabel* label = new QLabel(text);
    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    layout->addWidget(label);

    return label;
}

QDoubleSpinBox* NodeBase::addDoubleSpinBox(QLayout* layout)
{
    QDoubleSpinBox* valueField = new QDoubleSpinBox();
    valueField->setButtonSymbols(QAbstractSpinBox::NoButtons);
    valueField->setRange(-DBL_MAX, DBL_MAX);
    valueField->setDecimals(3);
    valueField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    valueField->setMaximumWidth(100);
    valueField->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(valueField);

    return valueField;
}

void NodeBase::updateLayout()
{
    // do nothing
}

QPushButton* NodeBase::getRemoveButtonAtIndex(QVBoxLayout* dimensionsLayout, int index)
{
    QObjectList rows = dimensionsLayout->children();
    return (QPushButton*) ((QLayout*) rows.at(index))->itemAt(removeButtonIndex)->widget();
}

void NodeBase::saveNodeContent(YAML::Emitter* out)
{
    *out << YAML::LocalTag(localTag.toStdString());
    *out << YAML::BeginMap;
    saveValues(out);
    saveComponents(out);
    *out << YAML::EndMap;
}

void NodeBase::saveValues(YAML::Emitter* out)
{
    Q_UNUSED(out);
    return;
}

void NodeBase::addDimensionsLayoutRowRequested(bool clicked)
{
    Q_UNUSED(clicked);
    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    addNewDimensionsLayoutRow(dimensionsLayout, dimensionsLayout->children().size());
}

void NodeBase::removeDimensionsLayoutRowRequested(bool clicked)
{
    Q_UNUSED(clicked);
    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    QPushButton* removeButton = qobject_cast<QPushButton*>(sender());
    int index = removeButton->objectName().toInt();
    removeLayoutRow(dimensionsLayout, index);
    if (dimensionLineEditIndex != -1)
    {
        outputs->removeAt(index);
        emit transferOutputsRequested(outputs);
    }
    if (index != dimensionsLayout->children().size())
    {
        if (removeButtonIndex != -1)
            renameNextRemoveButtons(dimensionsLayout, index);
        if (dimensionLineEditIndex != -1)
            renameNextDimensionLineEdit(dimensionsLayout, index);
    }
    performResize();
}

void NodeBase::dimensionNameChanged(QString newOutput)
{
    QLineEdit* dimension = qobject_cast<QLineEdit*>(sender());
    int index = dimension->objectName().toInt();
    outputs->insert(index, newOutput);
    outputs->removeAt(index + 1);
    emit transferOutputsRequested(outputs);
}

void NodeBase::addOutputConnectorButtonClicked(bool clicked)
{
    Q_UNUSED(clicked);
    emit addOutputConnectorRequested(getProxyNode());
}

void NodeBase::deleteOutputConnectorButtonClicked(bool clicked)
{
    Q_UNUSED(clicked);
    deleteOutputConnector();
}

void NodeBase::renameNextRemoveButtons(QVBoxLayout* dimensionsLayout, int index)
{
    while (index < dimensionsLayout->children().size())
    {
        QPushButton* removeButtonAtIndex = getRemoveButtonAtIndex(dimensionsLayout, index);
        if (removeButtonAtIndex == nullptr)
            return;
        removeButtonAtIndex->setObjectName(QString::number(index));
        index++;
    }
}

QLineEdit* NodeBase::getDimensionLineEditAtIndex(QVBoxLayout* dimensionsLayout, int index)
{
    QObjectList rows = dimensionsLayout->children();
    return (QLineEdit*) ((QLayout*) rows.at(index))->itemAt(dimensionLineEditIndex)->widget();
}

void NodeBase::renameNextDimensionLineEdit(QVBoxLayout* dimensionsLayout, int index)
{
    while (index < dimensionsLayout->children().size())
    {
        QLineEdit* dimensionLineEditAtIndex = getDimensionLineEditAtIndex(dimensionsLayout, index);
        if (dimensionLineEditAtIndex == nullptr)
            return;
        dimensionLineEditAtIndex->setPlaceholderText("Dimension " + QString::number(index));
        index++;
    }
}

void NodeBase::transferOutputsReceived(QSharedPointer<QStringList> newInputs)
{
    if (this->inputs != newInputs)
        this->inputs = newInputs;
    inputsChanged();
}

void NodeBase::save(YAML::Emitter* out)
{
    saveNodeContent(out);
}

bool NodeBase::event(QEvent* event)
{
    if (event->type() == QEvent::Resize)
        performResize();
    return QWidget::event(event);
}
