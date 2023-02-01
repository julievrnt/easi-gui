#include "functionmapnode.h"

FunctionMapNode::FunctionMapNode()
{
    typeOfNode = FUNCTIONMAPNODE;
    localTag = "FunctionMap";
    setWindowTitle("Function Map");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}
