#include "sphericaldomainfilternode.h"

SphericalDomainFilterNode::SphericalDomainFilterNode()
{
    typeOfNode = SPHERICALDOMAINFILTERNODE;
    localTag = "SphericalDomainFilter";
    setWindowTitle("Spherical Domain Filter");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}
