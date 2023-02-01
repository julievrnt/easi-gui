#include "optimalstressnode.h"

OptimalStressNode::OptimalStressNode()
{
    typeOfNode = OPTIMALSTRESSNODE;
    localTag = "OptimalStress";
    setWindowTitle("Optimal Stress");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}
