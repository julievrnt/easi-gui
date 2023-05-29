#include "switchnode.h"
#include "src/Nodes/Extra/switchcomponentnode.h"

SwitchNode::SwitchNode(QSharedPointer<QStringList> inputs, QSharedPointer<QStringList> outputs) : NodeBase(inputs, outputs)
{
    typeOfNode = SWITCHNODE;
    localTag = "Switch";
    setWindowTitle("Switch");
    createLayout(false, false, true);

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

SwitchNode::~SwitchNode()
{
    // switchComponentProxies deleted in clearNodes()
}

void SwitchNode::setValues(QList<QSharedPointer<QStringList>> values)
{
    if (values.isEmpty())
        return;

    while (outputConnectors->size() < values.size())
        emit addOutputConnectorRequested(getProxyNode());

    for (int i = 0; i < values.size(); i++)
    {
        // get parameters for component
        QSharedPointer<QStringList> parameters = values.at(i);

        // set parameters values
        SwitchComponentNode* switchComponentNode = static_cast<SwitchComponentNode*>(this->switchComponentProxies.at(i)->widget());
        switchComponentNode->setParameters(parameters);
    }
}

void SwitchNode::addSwitchComponentProxy(QGraphicsProxyWidget* newSwitchComponentProxy)
{
    switchComponentProxies.append(newSwitchComponentProxy);
}

void SwitchNode::clearNodes()
{
    while (switchComponentProxies.size() > 0)
        removeComponentOfDimensionRow(switchComponentProxies.size() - 1);
}

const QList<QGraphicsProxyWidget*>& SwitchNode::getSwitchComponentProxies() const
{
    return switchComponentProxies;
}

void SwitchNode::removeComponentOfDimensionRow(int index)
{
    emit deleteNodeRequested(switchComponentProxies.takeAt(index));
}

void SwitchNode::deleteOutputConnector()
{
    NodeBase::deleteOutputConnector();
    if (switchComponentProxies.size() > 1)
        removeComponentOfDimensionRow(switchComponentProxies.size() - 1);
}

void SwitchNode::updateLayout()
{
    outputs = inputs;
    emit transferOutputsRequested(outputs);
}

void SwitchNode::saveNodeContent(YAML::Emitter* out)
{
    *out << YAML::LocalTag(localTag.toStdString());
    *out << YAML::BeginMap;
    saveValues(out);
    *out << YAML::EndMap;
}

void SwitchNode::saveValues(YAML::Emitter* out)
{
    if (!static_cast<OutputConnector*>(outputConnectors->at(0)->widget())->getConnectorLineConnected())
        return;

    for (int i = 0; i < outputConnectors->size(); i++)
    {
        static_cast<OutputConnector*>(outputConnectors->at(i)->widget())->saveComponent(out);
    }
}
