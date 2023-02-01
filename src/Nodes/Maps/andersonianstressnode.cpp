#include "andersonianstressnode.h"

AndersonianStressNode::AndersonianStressNode()
{
    typeOfNode = ANDERSONIANSTRESSNODE;
    localTag = "AndersonianStress";
    setWindowTitle("Andersonian Stress");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}
