Tigase Load Balancing
----------------------------

Tigase includes load balancing functionality allowing users to be redirected to the most suitable cluster node. Functionality relies on a see-other-host XMPP stream error message. The basic principle behind the mechanism is that user will get redirect if the host returned by the implementation differ from the host to which user currently tries to connect. It is required that the user JID to be known for the redirection to work correctly.

Available Implementations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tigase implementation is, as usual, extensible and allows for different, pluggable redirection strategies that implement the ``SeeOtherHostIfc`` interface.

Currently there are three strategies available:

-  ``SeeOtherHost`` - most basic implementation returning either single host configured in ``config.tdsl`` file or name of the current host;

-  ``SeeOtherHostHashed`` (default) - default implementation for cluster environment of SeeOtherHostIfc returning redirect host based on the hash value of the user’s JID; list of the available nodes from which a selection would be made is by default composed and reflects all connected nodes, alternatively hosts list can be configured in the config.tdsl;

-  ``SeeOtherHostDB`` - extended implementation of SeeOtherHost using redirect information from database in the form of pairs ``user_id`` and ``node_id`` to which given user should be redirected.

-  ``SeeOtherHostDualIP`` - matches internal Tigase cluster nodes against the lookup table to provide relevant redirection hostname/IP (by default internal Tigase tig_cluster_nodes table will be used)

Configuration Options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The most basic configuration is related to the choice of actual redirection implementation by declaring class for each connector:

.. code::

   bosh {
       seeOtherHost (class: <value>) {}
   }
   c2s {
       seeOtherHost (class: <value>) {}
   }
   ws2s {
       seeOtherHost (class: <value>) {}
   }

Possible values are:

-  ``tigase.server.xmppclient.SeeOtherHost``

-  ``tigase.server.xmppclient.SeeOtherHostHashed``

-  ``tigase.server.xmppclient.SeeOtherHostDB``

-  ``tigase.server.xmppclient.SeeOtherHostDualIP``

-  ``none`` - disables redirection

All options are configured on a per-connection-manager basis, thus all options need to be prefixed with the corresponding connection manager ID, i.e. c2s, bosh or ws; we will use c2s in the examples:

.. code::

   c2s {
       'cm-see-other-host' {
           'default-host' = 'host1;host2;host3'
           'phases' = [ 'OPEN', 'LOGIN' ]
       }
   }

-  ``'default-host' = 'host1;host2;host3'`` - a semicolon separated list of hosts to be used for redirection.

-  ``'phases' = []`` - an array of phases in which redirection should be active, currently possible values are:

   -  ``OPEN`` which enables redirection during opening of the XMPP stream;

   -  ``LOGIN`` which enables redirection upon authenticating user session;

By default redirection is currently enabled only in the ``OPEN`` phase.

SeeOtherHostDB
~~~~~~~~~~~~~~~~~~~

For ``SeeOtherHostDB`` implementation there are additional options:

.. code::

   c2s {
       'cm-see-other-host' {
           'db-url' = 'jdbc:mysqk://localhost/username?,password?'
           'get-all-query-timeout' = '10'
       }
   }

-  ``db-url`` - a JDBC connection URI which should be used to query redirect information; if not configured the default ``dataSource`` will be used;

-  ``get-host-query`` - a SQL query which should return redirection hostname;

-  ``get-all-data-query`` - a SQL helper query which should return all redirection data from database;

-  ``get-all-query-timeout`` - allows to set timeout for executed queries.

SeeOtherHostDualIP
~~~~~~~~~~~~~~~~~~~

This mechanisms matches internal Tigase cluster nodes against the lookup table to provide matching and relevant redirection hostname/IP. By default internal Tigase ``tig_cluster_nodes`` table is used (and appropriate repository implementation will be used).

To enable this redirection mechanism following configuration / class should be used. Note that for global use, all connection managers must have the same class defined. You can define each connection manager individually.

.. code::

   bosh {
       seeOtherHost (class: tigase.server.xmppclient.SeeOtherHostDualIP) {}
   }
   c2s {
       seeOtherHost (class: tigase.server.xmppclient.SeeOtherHostDualIP) {}
   }
   ws2s {
       seeOtherHost (class: tigase.server.xmppclient.SeeOtherHostDualIP) {}
   }

It offers following configuration options:

-  ``data-source`` - configuration of the source of redirection information - by default internal Tigase ``tig_cluster_nodes`` table will be used (and appropriate repository implementation will be used); alternatively it’s possible to use ``eventbus`` source;

-  ``db-url`` - a JDBC connection URI which should be used to query redirect information; if not configured ``user-db-uri`` will be used;

-  ``get-all-data-query`` - a SQL helper query which should return all redirection data from database;

-  ``get-all-query-timeout`` - allows to set timeout for executed queries;

-  ``fallback-redirection-host`` - if there is no redirection information present (i.e. secondary hostname is not configured for the particular node) redirection won’t be generated; with this it’s possible to configure fallback redirection address.

All options are configured or on per-component basis:

.. code::

   <connector> {
       'cm-see-other-host' {
           'data-source' = '<class implementing tigase.server.xmppclient.SeeOtherHostDualIP.DualIPRepository>'
           'db-url' = 'jdbc:<database>://<uri>'
           'fallback-redirection-host' = '<hostname>'
           'get-all-data-query' = 'select * from tig_cluster_nodes'
           'get-all-query-timeout' = 10
       }
   }

EventBus as a source of information

It’s possible to utilize EventBus and internal Tigase events as a source of redirection data. In order to do that ``eventbus-repository-notifications`` needs to be enabled in ClusterConnectionManager:

.. code::

   'cl-comp' {
       'eventbus-repository-notifications' = true
   }


Auxiliary setup options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Enforcing redirection
~~~~~~~~~~~~~~~~~~~~~~

It’s possible to enforce redirection of connections on the particular port of connection manager with ``force-redirect-to`` set to Integer with the following general setting option:

.. code::

   <connection_manager> {
       connections {
           <listening_port> {
               'force-redirect-to' = <destination_port>
           }
       }
   }

for example, enable additional port ``5322`` for ``c2s`` connection manager and enforce all connections to be redirected to port ``5222`` (it will utilize hostname retrieved from ``SeeOtherHost`` implementation and will be only used when such value is returned):

.. code::

   c2s {
       connections {
           ports = [ 5222, 5322 ]
           5322 {
               'force-redirect-to' = 5222
               socket = 'plain'
               type = 'accept'
           }
       }
   }


Configuring hostnames
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To fully utilize ``SeeOtherHostDualIP`` setup in automated fashion it’s now possible to provide both primary (*internal*) and secondary (*external*) hostname/IP (they need to be correct, ``InetAddress.getByName( property );`` will be used to verify correctness). It can be done via JVM properties ``tigase-primary-address`` and ``tigase-secondary-address``. You can also utilize different implementation of DNS resolver by providing class implementing ``tigase.util.DNSResolverIfc`` interface as value to ``resolver-class`` property. Those properties can be set via ``etc/tigase.conf`` (uncommenting following lines, or manually exposing in environment):

.. code:: bash

   DNS_RESOLVER=" -Dresolver-class=tigase.util.DNSResolverDefault "

   INTERNAL_IP=" -Dtigase-primary-address=hostname.local "
   EXTERNAL_IP=" -Dtigase-secondary-address=hostname "

or in the ``etc/config.tdsl`` (they will be converted to JVM properties):

.. code::

   'dns-resolver' {
       'tigase-resolver-class' = 'tigase.util.DNSResolverDefault'
       'tigase-primary-address' = 'hostname.local'
       'tigase-secondary-address' = 'hostname'
   }