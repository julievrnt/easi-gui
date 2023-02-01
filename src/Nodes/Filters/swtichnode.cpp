#include "swtichnode.h"

SwtichNode::SwtichNode()
{
    typeOfNode = SWITCHNODE;
    localTag = "Switch";
    setWindowTitle("Switch");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}
