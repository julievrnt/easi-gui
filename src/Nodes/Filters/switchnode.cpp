#include "switchnode.h"
#include "src/Nodes/Extra/switchcomponentnode.h"

SwitchNode::SwitchNode(QStringList* inputs, QStringList* outputs) : NodeBase(inputs, outputs)
{
    typeOfNode = SWITCHNODE;
    localTag = "Switch";
    setWindowTitle("Switch");
    createLayout(false, false, true);
    
    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void SwitchNode::setValues(QList<QStringList *> values)
{
    if (values.isEmpty())
        return;

    while(outputConnectors->size() < values.size())
        emit addOutputConnectorRequested(getProxyNode());

    for (int i = 1; i < values.size(); i++){
        // get parameters for component
        QStringList* parameters = values.at(i);

        // set parameters values
        SwitchComponentNode* switchComponentNode = (SwitchComponentNode*) this->switchComponentProxies.at(i)->widget();
        switchComponentNode->setParameters(parameters);
    }
}

void SwitchNode::addSwitchComponentProxy(QGraphicsProxyWidget *newSwitchComponentProxy)
{
    switchComponentProxies.append(newSwitchComponentProxy);
}

void SwitchNode::clearSwitchComponentNodes()
{
    while(switchComponentProxies.size()>0)
        removeComponentOfDimensionRow(switchComponentProxies.size()-1);
}

void SwitchNode::removeComponentOfDimensionRow(int index)
{
    emit deleteNodeRequested(switchComponentProxies.takeAt(index));
}

void SwitchNode::deleteOutputConnector()
{
    NodeBase::deleteOutputConnector();
    if(switchComponentProxies.size()>0)
        removeComponentOfDimensionRow(switchComponentProxies.size() - 1);
}

void SwitchNode::updateLayout()
{
    outputs = inputs;
    emit transferOutputsRequested(outputs);
}

void SwitchNode::saveNodeContent(YAML::Emitter *out)
{
    *out << YAML::LocalTag(localTag.toStdString());
    *out << YAML::BeginMap;
    saveValues(out);
    *out << YAML::EndMap;
}

void SwitchNode::saveValues(YAML::Emitter *out)
{
    if (!((OutputConnector*) outputConnectors->at(0)->widget())->getConnectorLineConnected())
        return;

    for (int i = 0; i < outputConnectors->size(); i++)
    {
        ((OutputConnector*) outputConnectors->at(i)->widget())->saveComponent(out);
    }
}
