Understanding the program's structure
=====================================

Let's first take a quick look on the program's structure. The program is divided in three parts :

* **MainWindow** and **WidgetsHandler** - they handle the basic functions of the software and the scene (e.g. opening a file, adding a node to the scene).
* **Nodes** - they handle the content of the nodes (e.g. adding a parameter, saving its content).
* **Connectors** and **ConnectorLine** - they handle the connection between two nodes as well as the transfer of information (e.g. the outputs of the parent).

MainWindow & WidgetsHandler
---------------------------

**MainWindow** deals with the basic features of a software: new, open, save, save as and close. It is also in this class, that the different actions created in ``mainwindow.ui`` are connected to other **MainWindow** methods. When an action in the menu or on the toolbar is triggered, **MainWindow** receives the signal and handles the action or transfer it to **WidgetsHandler**.

**WidgetsHandler** takes care of adding and deleting nodes, as well as connecting them to one another. It is called by **MainWindow** when a node needs to be added to the scene (by opening a file or adding a specific node using the menu), or when the scene needs to be saved. In the later case, it asks the root node **Inputs** to save its content, which further asks its child to save its content and so on.

Nodes
-----

There is one node class that you need to know: **NodeBase**. It contains most node functions like transferring the outputs, resizing the node and adding the title and component layouts, and can be further expanded, in case several nodes have the same code. All node types are subclasses of NodeBase.

The nodes are divided into four categories: maps, filters, builders and extra. Extra contains helper nodes that are connected to a map, filter or builder node. They contain parts of the map, filter or builder node to which they are connected. These parts have been separated into a new node for aesthetic reasons. This is where e.g. the **Function** node can be found. It is the node in which the functions are written for the **Function Map** node or the **Lua Map** node and that is connected to them via a blue connector.

The main methods needed for the subclasses are: ``addNewDimensionsLayoutRow`` and ``saveValues``. The first method is used to add a new row of dimensions / parameters used as outputs (see e.g. the **Constant Map** node or the **Affine Map** node). ``saveValues`` is where the content of a node is saved.

Connectors & ConnectorLine
--------------------------

They are two types of connectors: **InputConnector** and **OutputConnector**. Like their names suggest, one serves as the input of a node, while the other one is one of the many connectors to which another node can be connected. Except for a special case (see `Color Codes <https://easi-gui.readthedocs.io/en/latest/getting_started.html#color-codes>`_), an **InputConnector** can only be connected to an **OutputConnector** of the same subtype. The different subtypes and color codes are:

* NONE - green
* SPECIALCOMPONENT - blue
* MATH - violet
* EVAL - red

If you want to change the color of a connector or to create a new subtype of connector, you will need to update the parent class called **ConnectorBase**.

**ConnectorLine** is a class that represents a line between an **InputConnector** and **OutputConnector**.
