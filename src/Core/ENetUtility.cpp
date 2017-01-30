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
		unsigned uport = 0;
		try
		{
			uport = std::stoi(port);
		}
		catch (...)
		{
		}
		return toENetAddress(ip, uport);
	}

	ENetAddress toENetAddress(const std::string & ip, unsigned port)
	{
		ENetAddress addr;
		enet_address_set_host(&addr, ip.c_str());
		addr.port = port;
		return addr;
	}

	bool send(const Packer & packer, ENetPeer * peer, bool reliable)
	{
		ENetPacket * p = enet_packet_create(packer.getData(), packer.getDataSize(), reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED);
		return enet_peer_send(peer, 0, p) == 0;
	}
	bool receive(Unpacker & unpacker, ENetPacket * p)
	{
		unpacker.setData(p->data, p->dataLength);
		return p->dataLength > 0;
	}
	bool send(const Packer & packer, const ENetAddress & addr, ENetSocket socket)
	{
		ENetBuffer buf;
		buf.data = const_cast<void*>(packer.getData());
		buf.dataLength = packer.getDataSize();
		return enet_socket_send(socket, &addr, &buf, 1) > 0;
	}
	bool receive(Unpacker & unpacker, ENetAddress & addr, ENetSocket socket)
	{
		static char buf[ENET_PROTOCOL_MAXIMUM_MTU];
		ENetBuffer buffer;
		buffer.data = buf;
		buffer.dataLength = ENET_PROTOCOL_MAXIMUM_MTU;
		int receivedLength = enet_socket_receive(socket, &addr, &buffer, 1);
		if (receivedLength > 0)
			unpacker.setData(buffer.data, receivedLength);
		return receivedLength > 0;
	}
}
