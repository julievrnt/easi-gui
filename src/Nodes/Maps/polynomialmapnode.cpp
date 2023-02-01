#include "polynomialmapnode.h"

PolynomialMapNode::PolynomialMapNode()
{
    typeOfNode = POLYNOMIALMAPNODE;
    localTag = "PolynomialMap";
    setWindowTitle("Polynomial Map");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}
