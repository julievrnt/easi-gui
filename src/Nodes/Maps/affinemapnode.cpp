#include "affinemapnode.h"
#include "src/Nodes/Extra/affinematrixnode.h"
#include "src/Nodes/Extra/translationnode.h"
#include <QPushButton>
#include <QLineEdit>

AffineMapNode::AffineMapNode(QSharedPointer<QStringList> inputs, QSharedPointer<QStringList> outputs) : NodeBase(inputs, outputs)
{
    typeOfNode = AFFINEMAPNODE;
    localTag = "AffineMap";
    setWindowTitle("Affine Map");
    removeButtonIndex = 0;
    dimensionLineEditIndex = 1;
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

AffineMapNode::~AffineMapNode()
{
    // affineMatrixProxies deleted in clearNodes()
    // translationProxies deleted in clearNodes()
}

void AffineMapNode::setValues(QMap<QString, QList<double> >* values)
{
    if (values == nullptr)
        return;

    if (outputs->size() != values->size())
        qDebug() << "outputs and values sizes are different";

    for (int i = 0; i < outputs->size(); i++)
    {
        addMathsConnectors(i);

        // get matrix and translation values
        QList<double> matrixAndTranslationValues = values->value(outputs->at(i));
        double translationValue = matrixAndTranslationValues.takeLast();

        // set matrix values
        AffineMatrixNode* matrixNode = static_cast<AffineMatrixNode*>(this->affineMatrixProxies.at(i)->widget());
        matrixNode->setValues(inputs, &matrixAndTranslationValues);

        // set translation values
        TranslationNode* translationNode = static_cast<TranslationNode*>(this->translationProxies.at(i)->widget());
        translationNode->setValue(translationValue);
    }

    delete values;
}

void AffineMapNode::addAffineMatrixProxy(QGraphicsProxyWidget* newAffineMatrixProxy)
{
    affineMatrixProxies.append(newAffineMatrixProxy);
}

void AffineMapNode::addTranslationProxy(QGraphicsProxyWidget* newTranslationProxy)
{
    translationProxies.append(newTranslationProxy);
}

void AffineMapNode::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
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

    QVBoxLayout* transformLayout = new QVBoxLayout();
    QLabel* matrix = new QLabel("matrix");
    matrix->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    matrix->setAlignment(Qt::AlignRight);
    transformLayout->addWidget(matrix);
    // add connector to this line
    QLabel* translation = new QLabel("translation");
    translation->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    translation->setAlignment(Qt::AlignRight);
    transformLayout->addWidget(translation);
    // add connector to this line

    row->addLayout(transformLayout);
    dimensionsLayout->insertLayout(index, row);
}

void AffineMapNode::addMathsConnectors(int index)
{
    QPointF posMatrix(sizeHint().width() - 7, 58 + index * 46);
    QPointF posTranslation(sizeHint().width() - 7, 81 + index * 46);
    emit addMathOutputConnectorRequested(proxyNode, posMatrix);
    emit addMathOutputConnectorRequested(proxyNode, posTranslation);
}

void AffineMapNode::removeMathsOfDimensionRow(int index)
{
    emit deleteOutputConnectorRequested(mathOutputConnectors->takeAt(index * 2));
    emit deleteOutputConnectorRequested(mathOutputConnectors->takeAt(index * 2));

    emit deleteNodeRequested(affineMatrixProxies.takeAt(index));
    emit deleteNodeRequested(translationProxies.takeAt(index));
}

void AffineMapNode::performResize()
{
    NodeBase::performResize();
    for (int i = 0; i < mathOutputConnectors->size(); i++)
    {
        int x = sizeHint().width() - 7;
        int y = i / 2 * 46;
        if (i % 2 == 0)
            y += 58;
        else
            y += 81;
        mathOutputConnectors->at(i)->setPos(x, y);
    }
}

void AffineMapNode::clearNodes()
{
    while (mathOutputConnectors->size() > 0)
    {
        removeMathsOfDimensionRow((mathOutputConnectors->size() - 1) / 2);
    }
}

void AffineMapNode::addDimensionsLayoutRowRequested(bool clicked)
{
    NodeBase::addDimensionsLayoutRowRequested(clicked);
    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    addMathsConnectors(dimensionsLayout->children().size() - 1);
    updateLayout();
}

void AffineMapNode::removeDimensionsLayoutRowRequested(bool clicked)
{
    QPushButton* removeButton = qobject_cast<QPushButton*>(sender());
    removeMathsOfDimensionRow(removeButton->objectName().toInt());
    NodeBase::removeDimensionsLayoutRowRequested(clicked);
}

void AffineMapNode::updateLayout()
{
    foreach (QGraphicsProxyWidget* mathOutputConnector, (*mathOutputConnectors))
    {
        emit static_cast<OutputConnector*>(mathOutputConnector->widget())->transferOutputsRequested(inputs);
    }
}

void AffineMapNode::saveValues(YAML::Emitter* out)
{
    QStringList dimensions(*outputs);
    dimensions.sort();

    *out << YAML::Key << "matrix";
    *out << YAML::BeginMap ;
    for (int i = 0; i < dimensions.size(); i++)
    {
        int index = outputs->indexOf(dimensions[i]);
        *out << YAML::Key << dimensions.at(i).toStdString();
        static_cast<OutputConnector*>(mathOutputConnectors->at(index * 2)->widget())->saveComponent(out);
    }
    *out << YAML::EndMap;

    *out << YAML::Key << "translation";
    *out << YAML::BeginMap ;
    for (int i = 0; i < dimensions.size(); i++)
    {
        int index = outputs->indexOf(dimensions[i]);
        *out << YAML::Key << dimensions.at(i).toStdString();
        *out << YAML::Value;
        static_cast<OutputConnector*>(mathOutputConnectors->at(index * 2 + 1)->widget())->saveComponent(out);
    }
    *out << YAML::EndMap;

}
