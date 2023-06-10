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


/// ===========================================================================
/// =========================== GETTERS AND SETTERS ===========================
/// ===========================================================================

int NodeBase::getTypeOfNode()
{
    return typeOfNode;
}

/**
 * @brief Returns the proxy of the node
 * @return
 */
QGraphicsProxyWidget* NodeBase::getProxyNode() const
{
    return proxyNode;
}

/**
 * @brief Saves a pointer to the proxy of the node
 * @param newProxyNode  proxy of the node
 */
void NodeBase::setProxyNode(QGraphicsProxyWidget* newProxyNode)
{
    proxyNode = newProxyNode;
}

/**
 * @brief Returns the list of the names of the inputs
 * @return
 */
QSharedPointer<QStringList> NodeBase::getInputs() const
{
    return inputs;
}

/**
 * @brief Returns the list of the names of the outputs
 * @return
 */
QSharedPointer<QStringList> NodeBase::getOutputs() const
{
    return outputs;
}

/**
 * @brief If the node can be deleted, a menu will be displayed when right clicked on the node. Extra nodes and the root node cannot be deleted.
 * @return true if the node can be deleted and thus has a menu
 */
bool NodeBase::getHasMenu() const
{
    return hasMenu;
}

/**
 * @brief Returns the proxy of the input connector of the node
 * @return
 */
QGraphicsProxyWidget* NodeBase::getInputConnector() const
{
    return inputConnector;
}

/**
 * @brief Saves the proxy of the input connector of the node
 * @param newInputConnector proxy of the input connector
 */
void NodeBase::setInputConnector(QGraphicsProxyWidget* newInputConnector)
{
    inputConnector = newInputConnector;
}

/**
 * @brief Returns the list of the proxies of the basic output connectors of the node (green connector)
 * @return
 */
QList<QGraphicsProxyWidget*>* NodeBase::getOutputConnectors() const
{
    return outputConnectors;
}

/**
 * @brief Returns the proxy of the first non-connected connector of the list. If none is available, a new one is created and returned.
 * @return
 */
OutputConnector* NodeBase::getFirstAvailableOutputConnector()
{
    foreach (QGraphicsProxyWidget* proxy, *outputConnectors)
    {
        OutputConnector* outputConnector = static_cast<OutputConnector*>(proxy->widget());
        if (outputConnector->isFree())
            return outputConnector;
    }
    return (emit addOutputConnectorRequested(this->getProxyNode()));
}

/**
 * @brief Adds the proxy of a new output connector to the list
 * @param newOutputConnector
 */
void NodeBase::addOutputConnector(QGraphicsProxyWidget* newOutputConnector)
{
    outputConnectors->append(newOutputConnector);
}

/**
 * @brief Returns the list of the output connectors used for math purposes (violet connectors)
 * @return
 */
QList<QGraphicsProxyWidget*>* NodeBase::getMathOutputConnectors() const
{
    return mathOutputConnectors;
}

/**
 * @brief Adds the proxy of a new math output connector to the list
 * @param newMathOutputConnector
 */
void NodeBase::addMathOutputConnector(QGraphicsProxyWidget* newMathOutputConnector)
{
    mathOutputConnectors->append(newMathOutputConnector);
}

/**
 * @brief Returns the list of the output connectors used for functions purposes (blue connectors)
 * @return
 */
QList<QGraphicsProxyWidget*>* NodeBase::getFunctionOutputConnectors() const
{
    return functionOutputConnectors;
}

/**
 * @brief Adds the proxy of a new function output connector to the list
 * @param newFunctionOutputConnector
 */
void NodeBase::addFunctionOutputConnector(QGraphicsProxyWidget* newFunctionOutputConnector)
{
    functionOutputConnectors->append(newFunctionOutputConnector);
}

/**
 * @brief Returns the output connector used for eval purposes (red connectors)
 * @return
 */
OutputConnector* NodeBase::getOutputConnectorModel() const
{
    if (outputConnectorModel == nullptr)
        return nullptr;
    return static_cast<OutputConnector*>(outputConnectorModel->widget());
}

/**
 * @brief Saves the proxy of the model output connector
 * @param newOutputConnectorModel
 */
void NodeBase::setOutputConnectorModel(QGraphicsProxyWidget* newOutputConnectorModel)
{
    outputConnectorModel = newOutputConnectorModel;
}


/// ===========================================================================

/**
 * @brief Resises the node and its parent when the content of the node has changed (e.g. new rows or new output connectors added)
 */
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

/**
 * @brief Deletes all extra nodes connected to the node. It needs to be implemented in subclasses (e.g. FunctionMapNode)
 */
void NodeBase::clearNodes()
{
    // Needs to be implemented in subclasses
}


/// ===========================================================================
/// ========================= INHERITED FROM QWIDGET ==========================
/// ===========================================================================

bool NodeBase::event(QEvent* event)
{
    if (event->type() == QEvent::Resize)
        performResize();
    return QWidget::event(event);
}

/**
 * @brief Opens menu by right clicking on nodes that can have a menu
 * @param event
 */
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


/// ===========================================================================

/**
 * @brief Updates the layout when the inputs have changed
 */
void NodeBase::inputsChanged()
{
    if (inputs == nullptr)
        return;
    updateLayout();
}

/**
 * @brief Emits a signal to inform the children that the ouputs have changed
 */
void NodeBase::outputsChanged()
{
    emit transferOutputsRequested(outputs);
}

/**
 * @brief Emit a signal received by WidgetsHandler to deletes the last output connector of the node
 * @param clicked
 */
void NodeBase::deleteOutputConnector()
{
    if (outputConnectors->size() > 1)
    {
        emit deleteOutputConnectorRequested(outputConnectors->takeLast());
        performResize();
    }
}

/// ===========================================================================
/// ========================== LAYOUTS FOR ALL NODES ==========================
/// ===========================================================================

/**
 * @brief Creates a basic layout with a title, the main part (called dimensionLayout) and the part where children can be added to the node
 * @param hasTitleSeparatorLine says if an horizontal line is added after the title (e.g. to separate the title from the main part)
 * @param hasAddButton says if the dimensionLayout needs a button to add new rows to its layout
 * @param hasComponentsLayout says if the nodes can have children
 */
void NodeBase::createLayout(bool hasTitleSeparatorLine, bool hasAddButton, bool hasComponentsLayout)
{
    QVBoxLayout* globalLayout = new QVBoxLayout(this);
    addTitleLayout(globalLayout, hasTitleSeparatorLine);
    addDimensionLayout(globalLayout, hasAddButton);
    if (hasComponentsLayout)
        addComponentsLayout(globalLayout);
    setLayout(globalLayout);
}

/**
 * @brief Adds a layout containing the name of the node to globalLayout. The name of the node is set in the constructor as the windowTitle
 * @param globalLayout layout to which the title layout will be added
 * @param hasTitleSeparatorLine if hasTitleSeparatorLine is true, an horizontal line is added after the title (e.g. to separate the title from the main part)
 */
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

/**
 * @brief Creates a vertical layout in which rows are added for each existing output and adds it to globalLayout
 * @param globalLayout layout to which the dimension layout will be added
 * @param hasAddButton if hasAddButton is true, a button is added to globalLayout. When this button is clicked, new rows are added to the dimensionLayout
 */
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

/**
 * @brief Creates a layout with add and remove buttons that will handle the creation and deletion of output connectors and add it to globalLayout. It is used for nodes that can have children
 * @param globalLayout layout to which the components layout will be added
 */
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

/**
 * @brief Inserts a row at index into dimensionsLayout. It needs to be implemented in subclasses of NodeBase.
 * @param dimensionsLayout
 * @param index
 */
void NodeBase::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    // needs to be implemented in subclasses
    Q_UNUSED(dimensionsLayout);
    Q_UNUSED(index);
}

/**
 * @brief Removes the row at index from dimensionsLayout
 * @param dimensionsLayout
 * @param index
 */
void NodeBase::removeLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    QObjectList children = dimensionsLayout->children();
    QLayout* layoutToRemove = static_cast<QLayout*>(children.at(index));
    clearLayout(layoutToRemove, true);
    dimensionsLayout->removeItem(layoutToRemove);
    delete layoutToRemove;
}

/**
 * @brief Adds an horizontal line to layout
 * @param layout layout to which the line is added
 */
void NodeBase::addSeparatorLineInLayout(QLayout* layout)
{
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(line);
}

/**
 * @brief Creates a remove button and connect it to the slot removeDimensionsLayoutRowRequested.
 * @param layout layout to which the button is added
 * @param index indicates the row on which the button is. This name is further used to know which row needs to be deleted.
 */
void NodeBase::addRemoveButton(QLayout* layout, int index)
{
    // add button to remove dimension
    QPushButton* removeButton = new QPushButton("-");
    removeButton->setObjectName(QString::number(index));
    connect(removeButton, SIGNAL(clicked(bool)), this, SLOT(removeDimensionsLayoutRowRequested(bool)));
    layout->addWidget(removeButton);
}

/**
 * @brief Adds a label to layout and returns it
 * @param layout layout to which the label is added
 * @param text  text that will be displayed by the label
 * @return the created label
 */
QLabel* NodeBase::addLabel(QLayout* layout, QString text)
{
    QLabel* label = new QLabel(text);
    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    layout->addWidget(label);

    return label;
}

/**
 * @brief Adds a double spin box to layout and returns it
 * @param layout layout to which the double spin box is added
 * @return the created double spin box
 */
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

/**
 * @brief Update the main part of the node (dimensionsLayout) if the inputs have changed. It needs to be implemented in the subclasses.
 */
void NodeBase::updateLayout()
{
    // needs to be implemented in the subclasses
}

/**
 * @brief Removes and deletes layout and its content
 * @param layout the layout that needs to be cleared
 * @param deleteWidgets when true, the widget of the items in the layout are deleted. deleteWidgets is true by default.
 */
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


/// ===========================================================================
/// ============================= SAVE FUNCTIONS ==============================
/// ===========================================================================

/**
 * @brief Adds the content of the node to the YAML emitter. Can be inherited by the subclasses (e.g. an extra node doesn't have a local tag).
 * @param out YAML emitter
 */
void NodeBase::saveNodeContent(YAML::Emitter* out)
{
    *out << YAML::LocalTag(localTag.toStdString());
    *out << YAML::BeginMap;
    saveValues(out);
    saveComponents(out);
    *out << YAML::EndMap;
}

/**
 * @brief Adds the content of the main part of the node to the YAML emitter. Needs to be inherited by the subclasses.
 * @param out   YAML emitter
 */
void NodeBase::saveValues(YAML::Emitter* out)
{
    // needs to be implemented in the subclasses
    Q_UNUSED(out);
    return;
}

/**
 * @brief Adds the content of the children of the node to the YAML emitter.
 * @param out   YAML emitter
 */
void NodeBase::saveComponents(YAML::Emitter* out)
{
    if (outputConnectors->size() == 0)
        return;
    if (!static_cast<OutputConnector*>(outputConnectors->at(0)->widget())->getConnectorLineConnected())
        return;
    *out << YAML::Key << "components";
    if (outputConnectors->size() > 1 && static_cast<OutputConnector*>(outputConnectors->at(1)->widget())->getConnectorLineConnected())
        *out << YAML::BeginSeq;
    for (int i = 0; i < outputConnectors->size(); i++)
    {
        *out << YAML::Value;
        static_cast<OutputConnector*>(outputConnectors->at(i)->widget())->saveComponent(out);
    }
    if (outputConnectors->size() > 1 && static_cast<OutputConnector*>(outputConnectors->at(1)->widget())->getConnectorLineConnected())
        *out << YAML::EndSeq;
}


/// ===========================================================================
/// ================================== SLOTS ==================================
/// ===========================================================================

/**
 * @brief Adds a row to dimensionsLayout. Can be inherited by subclasses when needed (e.g. PolynomialMapNode).
 * @param clicked
 */
void NodeBase::addDimensionsLayoutRowRequested(bool clicked)
{
    Q_UNUSED(clicked);
    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    addNewDimensionsLayoutRow(dimensionsLayout, dimensionsLayout->children().size());
}

/**
 * @brief Remove the row on which a removeButton has been clicked on from dimensionsLayout. It also renames the buttons and lines edits of the next row. Can be inherited by subclasses when needed (e.g. PolynomialMapNode).
 * @param clicked
 */
void NodeBase::removeDimensionsLayoutRowRequested(bool clicked)
{
    Q_UNUSED(clicked);
    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    QPushButton* removeButton = qobject_cast<QPushButton*>(sender());
    int index = removeButton->objectName().toInt();
    removeLayoutRow(dimensionsLayout, index);
    if (dimensionLineEditIndex != -1)
    {
        qDebug() << outputs->size() << " vs " << index;
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

/**
 * @brief Emit a signal received by WidgetsHandler to add an output connector to the scene.
 * @param clicked
 */
void NodeBase::addOutputConnectorButtonClicked(bool clicked)
{
    Q_UNUSED(clicked);
    emit addOutputConnectorRequested(getProxyNode());
}

/**
 * @brief Calls deleteOutputConnector() when a signal is received.
 * @param clicked
 */
void NodeBase::deleteOutputConnectorButtonClicked(bool clicked)
{
    Q_UNUSED(clicked);
    deleteOutputConnector();
}

/**
 * @brief Updates the value of an element of the outputs when the content of the line edit connected to this slot is changed. The index is given by the name of the line edit.
 * @param newOutput
 */
void NodeBase::dimensionNameChanged(QString newOutput)
{
    QLineEdit* dimension = qobject_cast<QLineEdit*>(sender());
    int index = dimension->objectName().toInt();
    outputs->replace(index, newOutput);
    emit transferOutputsRequested(outputs);
}

/**
 * @brief Returns the remove button of the index-th row of dimensionsLayout. The position of the button inside the row is given by the attribute removeButtonIndex
 * @param dimensionsLayout  layout that contains the remove button
 * @param index index of the row in which the remove button can be found
 * @return  the button
 */
QPushButton* NodeBase::getRemoveButtonAtIndex(QVBoxLayout* dimensionsLayout, int index)
{
    QObjectList rows = dimensionsLayout->children();
    return static_cast<QPushButton*>(static_cast<QLayout*>(rows.at(index))->itemAt(removeButtonIndex)->widget());
}

/**
 * @brief Renames the remove buttons on the rows starting from index-th row until the last row
 * @param dimensionsLayout  layout that contains the rows
 * @param index index of the first row on which the name of the remove button is updated.
 */
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

/**
 * @brief Returns the line edit of the index-th row of dimensionsLayout. The position of the button inside the row is given by the attribute dimensionLineEditIndex
 * @param dimensionsLayout  layout that contains the line edit
 * @param index index of the row in which the line edit can be found
 * @return  the line edit
 */
QLineEdit* NodeBase::getDimensionLineEditAtIndex(QVBoxLayout* dimensionsLayout, int index)
{
    QObjectList rows = dimensionsLayout->children();
    return static_cast<QLineEdit*>(static_cast<QLayout*>(rows.at(index))->itemAt(dimensionLineEditIndex)->widget());
}

/**
 * @brief Renames the line edits on the rows starting from the index-th row until the last row.
 * @param dimensionsLayout  layout that contains the rows
 * @param index index of the first row on which the name of the remove button is updated.
 */
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

/**
 * @brief Updates the attribute inputs if the pointer changed and calls inputsChanged()
 * @param newInputs
 */
void NodeBase::transferOutputsReceived(QSharedPointer<QStringList> newInputs)
{
    if (this->inputs != newInputs)
        this->inputs = newInputs;
    inputsChanged();
}

/**
 * @brief Calls saveNodeContent(YAML::Emitter* out) when a signal is received.
 * @param out   YAML emitter to which the content of the nodes needs to be added
 */
void NodeBase::save(YAML::Emitter* out)
{
    saveNodeContent(out);
}
