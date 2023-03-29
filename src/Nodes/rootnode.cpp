#include "rootnode.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

RootNode::RootNode()
{
    typeOfNode = ROOTNODE;
    hasMenu = false;
    setWindowTitle("Inputs");
    outputConnector = nullptr;

    outputs = QSharedPointer<QStringList>(new QStringList);
    *outputs << "x" << "y" << "z";

    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

RootNode::~RootNode()
{
    delete outputConnector;
    outputConnector = nullptr;
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
    addTitleLayout(globalLayout, true);

    // create layout in order to modify it later
    QVBoxLayout* outputsLayout = new QVBoxLayout();
    outputsLayout->setObjectName("outputsLayout");

    for (int i = 0; i < outputs->size(); i++)
    {
        QLabel* output = new QLabel();
        output->setText(outputs->at(i));
        output->setAlignment(Qt::AlignCenter);
        output->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        output->setFixedWidth(60);
        outputsLayout->addWidget(output);
    }

    globalLayout->addLayout(outputsLayout);

    this->setLayout(globalLayout);
}

void RootNode::saveNodeContent(YAML::Emitter* out)
{
    if (outputConnector == nullptr)
    {
        qDebug() << "ERROR: output connnector of root is null !";
        return;
    }
    ((OutputConnector*) outputConnector->widget())->saveComponent(out);
}

void RootNode::performResize()
{
    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
    emit resized(this->geometry());
    if (outputConnector != nullptr)
        outputConnector->setX(outputConnector->parentWidget()->geometry().width() - 7);
}
