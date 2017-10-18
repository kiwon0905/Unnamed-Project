#include "ENetUtility.h"

namespace enutil
{
	
	bool toString(const ENetAddress & address, std::string & ans)
	{
		char ip[20];
		if (enet_address_get_host_ip(&address, ip, 20) < 0)
			return false;

		ans = std::string(ip) + ":" + std::to_string(address.port);
		return true;
	}

	bool toENetAddress(const std::string & addr, ENetAddress & ans)
	{
		std::size_t pos = addr.find(":");

		if (pos == std::string::npos)
			return false;

		std::string ip = addr.substr(0, pos);
		unsigned port = 0;

		try
		{
			std::string sport = addr.substr(pos + 1);
			port = std::stoi(sport);
		}
		catch (...)
		{
			return false;
		}
		return toENetAddress(ip, port, ans);
	}

	bool toENetAddress(const std::string & ip, unsigned port, ENetAddress & ans)
	{
		if (enet_address_set_host(&ans, ip.c_str()) < 0)
			return false;
		ans.port = port;
		return true;
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
