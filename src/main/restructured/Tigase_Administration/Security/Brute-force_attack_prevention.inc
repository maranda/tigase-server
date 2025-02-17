Brute-force attack prevention
---------------------------------

Brute-force Prevention is designed to protect Tigase Server against user password guessing. It counts invalid login tries and when it is above limit, it locks login ability for specific time (soft ban). When invalid login counter reaches second level, account will be disabled permanently.

Configuration
^^^^^^^^^^^^^^^^^

Brute-force Prevention is configured by VHost. There is following lis of configuration parameters:

+-------------------------------------+-------------+---------------------------------------------------------------------------+
| ``brute-force-lock-enabled``        | ``boolean`` | Brute Force Prevention Enabled                                            |
+-------------------------------------+-------------+---------------------------------------------------------------------------+
| ``brute-force-lock-after-fails``    | ``long``    | Number of allowed invalid login                                           |
+-------------------------------------+-------------+---------------------------------------------------------------------------+
| ``brute-force-period-time``         | ``long``    | Time [sec] in what failed login tries are counted                         |
+-------------------------------------+-------------+---------------------------------------------------------------------------+
| ``brute-force-disable-after-fails`` | ``long``    | Threshold beyond which account will be permanently disabled               |
+-------------------------------------+-------------+---------------------------------------------------------------------------+
| ``brute-force-lock-time``           | ``long``    | Time [sec] of soft ban (first threshold)                                  |
+-------------------------------------+-------------+---------------------------------------------------------------------------+
| ``brute-force-mode``                | ``string``  | Working mode (see `Working modes <#bruteForcePrevention_WorkingModes>`__) |
+-------------------------------------+-------------+---------------------------------------------------------------------------+

Detailed statistics
~~~~~~~~~~~~~~~~~~~~~~~

By default, in order not to pollute statistics, Brute-Force locker will only provide details about number of locker IPs and JIDs (and total number of locked attempts). In order to have detailed information about IPs and JIDs that has been locked in statistics you should use following configuration:

::

   'sess-man' () {
       'brute-force-locker' () {
           detailedStatistics = false
       }
   }

Working modes
~~~~~~~~~~~~~~~~~~~~~~~

There are three working modes:

-  ``Ip`` - it counts invalid login tries from IP, and locks login ability (soft ban) for IP what reach the threshold

-  ``IpJid`` - it counts tries from IP to specific user account. Soft ban locks ability of login to specific JID from specific IP.

-  ``Jid``- similar to ``IpJid`` but checks only JID. Soft ban locks ability of login to specific JID from all IPs.

.. **Note**::

   Only in modes ``Jid`` and ``IpJid`` account may be permanently disabled.

Permanent ban
~~~~~~~~~~~~~~~~~~~~~~~

In modes ``Jid`` and ``IpJid``, when invalid login counter reach threshold ``brute-force-disable-after-fails``, account status will be set o ``disabled``. To enable it again you should use `Re-Enable User <https://xmpp.org/extensions/xep-0133.html#reenable-users>`__ Ad-hoc Command.