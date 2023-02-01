#include "scecfilenode.h"

SCECFileNode::SCECFileNode()
{
    typeOfNode = SCECFILENODE;
    localTag = "SCECFile";
    setWindowTitle("SCEC File");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}
