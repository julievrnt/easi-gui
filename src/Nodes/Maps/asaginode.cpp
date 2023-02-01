#include "asaginode.h"

ASAGINode::ASAGINode()
{
    typeOfNode = ASAGINODE;
    localTag = "ASAGI";
    setWindowTitle("ASAGI");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}
