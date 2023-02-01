#include "evalmodelnode.h"

EvalModelNode::EvalModelNode()
{
    typeOfNode = EVALMODELNODE;
    localTag = "EvalModel";
    setWindowTitle("Eval Model");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}
