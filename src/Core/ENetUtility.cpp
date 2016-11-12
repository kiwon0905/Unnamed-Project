#include "ENetUtility.h"

namespace enutil
{
	std::string toString(const ENetAddress & address)
	{
		char ip[20];
		enet_address_get_host_ip(&address, ip, 20);
		return std::string(ip) + ":" + std::to_string(address.port);
	}

	ENetAddress toENetAddress(const std::string & addr)
	{
		std::size_t pos = addr.find(":");
		std::string ip = addr.substr(0, pos);
		std::string port = addr.substr(pos + 1);
		return toENetAddress(ip, std::stoi(port));
	}

	ENetAddress toENetAddress(const std::string & ip, unsigned port)
	{
		ENetAddress addr;
		enet_address_set_host(&addr, ip.c_str());
		addr.port = port;
		return addr;
	}

	int send(ENetPeer * peer, const Packet & packet, bool reliable)
	{
		ENetPacket * p = enet_packet_create(packet.getData(), packet.getDataSize(), reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED);
		if (!p)
			return -1;
		return enet_peer_send(peer, 0, p) == 0;
	}

	void receive(ENetPacket * p, Packet & packet)
	{
		packet.clear();
		packet.write(p->data, p->dataLength);
	}

	int send(ENetSocket socket, const ENetAddress & addr, const Packet & packet)
	{
		ENetBuffer buf;
		buf.data = const_cast<void*>(packet.getData());
		buf.dataLength = packet.getDataSize();
		return enet_socket_send(socket, &addr, &buf, 1);
		return 0;
	}

	int receive(ENetSocket socket, ENetAddress & addr, Packet & packet)
	{
		static char buf[ENET_PROTOCOL_MAXIMUM_MTU];
		ENetBuffer buffer;
		buffer.data = buf;
		buffer.dataLength = ENET_PROTOCOL_MAXIMUM_MTU;
		int receivedLength = enet_socket_receive(socket, &addr, &buffer, 1);
		if(receivedLength > 0)
			packet.write(buffer.data, receivedLength);
		return receivedLength;
	}
}
