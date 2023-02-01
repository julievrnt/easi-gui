#include "groupfilternode.h"

GroupFilterNode::GroupFilterNode()
{
    typeOfNode = GROUPFILTERNODE;
    setWindowTitle("Group Filter");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}
