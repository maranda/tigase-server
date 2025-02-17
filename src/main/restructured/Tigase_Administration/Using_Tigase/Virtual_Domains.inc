.. _addManageDomain:

Add and Manage Domains (VHosts)
------------------------------------

Tigase XMPP Server offers an easy to use and very flexible way to add and manage domains hosted on installation (vhosts).

There are two ways of managing domains you host on your server:

-  using web-based admin management console - :ref:`Admin UI<usingAdminUI>`

-  using XMPP ad-hoc commands by XMPP client, ie. `Psi <http://psi-im.org/>`__

.. Note::

   To use any of those ways, you need to be an administrator of the server, which means that you have a XMPP account created on this XMPP server and your account JID is added to :ref:`the list of the administrators<admins>` in the Tigase XMPP Server configuration file.

.. _usingAdminUI:

Using Admin UI
^^^^^^^^^^^^^^^^^^

First, you need to open Admin UI web page. By default Admin UI is enabled and available at the port ``8080`` at path ``/admin/`` on the XMPP server. Assuming that your are logged on the same machine which hosts Tigase XMPP Server, it will be available at http://localhost:8080/admin/.

When you will be prompted for username and password to login to the Admin UI please fill username with full JID of your XMPP admin account and fill password field with password for this account. When you submit correct credentials you will get access to the Admin UI and Tigase XMPP Server configuration and management web-based interface.

Adding a new domain
~~~~~~~~~~~~~~~~~~~~~~~~

To add a new domain you need to open ``Configuration`` section of the Admin UI (by clicking on ``Configuration`` label and then selecting ``Add new item`` position which mentions ``vhost-man``.

|adminui vhost add item button|

After doing that, you will be presented with a form which you need to fill in. This form allows you to pass ``Domain name`` to add and other options (some of the are advanced options).

|adminui vhost add item form|

.. Tip::

   All options with exception of ``Domain name`` may be changed later on by modifying vhost settings.

When you will be ready, please submit the form using button below the form. As a result you will be presented with a result of this operation. If it was successful it show ``Operation successful`` message and if something was not OK, it will display an error to help you fix this issue which you encountered.

Modifying domain settings
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Modifying a domain settings is very similar to adding a new domain. You need to open ``Configuration`` section of the Admin UI and then select ``Update item configuration`` position which mentions ``vhost-man``.

|adminui vhost update item button|

You will be presented with a list of domains hosted on this Tigase XMPP Server installation. From them you should choose the one for which you wish to modify settings.

|adminui vhost update item domains list|

After submitting this selection, you will be presented with a the same form as the one used during adding a new domain. It presents configuration options for this domain and currently used values.

|adminui vhost update item form|

Now you should adjust them as you wish and submit this form using the button below the form.

As a result you will be presented with a result of this operation. If it was successful it show ``Operation successful`` message and if something was not OK, it will display an error to help you fix this issue which you encountered.

Removing a domain
~~~~~~~~~~~~~~~~~~~~~

Removing a hosted domain from the Tigase XMPP Server installation is quite simple as well. You need to open ``Configuration`` section of the Admin UI and then select ``Remove an item`` position which mentions ``vhost-man``.

|adminui vhost remove item button|

You will be presented with a list of domains hosted on this Tigase XMPP Server installation. From them you should select the one which should be removed.

|adminui vhost remove item domains list|

After submitting your selection, Tigase XMPP Server will try to remove this domain from the list of hosted domains and will present you with the result. If it was successful it show ``Operation successful`` message and if something was not OK, it will display an error to help you fix this issue which you encountered.

Using ad-hoc commands
^^^^^^^^^^^^^^^^^^^^^^

For everybody interested in using our service to host their own XMPP domain we have good news! You do not have to ask an administrator to add your domain or add users for your domain anymore. You can do it on your own.

Please note, this is very new stuff. Something may go wrong or may not be polished. Please report any problems, notices or suggestions.

This is the guide to walk you through the new functions and describes how to add a new domain and new users within your domain.

You can do everything from your XMPP client or you can use our web application that allows you to connect to the service and execute admin commands. I recommend `Psi <http://psi-im.org/>`__ because of its excellent support for parts of the XMPP protocol which are used for domains and user management. You may use other clients as well, but we can only offer support and help if you use Psi client.

Secondly, you need an account on the server. This is because all the commands and features described here are available to local users only. Therefore, if you do not have a registered domain with us yet, please go ahead and register an account on the website either the `Tigase.IM <http://www.tigase.im/>`__ or `Jabber.Today <http://jabber.today/>`__.

Adding a New Domain
~~~~~~~~~~~~~~~~~~~~~

Once you register an account on one of the websites, connect to the XMPP server using the account on the Psi client. We will be using the following account: green@tigase.im which is this guide.

When you are ready right click on the account name in Psi roster window to bring up context menu. Select **Service Discovery** element.

|service disco menu|

A new windows pops up as in the example on the right. The service discovery window is where all the stuff installed on XMPP service should show up. Most of elements on the list are well known transports, MUC and PubSub components. The new stuff on the list, which we are interested in, are 2 elements: **VHost Manager** and **Session Manager**.

|service disco window vhost|

**VHost Manager** component in Tigase is responsible for managing and controlling virtual hosts on the installation. It provides virtual hosts information to all other parts of the system and also allows you to add new hosts and remove/update existing virtual hosts.

**Session Manager** component in Tigase is responsible for managing users. In most cases online users but it can also perform some actions on user repository where all user data is stored.

Select **VHost Manager** and double click on it. A new windows shows up (might be hidden behind the service discovery window). The window contains another menu with a few items: **Add…​, Remove…​** and **Update…​** . These are for adding, removing and updating VHost information. For now, just select the first element **Add…​.**

|command menu add vhost|

Click **Execute** and you get a new window where you can enter all of your VHost configuration details. All fields should be self explanatory. Leave a blank field for **Other parameters** for now. **Owner** is you, that is Jabber ID which controls the domain and can change the domain configuration settings or can remove the domain from the service. **Administrators** field can be left blank or can contain comma separated list of Jabber IDs for people who can manage users within the domain. You do not need to add your user name to the list as Owners can always manage users for the domain.

|add vhost window|

When you are ready click the **Finish** button. All done, hopefully. You can get either a window confirming everything went well or a window printing an error message if something went wrong. What can be wrong? There are some restrictions I decided to put on the service to prevent abuse. One of the restrictions is the maximum number of domains a user can register for himself which is **25** right now. Another restriction is that the domain which you add must have a valid DNS entry pointing to our service. The XMPP guide describes all the details about DNS settings. Please refer to these instructions if you need more details.

Adding a New User
~~~~~~~~~~~~~~~~~~~~~

Adding a new user process is quite similar, almost identical to adding a new domain. This time, however we have to select **Session Manager** in the service discovery window.

|service disco window sm|

Double click on the **Session Manager** and a window with SM’s commands list shows up. Right now, there is only one command available to domain administrators - **Add user**. I am going to make available more commands in the future and I am waiting for your suggestions.

|command menu add user|

If you click **Execute** a window presented on the left shows up. Fill all fields accordingly and press **Finish**.

|add user window|

If everything went well you have just added a new user and you should get a window confirming successful operation. If something went wrong, a window with an error message should show up. Possible errors may be you tried to add a user which is already present, or you may have tried to add a user for a domain to which you do not have permission or to non-existen domain.

SSL Certificate Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SSL Certificate Management has been implemented, and certificates can be manipulated when in a .pem form. For more details, see `Creating and Loading the Server Certificate in pem Files <#certspem>`__ section of documentation for more information.

.. |adminui vhost add item button| image:: /images/admin/adminui_vhost_add_item_button.png
.. |adminui vhost add item form| image:: /images/admin/adminui_vhost_add_item_form.png
.. |adminui vhost update item button| image:: /images/admin/adminui_vhost_update_item_button.png
.. |adminui vhost update item domains list| image:: /images/admin/adminui_vhost_update_item_domains_list.png
.. |adminui vhost update item form| image:: /images/admin/adminui_vhost_update_item_form.png
.. |adminui vhost remove item button| image:: /images/admin/adminui_vhost_remove_item_button.png
.. |adminui vhost remove item domains list| image:: /images/admin/adminui_vhost_remove_item_domains_list.png
.. |service disco menu| image:: /images/admin/service_disco_menu.png
.. |service disco window vhost| image:: /images/admin/service_disco_window_vhost.png
.. |command menu add vhost| image:: /images/admin/command_menu_add_vhost.png
.. |add vhost window| image:: /images/admin/add_vhost_window.png
.. |service disco window sm| image:: /images/admin/service_disco_window_sm.png
.. |command menu add user| image:: /images/admin/command_menu_add_user.png
.. |add user window| image:: /images/admin/add_user_window.png
