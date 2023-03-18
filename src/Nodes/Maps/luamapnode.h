#ifndef LUAMAPNODE_H
#define LUAMAPNODE_H

#include "src/Nodes/nodebase.h"

class LuaMapNode : public NodeBase
{
    Q_OBJECT
public:
    LuaMapNode(QSharedPointer<QStringList> inputs = nullptr, QSharedPointer<QStringList> outputs = nullptr);
    ~LuaMapNode();

    void setValue(QString value);
    void setFunctionNodeProxy(QGraphicsProxyWidget* newFunctionNodeProxy);

    void performResize();
    void clearNodes();

private:
    QGraphicsProxyWidget* functionNodeProxy;

protected:
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void updateLayout();
    void saveValues(YAML::Emitter* out);
};

#endif // LUAMAPNODE_H
