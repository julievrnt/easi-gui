#include "affinemapnode.h"
#include "src/helpers.h"
#include <QPushButton>
#include <QLineEdit>

AffineMapNode::AffineMapNode(QStringList* outputs) : NodeBase(outputs)
{
    typeOfNode = AFFINEMAPNODE;
    setWindowTitle("Affine Map");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

AffineMapNode::AffineMapNode(QStringList* outputs, QList<double>* values) : AffineMapNode(outputs)
{
    if (values == nullptr)
        return;

    QObjectList outputLayouts = this->layout()->findChild<QVBoxLayout*>("outputsLayout")->children();
    foreach (QObject* outputLayout, outputLayouts)
    {
        /// TODO
        UNUSED(outputLayout);
    }
    delete values;
}

AffineMapNode::~AffineMapNode()
{
    emit deleteNodeRequested(matrixProxy);
    emit deleteNodeRequested(translationProxy);
}

QList<QGraphicsProxyWidget*>* AffineMapNode::getMathOutputConnectors() const
{
    return mathOutputConnectors;
}

void AffineMapNode::setMatrixProxy(QGraphicsProxyWidget* newMatrixProxy)
{
    matrixProxy = newMatrixProxy;
}

void AffineMapNode::setTranslationProxy(QGraphicsProxyWidget* newTranslationProxy)
{
    translationProxy = newTranslationProxy;
}

void AffineMapNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout(this);
    addTitleLayout(globalLayout);

    // add a layout containing only one dimension
    QVBoxLayout* dimensionsLayout = new QVBoxLayout();
    dimensionsLayout->setObjectName("dimensionsLayout");
    addNewDimensionsLayoutRow(dimensionsLayout, 0);

    // add buttons to add or delete dimension row
    QHBoxLayout* buttonsLayout = new QHBoxLayout();

    QPushButton* addDimensionLayoutButton = new QPushButton();
    addDimensionLayoutButton->setText("Add New Dimension");
    connect(addDimensionLayoutButton, SIGNAL(clicked(bool)), this, SLOT(addDimensionsLayoutRowRequested(bool)));
    buttonsLayout->addWidget(addDimensionLayoutButton);

    QPushButton* removeDimensionLayoutButton = new QPushButton();
    removeDimensionLayoutButton->setText("Remove Last Dimension");
    connect(removeDimensionLayoutButton, SIGNAL(clicked(bool)), this, SLOT(removeDimensionsLayoutRowRequested(bool)));
    buttonsLayout->addWidget(removeDimensionLayoutButton);

    dimensionsLayout->addLayout(buttonsLayout);
    globalLayout->addLayout(dimensionsLayout);

    addComponentsLayout(globalLayout);
    this->setLayout(globalLayout);
}

void AffineMapNode::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    UNUSED(dimensionsLayout);
    QHBoxLayout* row = new QHBoxLayout();

    QLineEdit* dimension = new QLineEdit();
    dimension->setPlaceholderText("Dimension " + QString::number(index));
    dimension->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    dimension->setFixedHeight(40);
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
    QPointF posMatrix(sizeHint().width() - 7, 58 + index * 46);
    QPointF posTranslation(sizeHint().width() - 7, 81 + index * 46);
    emit addMathOutputConnectorRequested(proxyNode, posMatrix);
    emit addMathOutputConnectorRequested(proxyNode, posTranslation);
}

void AffineMapNode::removeDimensionsLayoutLastRow(QVBoxLayout* dimensionsLayout, int index)
{
    if (index < 0)
        return;
    QHBoxLayout* layoutToRemove = (QHBoxLayout*) dimensionsLayout->itemAt(index);
    clearLayout(layoutToRemove, true);
    dimensionsLayout->removeItem(layoutToRemove);
    emit deleteNodeRequested(matrixProxy);
    emit deleteNodeRequested(translationProxy);
    delete layoutToRemove;
}

void AffineMapNode::updateLayout()
{
    /// TODO
    // change matrix & translation part !
}

void AffineMapNode::addDimensionsLayoutRowRequested(bool clicked)
{
    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    addNewDimensionsLayoutRow(dimensionsLayout, dimensionsLayout->children().size() - 1);
    UNUSED(clicked);
}

void AffineMapNode::removeDimensionsLayoutRowRequested(bool clicked)
{
    UNUSED(clicked);
    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    if (mathOutputConnectors->size() > 2)
    {
        removeDimensionsLayoutLastRow(dimensionsLayout, dimensionsLayout->children().size() - 2);
        emit deleteOutputConnectorRequested(mathOutputConnectors->takeLast());
        emit deleteOutputConnectorRequested(mathOutputConnectors->takeLast());
        performResize();
    }
}

void AffineMapNode::saveNodeContent(YAML::Emitter* out)
{
    /// TODO
    UNUSED(out);
}

void AffineMapNode::saveValues(YAML::Emitter* out)
{
    /// TODO
    UNUSED(out);
}
