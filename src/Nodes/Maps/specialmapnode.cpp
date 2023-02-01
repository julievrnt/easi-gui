#include "specialmapnode.h"

SpecialMapNode::SpecialMapNode()
{
    typeOfNode = SPECIALMAPNODE;
    localTag = "SpecialMap";
    setWindowTitle("Special Map");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}
