Licensing
--------------

With the release of v7.1.0, users and commercial clients alike may now be able to register and request a license file from our servers on their own. This process makes it easier for everyone to obtain valid license file when needed. Users who do not wish to register will not be required to register. However, If you are using Tigase ACS or other commercial pieces of software, you will be required to register.

.. Warning::

    Tigase XMPP Server will shut down during license check if no installation-id or license is received within a given period of time.

**Again, Tigase XMPP Server will still be available free under AGPLv3, and free users will not need to register.**

.. Note::

   COMMERCIAL COMPONENTS REQUIRE THE USE OF A LICENSE.

Registering for a License
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There are currently two ways for registering for a license with Tigase commercial products. **The easiest and recommended method is using the built in automatic registration function**. However, you may also register via a web portal if your installation has limitations on network connectivity.

.. _AutomaticLicenceRegistration:

Automatic Registration (recommended)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Once a commercial component is activated on Tigase XMPP Server, the program will then retrieve an *Installation ID* from our servers, and make a file called ``installation-id`` in your ``etc/`` directory including the *Installation ID* for your instance. An installation ID is generated using the complete cluster map and all machines within the same cluster should have the same *Installation ID*. This *Installation ID* will then be sent along with server details to a license server, and appropriate license files will be made in your *tigasedir/etc* directory. When the license is due to be expired, this mechanism will update your license file automatically.

Manual
~~~~~~~~~~~

.. Caution::

   This method should be used only in extreme cases when :ref:`Automatic Registration (recommended)<AutomaticLicenceRegistration>` can’t be used.

If you do not wish to use the automatic method, you may decide to generate a license file using our web portal. Offline installation may obtain *Installation IDs* from our web portal in a three-step process: registration, generating hash, and obtaining license file.

Generating Installation ID

For offline installations, you may obtain an *Installation ID* from this address: https://license.tigase.software/register.

Data Fields:

-  ``Customer name``: Company or user name used to identify machines. Multiple clusters or servers can have the same customer name.

-  ``VHosts``: Comma separated list of VHosts you will be using on this node. NOTE: these fields are case sensitive!

-  ``Legacy license hashes``: Copy the digest hash generated for all legacy licenses - it’s available in the ``etc/tigase-console.log`` after startup (if such licenses are present).

-  ``Captcha question``: Enter the basic math answer for this form to prove you are not a robot.

The next page will provide you with an installation ID like the following:

::

   1TCICGG7K8AS2JSSEVMDA9QOLR4NVLJSR

Edit your ``config.tdsl`` file and add your installation-id

.. code::

   'installation-id' = '1TCICGG7K8AS2JSSEVMDA9QOLR4NVLJSR'

Note that the ``installation-id`` file will be made automatically once the license file is installed and verified by the server.

Obtaining a Server Code

Once you have the *Installation ID*, you will need to generate a server code. This can be done by accessing the admin UI page and navigating to the License section. Once there, click on Retrieve code for license. Select the component you wish to generate a code for and click Submit. You will see a fields with installation-id, module, VHosts filled out based on your server’s configuration. Copy the contents of the Code field and proceed to the next section.

Obtaining license file

Open a new browser and navigate to this address: https://license.tigase.software/retrieve once there, paste the generated code from the last step in the field and click submit. Afterwards you will be prompted to download a license file, place this file in your *etc/* folder and restart your server, your license is now activated and installed on your server.

**If you are provided a manually produced license, you will need to place it in the same ``etc/`` directory with the name ``<component_name>.license`` (e.g.: ``etc/acs.license``)**

What happens if I do not use a license file or it is expired?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tigase permits commercial products to be used without a license, but a validation process must complete otherwise the server will shutdown. Within the first hour of runtime, Tigase will check for the presence and validity of the license file. If none is found, or it is invalid or expired the server will then contact Tigase master server in order to obtain a valid one.

Communications will be made to license.tigase.software over https (port 443) to verify the license or download a new one.

Demo mode
^^^^^^^^^^^

If no valid license can be found, Tigase will revert to a demonstration mode. Most functions will be available and usable, but with a caveat. Statistics from that server will be sent to https://stats.tigase.software about your server and it’s usage. Details are in the next section. If this information cannot be sent, the server will assume unauthorized use and will shut down.

Statistics Sent
~~~~~~~~~~~~~~~~~~~

Statistics of your server may be sent to Tigase server’s if the all of following happens:

-  You are using commercial Tigase components.

-  You have registered an ``installation-id``.

-  You do not have a current license to run Tigase commercial components.

If these conditions exist, statistics will be sent to our servers and a warning will be posted in your logs. The following is an example of what information will be sent.

.. Note::

   The text below has been better formatted for readability, but does not reflect the actual text being sent to Tigase.

.. code:: xml

   <statistics version="1">
       <domain>xmppserver</domain>
       <timestamp>2016-06-23T17:16:24.777-0700</timestamp>
       <vhosts>
           <item>vhost1.xmppserver.com</item>
       </vhosts>
       <uptime>308833</uptime>
       <heap>
           <used>30924376</used>
           <max>1426063360</max>
       </heap>
       <cluster>
           <nodes_count>1</nodes_count>
       </cluster>
       <users>
           <online>0</online>
           <active>0</active>
           <max_today>1</max_today>
           <max_yesterday>0</max_yesterday>
       </users>
       <additional_data>
           <components>
               <cmpInfo>
                   <name>amp</name>
                   <title>Tigase XMPP Server</title>
                   <version>7.1.0-SNAPSHOT-b4226/5e7210f6 (2016-06-01/23:15:52)</version>
                   <class>tigase.cluster.AmpComponentClustered</class>
               </cmpInfo>

               <cmpInfo>
                   <name>bosh</name>
                   <title>Tigase XMPP Server</title>
                   <version>7.1.0-SNAPSHOT-b4226/5e7210f6 (2016-06-01/23:15:52)</version>
                   <class>tigase.cluster.BoshConnectionClustered</class>
               </cmpInfo>

               <cmpInfo>
                   <name>c2s</name>
                   <title>Tigase XMPP Server</title>
                   <version>7.1.0-SNAPSHOT-b4226/5e7210f6 (2016-06-01/23:15:52)</version>
                   <class>tigase.cluster.ClientConnectionClustered</class>
               </cmpInfo>

               <cmpInfo>
                   <name>cl-comp</name>
                   <title>Tigase XMPP Server</title>
                   <version>7.1.0-SNAPSHOT-b4226/5e7210f6 (2016-06-01/23:15:52)</version>
                   <class>tigase.cluster.ClusterConnectionManager</class>
               </cmpInfo>

               <cmpInfo>
                   <name>eventbus</name>
                   <title>Tigase XMPP Server</title>
                   <version>7.1.0-SNAPSHOT-b4226/5e7210f6 (2016-06-01/23:15:52)</version>
                   <class>tigase.disteventbus.component.EventBusComponent</class>
               </cmpInfo>

               <cmpInfo>
                   <name>http</name>
                   <title>Tigase HTTP API component: Tigase HTTP API component</title>
                   <version>1.2.0-SNAPSHOT-b135/27310f9b-7.1.0-SNAPSHOT-b4226/5e7210f6 (2016-06-01/23:15:52)</version>
                   <class>tigase.http.HttpMessageReceiver</class>
               </cmpInfo>

               <cmpInfo>
                   <name>monitor</name>
                   <title>Tigase XMPP Server</title>
                   <version>7.1.0-SNAPSHOT-b4226/5e7210f6 (2016-06-01/23:15:52)</version>
                   <class>tigase.monitor.MonitorComponent</class>
               </cmpInfo>

               <cmpInfo>
                   <name>muc</name>
                   <title>Tigase ACS - MUC Component</title>
                   <version>1.2.0-SNAPSHOT-b62/74afbb91-2.4.0-SNAPSHOT-b425/d2e26014</version>
                   <class>tigase.muc.cluster.MUCComponentClustered</class>
                   <cmpData>
                       <MUCClusteringStrategy>class tigase.muc.cluster.ShardingStrategy</MUCClusteringStrategy>
                   </cmpData>
               </cmpInfo>

               <cmpInfo>
                   <name>pubsub</name>
                   <title>Tigase ACS - PubSub Component</title>
                   <version>1.2.0-SNAPSHOT-b65/1c802a4c-3.2.0-SNAPSHOT-b524/892f867f</version>
                   <class>tigase.pubsub.cluster.PubSubComponentClustered</class>
                   <cmpData>
                       <PubSubClusteringStrategy>class tigase.pubsub.cluster.PartitionedStrategy</PubSubClusteringStrategy>
                   </cmpData>
               </cmpInfo>

               <cmpInfo>
                   <name>s2s</name>
                   <title>Tigase XMPP Server</title>
                   <version>7.1.0-SNAPSHOT-b4226/5e7210f6 (2016-06-01/23:15:52)</version>
                   <class>tigase.server.xmppserver.S2SConnectionManager</class>
               </cmpInfo>

               <cmpInfo>
                   <name>sess-man</name>
                   <title>Tigase XMPP Server</title>
                   <version>7.1.0-SNAPSHOT-b4226/5e7210f6 (2016-06-01/23:15:52)</version>
                   <class>tigase.cluster.SessionManagerClustered</class>
                   <cmpData>
                       <ClusteringStrategy>class tigase.server.cluster.strategy.OnlineUsersCachingStrategy
                       </ClusteringStrategy>
                   </cmpData>
               </cmpInfo>

               <cmpInfo>
                   <name>ws2s</name>
                   <title>Tigase XMPP Server</title>
                   <version>7.1.0-SNAPSHOT-b4226/5e7210f6 (2016-06-01/23:15:52)</version>
                   <class>tigase.cluster.WebSocketClientConnectionClustered</class>
               </cmpInfo>

               <cmpInfo>
                   <name>vhost-man</name>
                   <title>Tigase XMPP Server</title>
                   <version>7.1.0-SNAPSHOT-b4226/5e7210f6 (2016-06-01/23:15:52)</version>
                   <class>tigase.vhosts.VHostManager</class>
               </cmpInfo>

               <cmpInfo>
                   <name>stats</name>
                   <title>Tigase XMPP Server</title>
                   <version>7.1.0-SNAPSHOT-b4226/5e7210f6 (2016-06-01/23:15:52)</version>
                   <class>tigase.stats.StatisticsCollector</class>
               </cmpInfo>

               <cmpInfo>
                   <name>cluster-contr</name>
                   <title>Tigase XMPP Server</title>
                   <version>7.1.0-SNAPSHOT-b4226/5e7210f6 (2016-06-01/23:15:52)</version>
                   <class>tigase.cluster.ClusterController</class>
               </cmpInfo>
           </components>

           <unlicencedComponenents>
               <ComponentAdditionalInfo name=&quot;acs&quot;/>
           </unlicencedComponenents>
       </additional_data>
   </statistics>

Unauthorized use
^^^^^^^^^^^^^^^^^^^^

Tigase will consider itself unauthorized if the following conditions are met:

-  if Tigase XMPP Server does not have a valid license file and

-  cannot contact the licensing server to obtain installation id and attached licenses.

Then the program will then attempt to send statistics.

-  if unable to sent statistics the server after a random number of retries.

-  if these retries are not successful within 10 attempts, the server will then shutdown.

If you are experiencing this condition, please contact Tigase.

Manual mode
^^^^^^^^^^^^^^^^

If you cannot open communication to ``stats.tigase.software`` or ``license.tigase.software`` over the required ports (https over port 443), you may request to use manual mode. Manual mode requires Tigase to create a license file to be used on your machine locally. This must be placed in the same folder as the above information, and the license check system will not seek communication unless the license is invalid or expired.
