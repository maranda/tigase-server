/*  Package Jabber Server
 *  Copyright (C) 2001, 2002, 2003, 2004, 2005
 *  "Artur Hefczyc" <artur.hefczyc@tigase.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * $Rev$
 * Last modified by $Author$
 * $Date$
 */
package tigase.xmpp.impl;

import java.util.Queue;
import java.util.logging.Logger;
import tigase.server.Packet;
import tigase.server.XMPPServer;
import tigase.xml.Element;
import tigase.xmpp.NotAuthorizedException;
import tigase.xmpp.XMPPProcessor;
import tigase.xmpp.XMPPProcessorIfc;
import tigase.xmpp.XMPPResourceConnection;
import tigase.util.JIDUtils;
import tigase.db.NonAuthUserRepository;

/**
 * XEP-0092: Software Version
 *
 *
 * Created: Tue Mar 21 06:45:51 2006
 *
 * @author <a href="mailto:artur.hefczyc@tigase.org">Artur Hefczyc</a>
 * @version $Rev$
 */
public class JabberIqVersion extends XMPPProcessor
	implements XMPPProcessorIfc {

  private static final Logger log =
    Logger.getLogger("tigase.xmpp.impl.JabberIQVersion");

  protected static final String XMLNS = "jabber:iq:version";
	protected static final String ID = XMLNS;
  protected static final String[] ELEMENTS = {"query"};
  protected static final String[] XMLNSS = {XMLNS};
  protected static final Element[] DISCO_FEATURES =
	{new Element("feature",	new String[] {"var"},	new String[] {XMLNS})};

	public String id() { return ID; }

	public String[] supElements() { return ELEMENTS; }

  public String[] supNamespaces() { return XMLNSS; }

  public Element[] supDiscoFeatures(final XMPPResourceConnection session)
	{ return DISCO_FEATURES; }

	public void process(final Packet packet, final XMPPResourceConnection session,
		final NonAuthUserRepository repo, final Queue<Packet> results) {

		if (session == null) {
			return;
		} // end of if (session == null)

// 		// For some reason Psi sends version request to itself
// 		// I am assuming it wants version of the jabber server
// 		// so let's try pickup this case
// 		try {
// 			final String jid = session.getJID();
// 			if (packet.getFrom().equals(session.getConnectionId())) {
// 				packet.getElement().setAttribute("from", jid);
// 			} // end of if (packet.getFrom().equals(session.getConnectionId()))
// 		} catch (NotAuthorizedException e) {
// 			// Just ignore user doesn't have to be autorized to retrieve
// 			// server version.
// 		} // end of try-catch

		// Maybe it is message to admininstrator:
		String id = session.getDomain();
		if (packet.getElemTo() != null) {
			id = JIDUtils.getNodeID(packet.getElemTo());
		} // end of if (packet.getElemTo() != null)

		// If ID part of user account contains only host name
		// and this is local domain it is message to admin
		if (id == null || id.equals("")
			|| id.equalsIgnoreCase(session.getDomain())
// 			|| (packet.getElemTo() != null && packet.getElemFrom() != null
// 				&& packet.getElemTo().equals(packet.getElemFrom()))
				) {

			StringBuilder reply = new StringBuilder();
			reply.append("<name>" + XMPPServer.NAME + "</name>");
			reply.append(
				"<version>"
				+ XMPPServer.getImplementationVersion()
				+ "</version>");
			reply.append(
				"<os>"
				+ System.getProperty("os.name")
				+ "-" + System.getProperty("os.arch")
				+ "-" + System.getProperty("os.version")
				+ ", " + System.getProperty("java.vm.name")
				+ "-" + System.getProperty("java.vm.version")
				+ "-" + System.getProperty("java.vm.vendor")
				+ "</os>"
									 );
			results.offer(packet.okResult(reply.toString(), 1));
			return;
		}

		try {
			// For all messages coming from the owner of this account set
			// proper 'from' attribute. This is actually needed for the case
			// when the user sends a message to himself.
			if (packet.getFrom().equals(session.getConnectionId())) {
				packet.getElement().setAttribute("from", session.getJID());
			} // end of if (packet.getFrom().equals(session.getConnectionId()))

			// According to JEP-0092 user doesn't need to be logged in to
			// retrieve server version information, so this part is executed
			// after checking if this message is just to local server
			if (id.equals(session.getUserId())) {
				// Yes this is message to 'this' client
				Element elem = packet.getElement().clone();
				Packet result = new Packet(elem);
				result.setTo(session.getConnectionId());
				result.setFrom(packet.getTo());
				results.offer(result);
			} else {
				// This is message to some other client so I need to
				// set proper 'from' attribute whatever it is set to now.
				// Actually processor should not modify request but in this
				// case it is absolutely safe and recommended to set 'from'
				// attribute
				Element result = packet.getElement().clone();
				// According to spec we must set proper FROM attribute
				//				result.setAttribute("from", session.getJID());
				results.offer(new Packet(result));
			} // end of else
		} catch (NotAuthorizedException e) {
			// Do nothing, shouldn't happen....
			log.warning("NotAuthorizedException for packet: "
				+ packet.getStringData());
		} // end of try-catch
	}

} // JabberIqVersion
