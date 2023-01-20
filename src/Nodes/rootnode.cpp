#include "rootnode.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "rootnodedialog.h"
#include "../helpers.h"
#include "../Connectors/outputs/outputconnector.h"

RootNode::RootNode(QStringList* outputs) : NodeBase(outputs)
{
    typeOfNode = ROOT;
    setWindowTitle("Outputs");
    outputConnector = nullptr;

    // add default outputs
    if (outputs == nullptr)
    {
        this->outputs = new QStringList();
        *this->outputs << "lambda" << "mu" << "rho";
    }
    else
        this->outputs = outputs;

    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

RootNode::~RootNode()
{
    delete outputs;
    delete outputConnectors;
}

void RootNode::updateOutputs()
{
    sortOutputs(0);
}

void RootNode::setOutputConnector(QGraphicsProxyWidget* newOutputConnector)
{
    outputConnector = newOutputConnector;
}

OutputConnector* RootNode::getFirstAvailableOutputConnector()
{
    return (OutputConnector*) outputConnector->widget();
}

void RootNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout();
    addTitleLayout(globalLayout);

    // create layout in order to modify it later
    QVBoxLayout* outputsLayout = new QVBoxLayout();
    outputsLayout->setObjectName("Outputs Layout");

    for (int i = 0; i < outputs->size(); i++)
    {
        QLabel* outputLabel = new QLabel();
        outputLabel->setText(outputs->at(i));
        outputLabel->setAlignment(Qt::AlignCenter);
        outputLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        outputsLayout->addWidget(outputLabel);
        // store label in list to easily remove it later
        outputLabels.append(outputLabel);
    }

    globalLayout->addLayout(outputsLayout);

    // modifyButton will open a dialog
    QPushButton* modifyButton = new QPushButton();
    modifyButton->setText("Modify Outputs");

    connect(modifyButton, SIGNAL(clicked(bool)), this, SLOT(modifyOutputsButtonClicked(bool)));

    globalLayout->addWidget(modifyButton);

    this->setLayout(globalLayout);
}

void RootNode::updateLayout()
{
    QVBoxLayout* outputsLayout = (QVBoxLayout*) layout()->findChild<QVBoxLayout*>("Outputs Layout");

    while (!outputLabels.isEmpty())
    {
        QLabel* outputLabelToRemove = outputLabels.takeLast();
        outputsLayout->removeWidget(outputLabelToRemove);
        delete outputLabelToRemove;
    }

    // add all new outputs in layout
    for (int i = 0; i < outputs->size(); i++)
    {
        QLabel* outputLabel = new QLabel();
        outputLabel->setText(outputs->at(i));
        outputLabel->setAlignment(Qt::AlignCenter);
        outputLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        outputsLayout->addWidget(outputLabel);
        // store label in list to easily remove it later
        outputLabels.append(outputLabel);
    }
}

void RootNode::modifyOutputs()
{
    RootNodeDialog modifyOutputsDialog(outputs);
    connect(&modifyOutputsDialog, SIGNAL(finished(int)), this, SLOT(sortOutputs(int)));
    modifyOutputsDialog.exec();
}

void RootNode::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
}

void RootNode::modifyOutputsButtonClicked(bool clicked)
{
    UNUSED(clicked);
    modifyOutputs();
}

void RootNode::sortOutputs(int result)
{
    UNUSED(result);
    outputs->sort();
    updateLayout();
    emit transferOutputsRequested(outputs);
    /// TODO: find a way to resize correctly + add cancel !!!
    performResize();
}

void RootNode::saveNodeContent(YAML::Emitter* out)
{
    if (outputConnector == nullptr)
        qDebug() << "ERROR: output connnector of root is null !";
    ((OutputConnector*) outputConnector->widget())->saveComponent(out);
}

void RootNode::performResize()
{
    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
    emit resized(this->geometry());
    if (outputConnector != nullptr)
        outputConnector->setX(outputConnector->parentWidget()->geometry().width() - 7);
}
