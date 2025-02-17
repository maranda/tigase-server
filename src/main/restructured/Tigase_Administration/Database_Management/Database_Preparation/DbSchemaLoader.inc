.. _SchemaUtility:

Schema Utility
^^^^^^^^^^^^^^^

With the release of v8.0.0 calling the Tigase dbSchemaLoader utility now can be done using tasks instead of calling the specific method. Support for Derby, MySQL, PostgreSQL, MSSQL, and MongoDB is available.

In order to use this utility with any of the databases, you will need to first have the database environment up and running, and have established user credentials. You may use root or an account with administrator write privileges.

Operation & Variables
~~~~~~~~~~~~~~~~~~~~~~~~~

Operation

Operating the schema utility is quite easy! To use it run this command from the installation directory:

.. code:: bash

   ./scripts/tigase.sh [task] [params_file.conf] [options]

Operations are now converted to tasks, of which there are now three: ``install-schema``, ``upgrade-schema``, and ``destroy-schema``.

-  ``upgrade-schema``: Upgrade the schema of the database specified in your ``config.tdsl`` configuration file. (options are ignored for this option)

-  ``install-schema``: Install a schema to database.

-  ``destroy-schema``: Destroy database and schemas. **DANGEROUS**

Options

Use the following options to customize. Options in bold are required, *{potential options are in brackets}*:

-  ``--help`` Prints the help for the task.

-  ``-I`` or ``--interactive`` - enables interactive mode which will prompt for parameters not defined.

-  ``-T`` or ``--dbType`` - database type {derby, mongodb, mysql, postgresql, sqlserver}.

-  ``-C`` or ``--components`` - Allows the specification of components for use when installing a schema.

Usage
~~~~~~~

upgrade-schema

This task will locate any schema versions above your current one, and will install them to the database configured in the ``config.tdsl`` file.

.. Note::

   To use this utility, you must have Tigase XMPP server fully setup with a configured configuration file.

.. code:: bash

   ./scripts/tigase.sh upgrade-schema etc/tigase.conf

Windows users will need to run the command using the following command:

.. code::

   java -cp "jars/*" tigase.db.util.SchemaManager "upgrade-schema" --config-file=etc/config.tdsl

.. _install-schema:

install-schema
'''''''''''''''

This task will install a schema using the parameters provided.

**If you are setting up a server manually, we HIGHLY recommend using this method**

.. code:: bash

   ./scripts/tigase.sh install-schema [Options]

This command will install tigase using a Derby database on one named ``tigasedb`` hosted on ``localhost``. The username and password editing the database is ``tigase_pass`` and ``root``. Note that ``-J`` explicitly adds the administrator, this is highly recommended with the ``-N`` passing the password.

If you are using a windows system, you need to call the program directly:

.. code::

   java -cp "jars/*" tigase.db.util.SchemaManager "install-schema" [options]


Options
'''''''''

Options for schema installation are as follows, required options are in bold

-  ``--help``, Outputs the help.

-  ``-I``, ``--interactive`` - enables interactive mode, which will result in prompting for any missing parameters.

-  ``-C``, ``--components=`` - list of enabled components identifiers (+/-), possible values: [``amp``, ``bosh``, ``c2s``, ``eventbus``, ``ext-disco``, ``http``, ``mdns``, ``message-archive``, ``monitor``, ``muc``, ``pubsub``, ``push``, ``s2s``, ``socks5``, ``test``, ``unified-archive``, ``upload``, ``ws2s``] (default: amp,bosh,c2s,eventbus,http,message-archive,monitor,muc,pubsub,s2s,ws2s). **This is required for certain components like socks5.**

-  ``-T``, ``--dbType=`` - database server type, possible values are: [``derby``, ``mongodb``, ``mysql``, ``postgresql``, ``sqlserver``] (*required*)

-  ``-D``, ``--dbName=`` - name of the database that will be created (by default it is ``tigasedb``). (*required*)

-  ``-H``, ``--dbHostname=`` - address of the database instance (by default it is ``localhost``). (*required*)

-  ``-U``, ``--dbUser=`` - name of the user that will be created specifically to access Tigase XMPP Server database (default is ``tigase_user``). (*required*)

-  ``-P``, ``--dbPass=`` - password of the user that will be created specifically to access Tigase XMPP Server database (default is ``tigase_pass``). (*required*)

-  ``-R``, ``--rootUser=`` - database root account username used to create user and database (default is ``root``). (*required*)

-  ``-A``, ``--rootPass=`` - database root account password used to create user and database (default is ``root``). (*required*)

-  ``-S``, ``--useSSL`` - enable SSL support for database connection (if the database supports it) (default is false).

-  ``-F``, ``--file=`` - comma separated list of SQL files that will be processed.

-  ``-Q``, ``--query=`` - custom queries to be executed, see `Query function <#queryschema>`__ for details.

-  ``-L``, ``--logLevel=`` - logger level used during loading process (default is ``CONFIG``).

-  ``-J``, ``--adminJID=`` - comma separated list of administrator JID(s).

-  ``-N``, ``--adminJIDpass=`` - password that will be used for the entered JID(s) - one password for all configured JIDs.

-  ``--getURI=`` - generate database URI (default is ``false``).

-  ``--ignoreMissingFiles=`` - force ignoring missing files errors (default is ``false``).

Query function

Should you decide to customize your own functions, or have specific information you want to put into the database, you can use the -query function to perform a single query step.

.. code::

   ./scripts/tigase.sh install-schema -T mysql -D tigasedb -R root -A root -Q "CREATE TABLE tigasedb.EXTRA_TABLE (id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, name VARCHAR(10) NOT NULL)"

Of course this would break the schema for tigasedb by adding an unexpected table, you will receive the following message:

::

   tigase.db.util.DBSchemaLoader       printInfo          WARNING       Database schema is invalid

But this is a demonstration how you may run a query through the database without the need to use another tool. Note that you will need to select the specific database for each query.

destroy-schema


This will destroy the database specified in the configuration file.

.. Warning::

    **THIS ACTION IS NOT REVERSIBLE**

.. code::

   ./scripts/tigase.sh destroy-schema etc/config.tdsl

Only use this if you wish to destroy a database and not have the information recoverable.

Windows users will need to call the method directly:

.. code::

   java -cp "jars/*" tigase.db.util.SchemaManager "destroy-schema" etc/config.tdsl


A note about MySQL

If you are using these commands, you may result in the following error:

.. code:: bash

   tigase.util.DBSchemaLoader       validateDBConnection    WARNING    Table 'performance_schema.session_variables' does not exist

If this occurs, you will need to upgrade your version of MySQL using the following command:

.. code:: bash

   mysql_upgrade -u root -p --force

After entering the password and upgrading MySQL the schema error should no longer show when working with Tigase databases.