#include "functionnode.h"
#include <QTextEdit>

FunctionNode::FunctionNode(QStringList* inputs) : NodeBase(inputs)
{
    typeOfNode = FUNCTIONNODE;
    setWindowTitle("Function");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void FunctionNode::setValue(QStringList* inputs, QString value)
{
    if (inputs != nullptr)
    {
        this->inputs = inputs;
        updateLayout();
    }

    QVBoxLayout* functionLayout = this->layout()->findChild<QVBoxLayout*>("functionLayout");
    QTextEdit* function = (QTextEdit*) functionLayout->itemAt(0)->widget();
    function->setText(value);
}

void FunctionNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout(this);
    addTitleLayout(globalLayout, true);
    addInputsLayout(globalLayout);
    addFunctionLayout(globalLayout);
    setLayout(globalLayout);
}

void FunctionNode::addInputsLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* inputsLayout = new QHBoxLayout();
    inputsLayout->setObjectName("inputsLayout");
    for (int i = 0; i < inputs->size(); i++)
    {
        QPushButton* inputButton = new QPushButton(inputs->at(i));
        connect(inputButton, SIGNAL(clicked(bool)), this, SLOT(inputButtonClicked(bool)));
        inputsLayout->addWidget(inputButton);
    }
    globalLayout->addLayout(inputsLayout);
}

void FunctionNode::addFunctionLayout(QVBoxLayout* globalLayout)
{
    QVBoxLayout* functionLayout = new QVBoxLayout();
    functionLayout->setObjectName("functionLayout");
    QTextEdit* function = new QTextEdit();
    function->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    function->setFixedSize(500, 100);
    function->setText("");
    functionLayout->addWidget(function);
    globalLayout->addLayout(functionLayout);
}

void FunctionNode::updateLayout()
{
    QHBoxLayout* inputsLayout = this->layout()->findChild<QHBoxLayout*>("inputsLayout");
    QStringList oldInputs = QStringList();


    if (!inputsLayout->isEmpty())
    {
        for (int i = 0; i < inputsLayout->count(); i++)
        {
            oldInputs.append(((QPushButton*) inputsLayout->itemAt(i)->widget())->text());
        }
    }

    for (int i = 0; i < oldInputs.size(); i++)
    {
        if (!inputs->contains(oldInputs.at(i)))
        {
            QPushButton* buttonToRemove = (QPushButton*) inputsLayout->itemAt(i)->widget();
            inputsLayout->removeWidget(buttonToRemove);
            buttonToRemove->deleteLater();
            oldInputs.remove(i);
            i--;
        }
    }

    for (int i = 0; i < inputs->size(); i++)
    {
        if (!oldInputs.contains(inputs->at(i)))
        {
            oldInputs.insert(i, inputs->at(i));
            QPushButton* inputButton = new QPushButton(inputs->at(i));
            connect(inputButton, SIGNAL(clicked(bool)), this, SLOT(inputButtonClicked(bool)));
            inputsLayout->insertWidget(i, inputButton);
        }
    }
}

void FunctionNode::saveNodeContent(YAML::Emitter* out)
{
    saveValues(out);
}

void FunctionNode::saveValues(YAML::Emitter* out)
{
    QString value = getValue();
    if (value.size() >= 6)
        if (value.first(6) != "return")
            *out << YAML::Literal;
    *out << value.toStdString();
}

QString FunctionNode::getValue()
{
    QVBoxLayout* functionLayout = this->layout()->findChild<QVBoxLayout*>("functionLayout");
    QTextEdit* function = (QTextEdit*) functionLayout->itemAt(0)->widget();
    return function->toPlainText();
}

void FunctionNode::inputButtonClicked(bool clicked)
{
    Q_UNUSED(clicked);
    QPushButton* inputButton = qobject_cast<QPushButton*>(sender());
    QVBoxLayout* functionLayout = this->layout()->findChild<QVBoxLayout*>("functionLayout");
    QTextEdit* function = (QTextEdit*) functionLayout->itemAt(0)->widget();
    function->insertPlainText(inputButton->text());
}
