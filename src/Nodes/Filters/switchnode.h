#ifndef SWITCHNODE_H
#define SWITCHNODE_H

#include "src/Nodes/nodebase.h"

class SwitchNode : public NodeBase
{
    Q_OBJECT
public:
    SwitchNode(QStringList* inputs = nullptr, QStringList* outputs = nullptr);

    void setValues(QList<QStringList*> values);
    void addSwitchComponentProxy(QGraphicsProxyWidget* newSwitchComponentProxy);
    void clearSwitchComponentNodes();

private:
    QList<QGraphicsProxyWidget*> switchComponentProxies;
    void addOutputConnector(int index);
    void removeComponentOfDimensionRow(int index);

    // NodeBase interface
protected:
    void deleteOutputConnector();
    void updateLayout();
    void saveNodeContent(YAML::Emitter *out);
    void saveValues(YAML::Emitter* out);
};

#endif // SWITCHNODE_H
