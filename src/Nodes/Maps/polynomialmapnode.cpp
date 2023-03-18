#include "polynomialmapnode.h"
#include "src/Nodes/Extra/polynomialmatrixnode.h"
#include <QLabel>
#include <QSpinBox>

PolynomialMapNode::PolynomialMapNode(QSharedPointer<QStringList> inputs, QSharedPointer<QStringList> outputs) : NodeBase(inputs, outputs)
{
    typeOfNode = POLYNOMIALMAPNODE;
    localTag = "PolynomialMap";
    setWindowTitle("Polynomial Map");
    removeButtonIndex = 0;
    dimensionLineEditIndex = 1;
    createLayout();

    QVBoxLayout* degreeAndLineLayout = new QVBoxLayout();
    QHBoxLayout* degreeLayout = new QHBoxLayout();
    degreeLayout->setObjectName("degreeLayout");

    addLabel(degreeLayout, "degree");

    QSpinBox* degreeValue = new QSpinBox();
    degreeValue->setButtonSymbols(QAbstractSpinBox::NoButtons);
    degreeValue->setRange(0, 100);
    degreeValue->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    degreeValue->setMaximumWidth(50);
    degreeValue->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    connect(degreeValue, SIGNAL(valueChanged(int)), this, SLOT(degreeHasChanged(int)));
    degreeLayout->addWidget(degreeValue);
    degreeAndLineLayout->addLayout(degreeLayout);

    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    degreeAndLineLayout->addWidget(line);

    ((QVBoxLayout*) this->layout())->insertLayout(1, degreeAndLineLayout);

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

PolynomialMapNode::~PolynomialMapNode()
{
    while (mathOutputConnectors->size() > 0)
    {
        removeMathOfDimensionRow(mathOutputConnectors->size() - 1);
    }
}

void PolynomialMapNode::setValues(QMap<QString, QList<double>>* values)
{
    if (values == nullptr)
        return;

    if (outputs->size() != values->size())
        qDebug() << "outputs and values sizes are different ";

    if (outputs->size() > 0)
        degree = values->value(outputs->at(0)).size() - 1;

    // set degree value
    QHBoxLayout* degreeLayout = this->layout()->findChild<QHBoxLayout*>("degreeLayout");
    ((QSpinBox*) degreeLayout->itemAt(1)->widget())->setValue(degree);

    for (int i = 0; i < outputs->size(); i++)
    {
        addMathConnector(i);

        // get matrix and translation values
        QList<double> matrixValues = values->value(outputs->at(i));

        // set matrix values
        PolynomialMatrixNode* matrixNode = (PolynomialMatrixNode*) this->polynomialMatrixProxies.at(i)->widget();
        matrixNode->setValues(inputs, &matrixValues);
    }

    delete values;
}

void PolynomialMapNode::addPolynomialMatrixProxy(QGraphicsProxyWidget* newPolynomialMatrixProxy)
{
    polynomialMatrixProxies.append(newPolynomialMatrixProxy);
}

void PolynomialMapNode::addMathConnector(int index)
{
    QPointF posMatrix(sizeHint().width() - 7, 110 + index * 46);
    emit addMathOutputConnectorRequested(proxyNode, posMatrix);
}

void PolynomialMapNode::removeMathOfDimensionRow(int index)
{
    emit deleteOutputConnectorRequested(mathOutputConnectors->takeAt(index));
    emit deleteNodeRequested(polynomialMatrixProxies.takeAt(index));
}

void PolynomialMapNode::degreeHasChanged(int newDegree)
{
    if (inputs == nullptr || inputs->size() == 0)
        return;

    foreach (QGraphicsProxyWidget* polynomialMatrixProxy, polynomialMatrixProxies)
    {
        ((PolynomialMatrixNode*) polynomialMatrixProxy->widget())->setDegree(newDegree);
    }

    setDegree(newDegree);
}

void PolynomialMapNode::performResize()
{
    NodeBase::performResize();
    for (int i = 0; i < mathOutputConnectors->size(); i++)
    {
        int x = sizeHint().width() - 7;
        int y = 110 + i * 46;
        mathOutputConnectors->at(i)->setPos(x, y);

    }
}

void PolynomialMapNode::clearNodes()
{
    while (mathOutputConnectors->size() > 0)
    {
        removeMathOfDimensionRow(mathOutputConnectors->size() - 1);
    }
}

int PolynomialMapNode::getDegree() const
{
    return degree;
}

void PolynomialMapNode::setDegree(int newDegree)
{
    degree = newDegree;
}

void PolynomialMapNode::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    QHBoxLayout* row = new QHBoxLayout();

    // add button to remove output
    addRemoveButton(row, index);

    QLineEdit* dimension = new QLineEdit();
    dimension->setObjectName(QString::number(index));
    dimension->setPlaceholderText("Dimension " + QString::number(index));
    if (outputs->size() > index)
        dimension->setText(outputs->at(index));
    else
        outputs->append("");
    dimension->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dimension->setFixedSize(100, 40);
    connect(dimension, SIGNAL(textChanged(QString)), this, SLOT(dimensionNameChanged(QString)));
    row->addWidget(dimension);

    QLabel* matrix = new QLabel("matrix");
    matrix->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    matrix->setAlignment(Qt::AlignRight);
    row->addWidget(matrix);

    dimensionsLayout->insertLayout(index, row);
}

void PolynomialMapNode::addDimensionsLayoutRowRequested(bool clicked)
{
    NodeBase::addDimensionsLayoutRowRequested(clicked);
    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    addMathConnector(dimensionsLayout->children().size() - 1);
    updateLayout();
}

void PolynomialMapNode::removeDimensionsLayoutRowRequested(bool clicked)
{
    QPushButton* removeButton = qobject_cast<QPushButton*>(sender());
    removeMathOfDimensionRow(removeButton->objectName().toInt());
    NodeBase::removeDimensionsLayoutRowRequested(clicked);
}

void PolynomialMapNode::updateLayout()
{
    foreach (QGraphicsProxyWidget* mathOutputConnector, (*mathOutputConnectors))
    {
        emit ((OutputConnector*) mathOutputConnector->widget())->transferOutputsRequested(inputs);
    }
}

void PolynomialMapNode::saveValues(YAML::Emitter* out)
{
    *out << YAML::Key << "map";
    *out << YAML::BeginMap;

    QStringList dimensions(*outputs);
    dimensions.sort();

    for (int i = 0; i < dimensions.size(); i++)
    {
        int index = outputs->indexOf(dimensions[i]);
        *out << YAML::Key << dimensions.at(i).toStdString();
        *out << YAML::Value;
        ((OutputConnector*) mathOutputConnectors->at(index)->widget())->saveComponent(out);
    }

    *out << YAML::EndMap;
}
