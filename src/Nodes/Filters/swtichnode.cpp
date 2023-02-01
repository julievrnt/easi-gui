#include "swtichnode.h"

SwtichNode::SwtichNode()
{
    typeOfNode = SWITCHNODE;
    setWindowTitle("Switch");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}
