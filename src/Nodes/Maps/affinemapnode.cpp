#include "affinemapnode.h"
#include "src/helpers.h"
#include <QPushButton>
#include <QLineEdit>

AffineMapNode::AffineMapNode(QStringList* outputs)
{
    typeOfNode = AFFINEMAPNODE;
    this->outputs = outputs;

    setWindowTitle("Affine Map");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

AffineMapNode::AffineMapNode(QStringList* outputs, QList<double>* values) : AffineMapNode(outputs)
{
    if (values == nullptr)
        return;

    if (outputs->size() != values->size())
    {
        qDebug() << "ERROR: constant map constructor: size of outputs != size of values";
        return;
    }

    QObjectList outputLayouts = this->layout()->findChild<QVBoxLayout*>("outputsLayout")->children();
    foreach (QObject* outputLayout, outputLayouts)
    {
        /// TODO
        UNUSED(outputLayout);
    }
    delete values;
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
    dimension->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
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

void AffineMapNode::removeDimensionsLayoutLastRow(QVBoxLayout* dimensionsLayout, int index)
{
    if (index < 0)
        return;
    UNUSED(dimensionsLayout);
    QObjectList children = dimensionsLayout->children();
    QHBoxLayout* layoutToRemove = (QHBoxLayout*) dimensionsLayout->itemAt(index);
    clearLayout(layoutToRemove, true);
    dimensionsLayout->removeItem(layoutToRemove);
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
    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    removeDimensionsLayoutLastRow(dimensionsLayout, dimensionsLayout->children().size() - 2);
    UNUSED(clicked);
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
