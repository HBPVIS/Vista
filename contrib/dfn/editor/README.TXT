This file describes dfnEdit, a GUI based approach to create DFN networks.

Installing
----------
* python 2.6.x 
  http://www.python.org
* pydot 1.0.2 
  http://code.google.com/p/pydot/
* pyparsing 1.5.2
  http://pyparsing.wikispaces.com/
* wxPython 2.8.x
  http://www.wxpython.org/


Note: when you are running python 2.5.x, things complicate a bit.
You will need pygraphviz and pydot and pyparsing installed, although
we will try to use only pygraphviz for the layout. Then, it is not
necessary to have the newest versions of pydot and pyparsing installed,
just use those shipped with your distro. (The above is the current
state on the long-term serviced ubuntu 8.04, for example).

* pygraphviz > 0.36 
  http://networkx.lanl.gov/pygraphviz/

Running
-------
In you shell, when everything is set up, type

cd <directory-to-dfnEdit>/src
python ./editor.py



Usage
-----

- See the menu for some top-level actions.
- Load a DFN network to see it.
- middle mouse button scrolls the whole pane, regardless
  of where the cursor is
- left button can be used to pick nodes and drag them
  (does not have a deeper meaning, but you can do so)
- right button opens a context sensitive popup menu
  - over nodes: allows to see some properties
  - over ports: allows to see the transitive hull of edges
                running into that port.  
  - over edges: allows to remove edges

  