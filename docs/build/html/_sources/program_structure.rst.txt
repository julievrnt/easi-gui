Understanding the program's structure
=====================================

TODO

Global View
-----------

TODO:

* Add image of structure
* 3 types of widgets: nodes, connectors and connector line

MainWindow
----------

TODO
basic functions: close, open, save, save as ...

WidgetsHandler
--------------

TODO

* handle the widgets -> add them, delete them etc...
* is called by MainWindow when a file is opened or the scene needs to be saved

EasiGraphicsView
----------------

TODO

* subclass of qgraphicsview
* handles the zoom

Nodes/NodeBase
--------------

TODO

* contains most nodes function
* defines all node types

Nodes/RootNode
--------------

TODO

* base of the tree -> a simple node with no inputs and x,y,z as outputs
* When saving, signals are sent from nodes to other connected nodes starting from the root

Nodes/NodeParentWidget
----------------------

TODO

* Parent of the nodes
* Here to making the nodes selectable and movable

Other Nodes
-----------

TODO

* contains specific functions 
* "needs" to implement: node name, saveValues() and addDimensionsRow something

Connectors/ConnectorLine
------------------------

TODO

* connect one InputConnector to one OutputConnector of the same type

Connectors/ConnectorBase
------------------------

TODO

* contains almost all connector functions
* defines all connector types

Connectors/InputConnector && OutputConnector
--------------------------------------------

TODO

* just to differenciate the types
