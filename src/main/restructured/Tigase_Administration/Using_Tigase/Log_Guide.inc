.. _Tigase-Log-Guide:

Tigase Log Guide
--------------------

Tigase has multiple levels of logging available to help provide targeted and detailed information on processes, components, or traffic. In these documents we will look at where tigase generates logs, what they contain, and how we can customize them to our needs.

install.log
^^^^^^^^^^^^^^^

This log file is a basic list of files that are made on install of Tigase server. Although you may not need to use it, it can provide a handy list to see if any files were not written to your hard drive upon installation.

derby.log
^^^^^^^^^^^^^^^

If you are using the derby database installed with Tigase, this is the startup log for the database itself. Issues that might be related to the database, can be found in this file. Typically, if everything works okay, it’s a very small file with only 10 lines. It is overwritten on startup of the database.

etc/config-dump.properties
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The config-dump.properties is dump file of all your properties listed for every option within Tigase and components. The structure of the log lines is the same as the structure of Tigase XMPP Server config file - TDSL. Lets take the value for admins, listing who is administrator for the server.

::

   admins = [ 'admin@jabber.freehost.org', 'administrator@jabber.freehost.org', 'fJones@jabber.freehost.org' ]

The admin parameter which is an array of strings and has 3 users listed.

This file is re-written every time tigase starts.

logs/tigase.log.#
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The tigase.log files are where the majority of logging will take place. The rules for writing to these longs can be manipulated by editing files in the int.properties file. To see how, see the `Debugging Tigase <#debuggingTigase>`__ section of this manual for more details about how to turn on debug logging, and how to manipulate log settings. Entries to these logs are made in the following format:

::

   2015-08-10 13:09:41.504 [main]      Sctipr.init()         INFO: Initilized script command, id: comp-manager, lang:Groovy, ext: groovy

The format of these logs is below: ``<timestamp> <thread_name> <class>.<method>    <log_level>: <message> <thread_name>``. This can vary - for components it would be ``<direction>_<int>_<component name>``, for plugins it will just be the plugin name.

Let’s look at another example from the log file.

::

   2015-08-10 12:31:40.893 [in_14_muc] InMemoryMucRepository.createNewRoom()   FINE: Creating new room 'chatroom@muc.localhost.com'

The process ID may sometimes come in a different format such as ``[in_14-muc]`` which specifies the component (muc) along with the process thread identifier (14). As you can see, the format otherwise is nearly identical.

``tigase.log.#`` files are *rotated* - this means that server begins writing to tigase.log.0 when it is first run, and continues to dump information until the log size limit is hit. At this point, Tigase renames tigase.log.0 as tigase.log.1. A new tigase.log.0 will be created, and Tigase will begin logging to this file. When this file is full, tigase.log.1 will be renamed tigase.log.2 and tigase.log.0 will be renamed tigase.log.1. Using this scheme, tigase.log.0 will **always** be your most recent log.

By default, Tigase has a limit of 10000000 bytes or 10MB with a file rotation of 10 files. You can edit these values by editing the ``config.tdsl`` file and adding the following lines.

.. code::

   logging {
       java.util.logging.FileHandler {
           count = '15'
           limit = '20000000'
       }
   }

This code, if entered into the ``config.tdsl`` file increases the size of the files to 15, and enlarges the maximum size to 20MB. Note the larger the collective log space is, the larger number of sectors on hard disk are active. Large log blocks may impact system performance.

*You may see a tigase.log.0.lck file in the directory while the server is running. This is a temporary file only and is deleted once Tigase is cleanly shut down.*

logs/statistics.log.#
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Statistics log will duplicate any information that is related to sending of statistics to Tigase if you are using an unlicensed copy of Tigase XMPP server. Mainly it will consist output of LicenceChecker. The numbering logic will be the same as ``tigase.log.#`` files.

logs/tigase.pid
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``tigase.pid`` is a file that just contains the Process ID or PID for the current run of Tigase. It is only valid for the current or most recent run cycle and is overwritten every time Tigase starts.

logs/tigase-console.log
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. Important::

   This is the most important log file containing the most essential information related to operation of the Tigase XMPP Server. Any errors or exceptions in this file indicate with high probability serious issues with server operation.

This file contains information related to Tigase’s running environment, and is a dump from the server itself on what is being loaded, when, and if any issues are encountered. It will start by loading Java classes (consequently making sure the Java environment is present and functioning). Then it will begin loading the configuration file, and adding default values to settings that have not been customized. You can then see all the components being loaded, and settings added where default values are needed. Lastly you will see a log of any plugins that are loaded, and any parameters therein. You may see tags such as INFO or WARNING in the logs. Although they may contain important information, the program will continue to operate as normal are not of too great concern.

ERROR flags are issues you will want to pay attention as they may list problems that prevent Tigase or components from properly functioning.

.. Note::

   Windows does not create this file, rather the output is shown in the command line and is not dumped to a file.

If Tigase is gracefully shut down, tigase-console.log will add statistics from the server’s operation life in the following format.

::

   component/statistic = value

*Any component that may have a statistic, whether used or not, will place a value here*

This file can be handy if you are tracking issues in the server.

tigase-console.log is appended during each run session of the server.

Log File Location
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can also change the location of log files if you have a specific directory you wish to use. The configuration may be made by the following lines in your ``config.tdsl`` file:

.. code::

   logging {
       java.util.logging.FileHandler {
           pattern = '/var/log/tigase/tigase.log'
       }
   }

This setting changes the log file location to /var/log/tigase/ where all log files will be made. Files in the original location will be left.