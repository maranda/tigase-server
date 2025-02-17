.. _Tigase-and-Python:

Tigase and Python
^^^^^^^^^^^^^^^^^^^^

This article describes how to get Python working as a scripting language for ad-hoc commands in Tigase server. The first part is installation, and the second shows a few code examples with explanation of the differences between Python usage and some other languages.

*Please note, we are not a Python developer, and by no means this is Python development guide. All the code examples are used only to present the API available and there are certainly better ways to do it in the proper Python style. If you have any suggestions or have a better code examples I am happy to include them in the guide.*

Installation
~~~~~~~~~~~~~~~

In short, installation is extremely simple: just copy the file attached to this article to your Tigase installation, to the ``libs/`` directory. Restart the server and you are ready to start scripting and executing Python.

In theory the Tigase offers scripting support defined in `JSR-223 <http://www.jcp.org/en/jsr/detail?id=223>`__. You can use any language for which there is such support for JVM. This includes also stand-alone python implementations and the JSR-223 plugins acts just as a bridge. This, however, does not make much sense as you are not able to interact with JVM code (Tigase API). Therefore you need a language which is executed within JVM and can easily exchange data between the main application (Tigase server) and the script.

|lang list no python small|

The best way to go is to use Jython implementation. It works very well within JVM and more importantly, perfectly integrates with Tigase server. Tigase server is tested with **Jython-2.2.1** and is confirmed to work fine. Version **Jython-2.5.1** is recommended however, and all the examples are executed with this version installed. Please note, *Jython-2.5.0* does not work at all. Both supported versions can be downloaded from the `Jython website <http://wiki.python.org/jython/DownloadInstructions>`__.

**Version 2.5.1** is a bit simpler to install. When you download and run the Jython installer, find ``jython.jar`` file in the directory where you installed Jython. Copy the file to the Tigase’s **libs/** directory and all is ready to go. Please note, this is the same file as the one attached to this article for your convenience.

**Version 2.2.1** needs a little bit more work. The first part is the same. It is not, however enough to copy the ``jython.jar`` file. One more file is necessary for the Jython to work with the Tigase server. You have to install JSR-223 engine separately. The binary file has to be unpacked and ``jython-engine.jar`` file needs to be copied to the Tigase ``libs/`` directory.

The best way to check if the Jython is installed correctly and support for Python is enabled, is by trying to submit a new script to the Tigase server. Browser the server service discovery, select "*Session manager*" component and run "*Execute command*" function. A new window should show with a list of all available ad-hoc commands. Select "*New command script*" item and click "*Execute*". Ad-hoc command dialog windows should show up. One of the field is "*Language*" with pull down list of available scripting languages. If "*python*" is on the list it means everything is ok and support for Python is enabled.

|lang list with python small|

Writing Python Scripts
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Python scripts work in a similar way to Groovy or other languages scripts, except one significant difference. You cannot call "*return*" from the script itself. Hence you cannot simply pass script results by calling "*return*" statement directly from the script.

To overcome the problem, Tigase offers another way to pass script execution results. It checks the value of a special variables on the script completion: “result” and “packet”. By assigning value to one of these variables the Python (or any other language) can pass execution results back to the Tigase server.

-  ``result`` allows to return simple text (or characters String) from the script.

-  ``packet`` allows to return Packet instance which is send back to the user.

The simplest possible Python script may look like this one:

``result = "Hello world!"``

For instructions how to load and execute the script, please refer to the `introductory article <#scriptingintro>`__ for scripting in Tigase server. There were some minor changes in Tigase 4.4.0 and later versions, so please have a look at the `article <#newElements>`__ describing new elements as well.

An example of a more advanced script asks the user for providing required parameters for the actual script execution:

.. code:: python

   from java.lang import *
   from tigase.server import *

   num1 = Command.getFieldValue(packet, "num1")
   num2 = Command.getFieldValue(packet, "num2")

   if num1 is None or num2 is None:
     res = Iq.commandResultForm(packet)
     Command.addTextField(res, "Note", "This is a Python script!")
     Command.addFieldValue(res, "num1", "")
     Command.addFieldValue(res, "num2", "")
     packet = res
   else:
     result = num1 + num2

Except this minor difference, the rest part of scripting in Python for the Tigase administrator commands is the same as all other languages. As all languages can return execution results via these special variables, it could be argued there is no difference at all.

In article *"Component Implementation - Lesson 6 - Scripting Support"* in Developer guide, I am going to present the Tigase server API available for scripting framework. My main language is Groovy as it offers the best integration with JVM and Tigase API, however I will try to include Python example code as well.

.. |lang list no python small| image:: /images/admin/lang-list-no-python-small.png
.. |lang list with python small| image:: /images/admin/lang-list-with-python-small.png
