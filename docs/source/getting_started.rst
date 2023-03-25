Getting Started with easi-gui
=============================

easi-gui is a software developped with Qt6. It needs to be compiled with qmake.
To install easi-gui, follow the next steps:

1 - Installing the Dependencies
-------------------------------

First, you will need to install the following libraries in order to compile easi-gui:

* `Qt6 <https://www.qt.io/download>`_
* `yaml-cpp <https://github.com/jbeder/yaml-cpp>`_

2 - Compiling easi-gui
----------------------

After having installed the required dependencies, clone the `easi-gui repository <https://github.com/julievrnt/easi-gui>`_. Regarding on where you have installed the yaml-cpp library, you may need to change the last three lines of ``easi-gui.pro``. When you are done, go to the root of the project and type the following commands :

.. code-block:: bash

   mkdir build
   cd build
   
The following command differs whether you have installed Qt using the command line or the official installer.

.. code-block:: bash

   # if you have installed Qt using the command line:
   qmake -makefile -o Makefile ../easi-gui.pro
   
   # if you have installed Qt using the official installer, where $QT_SRC is where Qt has been installed and $QT_VERSION is your Qt version
   $QT_SRC/$QT_VERSION/gcc_64/bin/qmake -makefile -o Makefile ../easi-gui.pro

Now you only need to execute the makefile and run the executable:

.. code-block:: bash

   makefile -j4 install
   ./easi-gui
   
You should now see the application running.
