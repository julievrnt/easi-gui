#ifndef SWITCHNODE_H
#define SWITCHNODE_H

#include "src/Nodes/nodebase.h"

class SwitchNode : public NodeBase
{
    Q_OBJECT
public:
    SwitchNode(QSharedPointer<QStringList> inputs = nullptr, QSharedPointer<QStringList> outputs = nullptr);
    ~SwitchNode();

    void setValues(QList<QSharedPointer<QStringList>> values);
    const QList<QGraphicsProxyWidget*>& getSwitchComponentProxies() const;
    void addSwitchComponentProxy(QGraphicsProxyWidget* newSwitchComponentProxy);
    void clearNodes();

private:
    QList<QGraphicsProxyWidget*> switchComponentProxies;
    void addOutputConnector(int index);
    void removeComponentOfDimensionRow(int index);

    // NodeBase interface
protected:
    void deleteOutputConnector();
    void updateLayout();
    void saveNodeContent(YAML::Emitter* out);
    void saveValues(YAML::Emitter* out);
};

#endif // SWITCHNODE_H
