/*
 * Tigase XMPP Server - The instant messaging server
 * Copyright (C) 2004 Tigase, Inc. (office@tigase.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. Look for COPYING file in the top folder.
 * If not, see http://www.gnu.org/licenses/.
 */
package tigase.xmpp.impl;

import tigase.db.NonAuthUserRepository;
import tigase.kernel.beans.Bean;
import tigase.kernel.beans.config.ConfigField;
import tigase.server.Iq;
import tigase.server.Packet;
import tigase.server.Presence;
import tigase.server.xmppsession.SessionManager;
import tigase.xml.Element;
import tigase.xmpp.*;

import java.util.Iterator;
import java.util.Map;
import java.util.Queue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Class responsible for queuing packets (usable in connections from mobile clients - power usage optimization) version
 * 1
 *
 * @author andrzej
 */
@Bean(name = MobileV1.ID, parent = SessionManager.class, active = false)
public class MobileV1
		extends XMPPProcessor
		implements XMPPProcessorIfc, ClientStateIndication.Logic {

	protected static final String ID = "mobile_v1";
	// default values
	private static final int DEF_MAX_QUEUE_SIZE_VAL = 50;
	private static final long DEF_MAX_TIMEOUT_VAL = 6 * 60 * 1000;
	private static final Logger log = Logger.getLogger(MobileV1.class.getCanonicalName());
	private static final String MAX_QUEUE_SIZE_KEY = "max-queue-size";
	private static final String MAX_TIMEOUT_KEY = "max-timeout";
	private static final String MOBILE_EL_NAME = "mobile";
	private static final String XMLNS = "http://tigase.org/protocol/mobile#v1";
	private static final String[][] ELEMENT_PATHS = {{Iq.ELEM_NAME, MOBILE_EL_NAME}};
	private static final String[] XMLNSS = {XMLNS};
	private static final String TIMEOUT_KEY = ID + "-timeout";
	private static final Element[] SUP_FEATURES = {
			new Element(MOBILE_EL_NAME, new String[]{"xmlns"}, new String[]{XMLNS})};
	private static final String QUEUE_KEY = ID + "-queue";

	// keys
	private static final String LAST_TRANSFER_KEY = ID + "-last-transfer";

	@ConfigField(desc = "Max queue size", alias = MAX_QUEUE_SIZE_KEY)
	private int maxQueueSize = DEF_MAX_QUEUE_SIZE_VAL;
	@ConfigField(desc = "Max timeout", alias = MAX_TIMEOUT_KEY)
	private long maxTimeout = DEF_MAX_TIMEOUT_VAL;

	@Override
	public String id() {
		return ID;
	}

	@Override
	public void process(final Packet packet, final XMPPResourceConnection session, final NonAuthUserRepository repo,
						final Queue<Packet> results, final Map<String, Object> settings) {
		if (session == null) {
			return;
		}
		if (!session.isAuthorized()) {
			try {
				results.offer(
						Authorization.NOT_AUTHORIZED.getResponseMessage(packet, "Session is not yet authorized.", false));
			} catch (PacketErrorTypeException ex) {
				log.log(Level.FINEST, "ignoring packet from not authorized session which is already of type error");
			}

			return;
		}
		try {
			StanzaType type = packet.getType();

			switch (type) {
				case set:
					Element el = packet.getElement().getChild(MOBILE_EL_NAME);
					String valueStr = el.getAttributeStaticStr("enable");

					// if value is true queuing will be enabled
					boolean value = (valueStr != null) && ("true".equals(valueStr) || "1".equals(valueStr));

					if (el.getAttributeStaticStr("timeout") != null) {

						// we got timeout so we should set it for this session
						long timeout = Long.parseLong(el.getAttributeStaticStr("timeout"));

						setTimeout(session, timeout);
					}

					if (value) {
						activate(session, results);
					} else {
						deactivate(session, results);
					}

					results.offer(packet.okResult((Element) null, 0));

					break;

				default:
					results.offer(
							Authorization.BAD_REQUEST.getResponseMessage(packet, "Mobile processing type is incorrect",
																		 false));
			}
		} catch (PacketErrorTypeException ex) {
			Logger.getLogger(MobileV1.class.getName()).log(Level.SEVERE, null, ex);
		}
	}

	@Override
	public String[][] supElementNamePaths() {
		return ELEMENT_PATHS;
	}

	@Override
	public String[] supNamespaces() {
		return XMLNSS;
	}

	@Override
	public Element[] supStreamFeatures(XMPPResourceConnection session) {
		if (session == null) {
			return null;
		}
		if (!session.isAuthorized()) {
			return null;
		}

		return SUP_FEATURES;
	}

	@Override
	public void activate(XMPPResourceConnection session, Queue<Packet> results) {
		if (session.getSessionData(QUEUE_KEY) == null) {
			session.putSessionDataIfAbsent(QUEUE_KEY, new LinkedBlockingQueue<Packet>());
		}
		session.putSessionData(XMLNS, true);
	}

	@Override
	public void deactivate(XMPPResourceConnection session, Queue<Packet> results) {
		session.putSessionData(XMLNS, false);

		flushQueue(session, results);
	}

	@Override
	@SuppressWarnings("unchecked")
	public void filter(Packet _packet, XMPPResourceConnection sessionFromSM, NonAuthUserRepository repo,
					   Queue<Packet> results) {
		if ((sessionFromSM == null) || !sessionFromSM.isAuthorized() || (results == null) || (results.size() == 0)) {
			return;
		}
		for (Iterator<Packet> it = results.iterator(); it.hasNext(); ) {
			Packet res = it.next();

			// check if packet contains destination
			if ((res == null) || (res.getPacketTo() == null)) {
				if (log.isLoggable(Level.FINEST)) {
					log.finest("packet without destination");
				}

				continue;
			}

			// get parent session to look up for connection for destination
			XMPPSession parentSession = sessionFromSM.getParentSession();
			if (parentSession == null) {
				if (log.isLoggable(Level.FINEST)) {
					log.log(Level.FINEST, "no session for destination {0} for packet {1} - missing parent session",
							new Object[]{res.getPacketTo().toString(), res.toString()});
				}
				continue;
			}

			// get resource connection for destination
			XMPPResourceConnection session = parentSession.getResourceForConnectionId(res.getPacketTo());

			if (session == null) {
				if (log.isLoggable(Level.FINEST)) {
					log.log(Level.FINEST, "no session for destination {0} for packet {1}",
							new Object[]{res.getPacketTo().toString(), res.toString()});
				}

				// if there is no session we should not queue
				continue;
			}

			// if queue is not enabled we do nothing
			if (!isQueueEnabled(session)) {
				if (log.isLoggable(Level.FINEST)) {
					log.finest("queue is no enabled");
				}

				flushQueue(session, results);

				continue;
			}

			Queue<Packet> queue = (Queue<Packet>) session.getSessionData(QUEUE_KEY);

			// lets check if packet should be queued
			if (filter(session, res, queue)) {
				if (log.isLoggable(Level.FINEST)) {
					log.log(Level.FINEST, "queuing packet = {0}", res.toString());
				}
				it.remove();
				if (queue.size() > maxQueueSize) {
					if (log.isLoggable(Level.FINEST)) {
						log.finest("sending packets from queue (OVERFLOW)");
					}

					Packet p;

					while ((p = queue.poll()) != null) {
						try {
							// setting destination for packet in case if
							// stream was resumed and connId changed
							p.setPacketTo(session.getConnectionId());
							results.offer(p);
						} catch (NoConnectionIdException ex) {
							log.log(Level.FINEST, "should not happen, as connection is ready", ex);
						}
					}
				}
			}
		}
	}

	public boolean filter(XMPPResourceConnection session, Packet res, Queue<Packet> queue) {
		if (log.isLoggable(Level.FINEST)) {
			log.log(Level.FINEST, "checking if packet should be queued {0}", res.toString());
		}
		if (res.getElemName() != Presence.ELEM_NAME) {
			if (log.isLoggable(Level.FINEST)) {
				log.log(Level.FINEST, "ignoring packet, packet is not presence:  {0}", res.toString());
			}

			return false;
		}

		StanzaType type = res.getType();

		if ((type != null) && (type != StanzaType.unavailable) && (type != StanzaType.available)) {
			return false;
		}
		if (log.isLoggable(Level.FINEST)) {
			log.log(Level.FINEST, "queuing packet {0}", res.toString());
		}
		queue.offer(res);

		return true;
	}

	protected void flushQueue(XMPPResourceConnection session, Queue<Packet> results) {
		Queue<Packet> queue = (Queue<Packet>) session.getSessionData(QUEUE_KEY);

		if ((queue != null) && !queue.isEmpty()) {
			if (log.isLoggable(Level.FINEST)) {
				log.finest("sending packets from queue (DISABLED)");
			}
			Packet p;
			while ((p = queue.poll()) != null) {
				try {
					// setting destination for packet in case if
					// stream was resumed and connId changed
					p.setPacketTo(session.getConnectionId());
					results.offer(p);
				} catch (NoConnectionIdException ex) {
					log.log(Level.FINEST, "should not happen, as connection is ready", ex);
				}
			}
		}
	}

	/**
	 * Check if queuing is enabled
	 *
	 */
	protected boolean isQueueEnabled(XMPPResourceConnection session) {
		Boolean enabled = (Boolean) session.getSessionData(XMLNS);

		return (enabled != null) && enabled;
	}

	/**
	 * Check timeout for queue
	 *
	 */
	protected boolean isTimedOut(XMPPResourceConnection session) {
		Long lastAccessTime = (Long) session.getSessionData(LAST_TRANSFER_KEY);

		if (lastAccessTime == null) {
			return true;
		}
		if (lastAccessTime + getTimeout(session) < System.currentTimeMillis()) {
			return true;
		}

		return false;
	}

	/**
	 * Update last send time
	 *
	 */
	protected void updateLastAccessTime(XMPPResourceConnection session) {
		session.putSessionData(LAST_TRANSFER_KEY, System.currentTimeMillis());
	}

	/**
	 * Get timeout used for session queue
	 *
	 */
	private long getTimeout(XMPPResourceConnection session) {
		Long timeout = (Long) session.getSessionData(TIMEOUT_KEY);

		if (timeout == null) {
			return maxTimeout;
		}

		return timeout;
	}

	/**
	 * Set timeout for session queue
	 *
	 */
	private void setTimeout(XMPPResourceConnection session, long timeout) {
		if (timeout == 0) {
			session.removeSessionData(TIMEOUT_KEY);
		} else {
			session.putSessionData(TIMEOUT_KEY, timeout);
		}
	}
}

