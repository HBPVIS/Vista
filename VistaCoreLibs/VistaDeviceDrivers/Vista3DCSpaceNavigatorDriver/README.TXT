$Id$

3DConnexion driver notes


WINDOWS
-------

* tested on XP, vc8, sp1

Prerequisites
-------------

In order to COMPILE it, you need

* installed 3dconnexion driver that is shipped with the driver
  (the one with the panel and the COM interface)
* ATL MFC libraries that come with vc8 / vc9


If you try to RUN it without the 3dconnexion driver installed,
you may get any number of errors. If you try to load it as plugin,
the dll loader will report a missing function. That is due to an exception
thrown after initializing the COM context.
Note that you need the ATL libraries already during compile time.
The driver just utilizes the *first* 3DC navigator it finds. 
This is determined during compile time as well.


