#pragma once

class Client;

class Renderer
{
public:
	bool initialize(Client & client);
	void finalize(Client & client);

private:
};