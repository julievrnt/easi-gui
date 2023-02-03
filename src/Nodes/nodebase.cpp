#include "nodebase.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include "../helpers.h"
#include "../Connectors/outputs/outputconnector.h"

NodeBase::NodeBase(QStringList* inputs, QStringList* outputs, QWidget* parent)
    : QWidget{parent}
{
    typeOfNode = NODE;

    this->inputs = inputs;
    if (this->inputs == nullptr)
        this->inputs = new QStringList();

    this->outputs = outputs;
    if (this->outputs == nullptr)
        this->outputs = new QStringList();

    this->setWindowTitle("Node Base");
    outputConnectors = new QList<QGraphicsProxyWidget*>();
    mathOutputConnectors = new QList<QGraphicsProxyWidget*>();
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("Node");
}

int NodeBase::getTypeOfNode()
{
    return typeOfNode;
}

QStringList* NodeBase::getOutputs() const
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

void NodeBase::addMathOutputConnector(QGraphicsProxyWidget* newMathOutputConnector)
{
    mathOutputConnectors->append(newMathOutputConnector);
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

QStringList* NodeBase::getInputs() const
{
    return inputs;
}

QGraphicsProxyWidget* NodeBase::getProxyNode() const
{
    return proxyNode;
}

void NodeBase::setProxyNode(QGraphicsProxyWidget* newProxyNode)
{
    proxyNode = newProxyNode;
}

void NodeBase::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
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

void NodeBase::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout(this);
    if (typeOfNode == ANYNODE || typeOfNode == IDENTITYMAPNODE)
        addTitleLayout(globalLayout, false);
    else
        addTitleLayout(globalLayout, true);
    addDimensionLayout(globalLayout);
    if (typeOfNode != ROOTNODE)
        addComponentsLayout(globalLayout);
    this->setLayout(globalLayout);
}

void NodeBase::addTitleLayout(QVBoxLayout* globalLayout, bool addSeparatorLine)
{
    QVBoxLayout* titleLayout = new QVBoxLayout();
    titleLayout->setObjectName("titleLayout");

    QLabel* title = new QLabel(this);
    title->setText(this->windowTitle());
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("QLabel { background-color : rgb(70,70,70);}");
    title->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    title->setFixedHeight(30);
    titleLayout->addWidget(title);

    if (!addSeparatorLine)
        return;

    // add line to separate title from the rest
    QFrame* lineTitel = new QFrame(this);
    lineTitel->setFrameShape(QFrame::HLine);
    lineTitel->setFrameShadow(QFrame::Sunken);
    lineTitel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    titleLayout->addWidget(lineTitel);

    globalLayout->addLayout(titleLayout);
}

void NodeBase::addDimensionLayout(QVBoxLayout* globalLayout)
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

    switch (typeOfNode)
    {
        case ANYNODE:
            return;
        case AXISALIGNEDCUBOIDALDOMAINFILTERNODE:
            return;
    case SPHERICALDOMAINFILTERNODE:
        return;
        case IDENTITYMAPNODE:
            return;
        default:
            break;
    }


    // add button to add dimension
    QPushButton* addButton = new QPushButton();
    addButton->setText("+");
    connect(addButton, SIGNAL(clicked(bool)), this, SLOT(addDimensionsLayoutRowRequested(bool)));
    globalLayout->addWidget(addButton);
}

void NodeBase::addComponentsLayout(QVBoxLayout* globalLayout)
{
    // add line to separate components from the rest
    QFrame* lineComponents = new QFrame(this);
    lineComponents->setFrameShape(QFrame::HLine);
    lineComponents->setFrameShadow(QFrame::Sunken);
    lineComponents->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    globalLayout->addWidget(lineComponents);

    QLabel* componentsLabel = new QLabel();
    componentsLabel->setText("Components");
    componentsLabel->setAlignment(Qt::AlignCenter);
    componentsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    globalLayout->addWidget(componentsLabel);

    // add buttons
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QPushButton* addButton = new QPushButton();
    addButton->setText("Add");
    QPushButton* deleteButton = new QPushButton();
    deleteButton->setText("Delete");

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
    QPushButton* removeButton = new QPushButton();
    removeButton->setObjectName(QString::number(index));
    removeButton->setText("-");
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

void NodeBase::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    // needs to be implemented in subclasses
    UNUSED(dimensionsLayout);
    UNUSED(index);
}

void NodeBase::removeLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    QObjectList children = dimensionsLayout->children();
    QLayout* layoutToRemove = (QLayout*) children.at(index);
    clearLayout(layoutToRemove, true);
    dimensionsLayout->removeItem(layoutToRemove);
    delete layoutToRemove;
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
    UNUSED(out);
    return;
}

void NodeBase::addDimensionsLayoutRowRequested(bool clicked)
{
    UNUSED(clicked);
    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    addNewDimensionsLayoutRow(dimensionsLayout, dimensionsLayout->children().size());
}

void NodeBase::removeDimensionsLayoutRowRequested(bool clicked)
{
    UNUSED(clicked);
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

void NodeBase::addOutputConnectorButtonClicked(bool clicked)
{
    UNUSED(clicked);
    emit addOutputConnectorRequested(this->getProxyNode());
}

void NodeBase::deleteOutputConnectorButtonClicked(bool clicked)
{
    UNUSED(clicked);
    if (outputConnectors->size() > 1)
    {
        emit deleteOutputConnectorRequested(outputConnectors->takeLast());
        performResize();
    }
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

void NodeBase::transferOutputsReceived(QStringList* newInputs)
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
