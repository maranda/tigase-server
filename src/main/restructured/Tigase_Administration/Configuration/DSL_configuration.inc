.. _dslConfig:

DSL file format
-------------------

In previous Tigase XMPP Server releases configuration was stored in properties based configuration file. From Tigase XMPP Server 8.0.0 release it will be required to use new DSL based configuration file format. This file format was inspired by Groovy language syntax and new core feature of Tigase XMPP Server - Tigase Kernel Framework.

why new format?
^^^^^^^^^^^^^^^^^^^^^

In properties configuration format each line contained key and value with optional definition of type of stored value:

.. code::

   c2s/ports[i]=5222,5223

where ``c2s/ports`` was name of property, ``[i]`` defined that type of value is array of integers, and ``5222,5223`` was comma separated list of values.

This format worked but in fact ``c2s/ports`` was not name of property you configured but key which was later split on ``/`` char to parts which defined by names path to property which name was in last part.From that you can see that it was domain based setting of properties.

Except from this multi-part keys we also used properties starting with ``--`` which were global properties accessible for every part of application, i.e.: to add new component and set some properties you needed to write:

.. code::

   --comp-name-1=pubsub
   --comp-class-1=tigase.pubsub.PubSubComponent
   pubsub/test[B]=true
   pubsub/pubsub-repo-url="jdbc:XXXX:XXXX/db_name"

This lead to mistakes like duplicated definition of name and class for same number of component or redefined property value in other place of a configuration file - especially in cases where configuration was big.

In this configuration structure it was hard to tell where is configuration for particular component or what databases this installation uses. This could be defined all over the file.

In this release we are introducing Tigase Kernel Framework, which allows to configure beans in configuration file and even define usage of new beans loaded from external jars which can modify behavior of Tigase components. This would make configuration file even more complex, difficult and less readable.

What is DSL?
^^^^^^^^^^^^^^^

DSL stands for domain-specific language - in this case language created for storage of configuration.

Now we use domain based configuration which means that our configuration file is not a flat key=value storage but it defines objects, it's properties and assigned values.

To illustrate it better let's start with a simple example. In properties file in order to configure PubSub component named ``pubsub`` you would use following properties:

.. code::

   --comp-name-1=pubsub
   --comp-class-1=tigase.pubsub.PubSubComponent
   pubsub/test[B]=true

In DSL based configuration this would be replaced by following block

.. code::

   pubsub (class: tigase.pubsub.PubSubComponent) {
       # comment
       test = true
   }

in which we define bean with name `pubsub` and set it's class inside ``()`` block to ``tigase.pubsub.PubSubComponent``.
We also use block between ``{}`` chars to define properties which are related to bean.
Which means this properties will be passed only to this instance of Tigase PubSub Component, same as it was before where we needed to add prefix.
Entries after ``\#`` are comments, to pass ``#`` you need to wrap whole part containing it in ``''``, ie. ``'test#242'``

.. WARNING:: 
    If a string value assigned to a property contains any char from a following list ``=:,[]#+-*/`` it needs to be wrapped in a ``''``.

Why DSL?
^^^^^^^^^

DSL configuration format provides a number of advantages over the old system of configuration. All configurations for components are related in a single block, so they are not spread out over several different lines. No need for long property names, no longer have to invoke a long string of settings for multiple values. Support is provided for environment variables. No longer need to escape certain characters, making settings far more readable at a glance. Values may be set using basic calculations, such as ``100 * 200 * 2`` rather than ``40000``. Parameter type values are no longer necessary, no more [i], [S], [B] etc.. Comma separated values can now be simplified lists with separate entries being able to be in multiple lines.

Although the format may seem more complex, looking like a section of java code, the formatting is consistent and can be far more readable. After some experience with DSL format, you'll find it's far more intuitive and user friendly than it may appear. Of course if there's any real confusion, Tigase can automatically convert old style properties files to the DSL format using the following command:

.. code-block:: bash

   ./scripts/tigase.sh upgrade-config etc/tigase.conf

Setting property
~~~~~~~~~~~~~~~~~~

To set property you just write property name followed by `=` and value to set. This is always done in context of bean which configuration property you want to set.

.. code-block::

   test=true

It is also possible to set property in main context by placing property outside of any context.
This sets property which value is available to access by any bean.

Setting global property
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Like in properties file it is still possible to use property names starting with ``--`` without any context or any other properties at global scope. Format is the same as in case of setting property but they are defined without scope (in global scope). This properties are global and accessible by any bean but also set as system property in JVM.

Defining bean
~~~~~~~~~~~~~~~~~~

You can configure bean by using following format:

.. code:: bash

   beanName (class: className, active: activeValue, exportable: exportableValue) {
       # scope of bean properties
   }

where ``beanName`` is name under which you want to configure bean.
`beanName` must be wrapped in ``''``, if ``beanName`` contains characters like ``=:,[]#+-*/`` and is recommended, if ``beanName`` is numeric only.

Inside block between ``(` and `)`` you can define:

* ``class`` which will be used as a bean, in example above we set class as ``className``. *_(default: if you try to configure bean under name which has default class assigned with it in Tigase framework then this assigned class will be used. In other case you need to pass name of class to use as a bean)_*
* ``active`` (boolean) whether you want the bean to be active or not (beans with ``active`` set to ``false`` are not loaded). *_(default: true)_*
* ``exportable`` (boolean) defines if this bean should be exported and available for use for beans in inner scopes. This is advanced option in most cases it is recommended to omit this field in configuration. *_(default: false)_*

Spaces between ``beanName`` and block between ``()`` is optional as well as space between block ``()`` and block ``{}``.
It is recommended that properties of bean would be placed in separate lines with indentation and first property will be placed in new line.

.. important::

   Usage of ``()`` block is very important. When this block is used in configuration it automatically sets ``active`` property of bean definition for bean for which it is used to to `true`. This is done due to fact that default value of ``active`` is ``true``.

   If you omit it in configuration, you will set bean configuration but it may remain ``inactive``. In this state bean will not be loaded and as a result will not be used by Tigase XMPP Server.

Configuring bean
~~~~~~~~~~~~~~~~~~

If you know that bean is defined and you do not want to change it's activity or class then you can just pass properties to configure bean in following way:

.. code::

   beanName {
       # scope of bean properties
       test = true
   }


where ``beanName`` is name of bean to configure and `test` is name of property to set to ``true`` in this bean.

Format of values
~~~~~~~~~~~~~~~~~~

In properties based configuration file every property was defined as a string and only by defining expected format it was properly converted to expected value. In DSL it is possible to set values in two ways:

as an object
   Using this format you set list as a list and integer is set as an integer.

.. table:: Format of values

    +-------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | Type        | Description                                                                                                                                                                                                            |
    +=============+========================================================================================================================================================================================================================+
    | **string**  | Wrap it in ``''``, ie. to set ``test`` as string you use ``'test'``                                                                                                                                                    |
    +-------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | **integer** | Just put value, ie. to set ``543`` use ``543``                                                                                                                                                                         |
    +-------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | **long**    | Put value and follow it with ``L``, ie. to set ``23645434`` as long use ``23645434L``                                                                                                                                  |
    +-------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | **float**   | Put value and follow it with ``f``, ie. to set ``231.342`` use ``231.342f``                                                                                                                                            |
    +-------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | **boolean** | To set value just use ``true`` or ``false``                                                                                                                                                                            |
    +-------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | **list**    | Lists can be of many types and to make it simple we decided to use as a comma separated list of values in proper format wrapped in ``[]``.                                                                             |
    |             |                                                                                                                                                                                                                        |
    |             | -  of strings - ``[ 'alfa', 'beta', 'gamma' ]``                                                                                                                                                                        |
    |             |                                                                                                                                                                                                                        |
    |             | -  of integers - ``[ 1, 2, 3, 4]``                                                                                                                                                                                     |
    |             |                                                                                                                                                                                                                        |
    |             | You can write it in multiple lines if you want:                                                                                                                                                                        |
    |             |                                                                                                                                                                                                                        |
    |             | ::                                                                                                                                                                                                                     |
    |             |                                                                                                                                                                                                                        |
    |             |    [                                                                                                                                                                                                                   |
    |             |        'alfa'                                                                                                                                                                                                          |
    |             |        'beta'                                                                                                                                                                                                          |
    |             |        'gamma'                                                                                                                                                                                                         |
    |             |    ]                                                                                                                                                                                                                   |
    +-------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | **map**     | Maps can be written as a block of properties wrapped in ``{}``. This format of map is the same as used for passing configuration to bean properties. Keys and values can be written in separate lines *(recommended)*: |
    |             |                                                                                                                                                                                                                        |
    |             | ::                                                                                                                                                                                                                     |
    |             |                                                                                                                                                                                                                        |
    |             |    {                                                                                                                                                                                                                   |
    |             |        test = true                                                                                                                                                                                                     |
    |             |        ssl = false                                                                                                                                                                                                     |
    |             |        ssl-certificate = '/test/cert.pem'                                                                                                                                                                              |
    |             |        another-map = {                                                                                                                                                                                                 |
    |             |            key = 'value'                                                                                                                                                                                               |
    |             |        }                                                                                                                                                                                                               |
    |             |    }                                                                                                                                                                                                                   |
    |             |                                                                                                                                                                                                                        |
    |             | or in single line *(separation with spaces is not required)*:                                                                                                                                                          |
    |             |                                                                                                                                                                                                                        |
    |             | ::                                                                                                                                                                                                                     |
    |             |                                                                                                                                                                                                                        |
    |             |    { test = true, ssl = false, ssl-certificate = '/test/cert.pem' }                                                                                                                                                    |
    +-------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

as a plain string
   Very similar to properties based configuration, in fact values are passed in same format and later are converted to correct type by checking type expected by bean. *(Not recommended)*

.. table:: Types

    +-------------+-----------------------------------------------------------------------------------------------+
    | Type        | Description                                                                                   |
    +=============+===============================================================================================+
    | **string**  | Just put value, ie. to set ``test`` use ``test``                                              |
    +-------------+-----------------------------------------------------------------------------------------------+
    | **integer** | Just put value, ie. to set ``543`` use ``543``                                                |
    +-------------+-----------------------------------------------------------------------------------------------+
    | **long**    | Put value, ie. to set ``23645434`` as long use ``23645434``                                   |
    +-------------+-----------------------------------------------------------------------------------------------+
    | **float**   | Put value, ie. to set ``231.342`` use ``231.342``                                             |
    +-------------+-----------------------------------------------------------------------------------------------+
    | **boolean** | To set value just use ``true`` or ``false``                                                   |
    +-------------+-----------------------------------------------------------------------------------------------+
    | **list**    | List needs to be written as comma separated list of values, ie. ``test,abc,efg`` or ``1,2,3`` |
    +-------------+-----------------------------------------------------------------------------------------------+
    | **map**     | Not possible                                                                                  |
    +-------------+-----------------------------------------------------------------------------------------------+


Using values from System Properties and Environment Variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Now it is possible to use values of `system properties <https://docs.oracle.com/javase/tutorial/essential/environment/sysprop.html>`__ and `environment variables <https://docs.oracle.com/javase/tutorial/essential/environment/env.html>`__ and assign them to bean properties. For this purpose we added functions which can be used in DSL and which will return values of:

system property
   ``prop('property-name')`` or ``prop('property-name','default value')``

environment variable
   ``env('variable-name')``

**Example of setting value of system property and environment variable to bean ``user``.**

.. code::

   user {
     name = env('USER')
     home = prop('user.home')
     paths = [ prop('user.home'), prop('user.dir') ]
   }


.. Warning::

    For properties which accepts lists it is not allowed to set value using variable/property with comma separated values like ``value1,value2`` wrapped in ``[]``, ie. ``property = [ env('some-variable') ]``. It needs to be set in following way ``property = env('some-variable')``
 
Computed values
~~~~~~~~~~~~~~~~~~

With DSL configuration format we introduce support for computable values for properties. It is now possible to set value which is result of a computation, ie. concatenation of a strings or very simple mathematical expression. We currently support only following mathematical operations:

-  add

-  subtract

-  multiply

-  divide

**Example of setting environment variable related path and computed timeout.**

.. code::

   bean {
     # setting path to `some-subdirectory` of user home directory
     path = prop('user.home') + '/some-subdirectory/'

     # setting timeout to 5 minutes (setting value in milliseconds)
     timeout = 5L * 60 * 1000
     # previously it would need to be configured in following way:
     # timeout = 300000L
   }

.. warning::

    For properties which accepts lists it is not allowed to set value using computed values with comma separated values like ``value1,value2`` wrapped in ``[]``, ie. ``property = [ env('some-variable') + ',other-value' ]``. It needs to be set in following way ``property = env('some-variable') + ',other-value'``.

Period / Duration values
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Some configuration options allow control of execution of tasks with particular period or within certain duration. DSL file format accepts strings denoting particular amount of time, which follows Java’s native structures (see: `Period <https://docs.oracle.com/javase/8/docs/api/java/time/Period.html#parse-java.lang.CharSequence->`__ and `Duration <https://docs.oracle.com/javase/8/docs/api/java/time/Duration.html#parse-java.lang.CharSequence->`__ for detailed explanation).

-  ``Duration`` formats accepted are based on the ISO-8601 duration format ``PnDTnHnMn.nS`` with days considered to be exactly 24 hours, for example:

   -  ``PT20.345S`` - 20.345 seconds

   -  ``PT15M`` - 15 minutes (where a minute is 60 seconds)

   -  ``PT10H`` - 10 hours (where an hour is 3600 seconds)

   -  ``P2D`` - 2 days (where a day is 24 hours or 86400 seconds)

   -  ``P2DT3H4M`` - 2 days, 3 hours and 4 minutes

-  ``Period`` format is based on the ISO-8601 period formats PnYnMnD and PnW, for example, the following are valid inputs:

   -  ``P2Y`` - 2 years

   -  ``P3M`` - 3 months

   -  ``P4W`` - 4 weeks

   -  ``P5D`` - 5 days

   -  ``P1Y2M3D`` - 1 year, 2 months, 3 days

   -  ``P1Y2M3W4D`` - 1 year, 2 months, 3 weeks, 4 days


Example configuration file in DSL
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code::

   # Enable cluster mode
   --cluster-mode = true
   # Enable debugging for server and xmpp.impl
   --debug = 'server,xmpp.impl'
   # Set list of virtual hosts (old way)
   --virt-hosts = 'example.com,test-1.example.com,test-2.example.com'

   # Configure list of administrator jids
   admins = [ 'admin@zeus', 'http@macbook-pro-andrzej.local' ]
   # Set config type
   config-type = '--gen-config-def'

   # Configure dataSource bean with database configuration
   dataSource {
       # Configure default data source (using default implementation so class is omitted)
       default () {
           uri = 'jdbc:postgresql://127.0.0.1/tigase?user=test&password=test&autoCreateUser=true'
       }

       # Configure data source with name exaple.com (will be used by domain example.com)
       'example.com' () {
           uri = 'jdbc:mysq://127.0.0.1/example?user=test&password=test&autoCreateUser=true'
       }
   }

   # Configure C2S component
   c2s {
       # Enable Stream Management bean
       'urn:xmpp:sm:3' () {}

       # Register tigase.server.xmppclient.SeeOtherHostDualIP as seeOtherHost bean
       seeOtherHost (class: tigase.server.xmppclient.SeeOtherHostDualIP) {}

       # Add additional port 5224 which is SSL port and disable port 5223
       connections () {
           '5224' () {
                socket = ssl
             }
           '5223' (active: false) {}
       }
   }

   # Configure HTTP API component
   http {
       # Set list of API keys
       api-keys = [ 'test1234', 'test2356' ]
       rest {
           # Set value of environment property as a path to look for REST scripts
           rest-scripts-dir = env('TIGASE_REST_SCRIPTS_DIR')
       }
   }

   # Register pubsub-2 (class is passed as pubsub-2 name do not have default class assigned)
   pubsub-2 (class: tigase.pubsub.cluster.PubSubComponentClustered) {
       # Set configuration bean properties
       pubsubConfig {
           persistentPep = true
       }
       # Use tigase.pubsub.cluster.ClusteredNodeStrategy as advanced clustering strategy
       strategy (class: tigase.pubsub.cluster.ClusteredNodeStrategy) {}
   }

   # Configure Session Manager
   sess-man {
       # Here we enable pep, urn:xmpp:mam:1 processors and disable message-archive-xep-0136 processor
       pep () {}
       'urn:xmpp:mam:1' () {}
       message-archive-xep-0136 (active: false) {}

       # Define class used as clustering strategy (it is different than default so class is required)
       strategy (class: tigase.server.cluster.strategy.OnlineUsersCachingStrategy) {}
   }

Default configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^

Tigase XMPP Server is packaged with a basic ``config.tdsl`` file that tells the server to start up in setup mode.

.. code::

   'config-type' = 'setup'

   http () {
       setup () {
           'admin-user' = 'admin'
       'admin-password' = 'tigase'
       }
   }

This tells Tigase to operate in a setup mode, and tells the http component to allow login with the username and password admin/tigase. With this you can enter the setup process that is covered in `this section <#webinstall>`__.

There are other options for config-type: ``default``, ``session-manager``, ``connection-managers``, and ``component``. For more information, visit `Config Type <#configType>`__ property description.