// std
#include <thread>

// RedFox
#include <RF/log.hpp>
#include <RF/net/client.hpp>

enum class msg_t : std::uint32_t
{
	Ping,
	PingAll
};

bool process_running { true };

void on_terminate()
{
	process_running = false;
}

int main()
{
	asio::io_context io;
	RF::net::client::client_m client(io, "127.0.0.1", 5503, "TestClient");

	client.on_receive = [](const RF::net::message &msg)
	{
		auto id = static_cast<msg_t>(msg.id);
		switch (id)
		{
			case msg_t::Ping:
			{
				RF::logf::info("Received Ping from server");
				break;
			}

			default:
			{
				RF::logf::warn("Received unknown message (<0>)", (std::uint32_t)id);
				break;
			}
		}
	};

	client.on_disconnect = []
	{
		RF::logf::warn("Disconnected from server");
		on_terminate();
	};

	client.on_server_timeout = []
	{
		RF::logf::error("Server timed out");
		on_terminate();
	};

	std::thread input_thread([&client]()
	{
		for (;;)
		{
			char c;
			std::cin >> c;

			if (c == 'a')
			{
				RF::logf::info("Sending Ping to server");

				RF::net::message msg;
				msg.id = static_cast<std::uint32_t>(msg_t::Ping);
				msg.payload = {};

				client.send(msg);
			}
			else if (c == 'm')
			{
				RF::logf::info("Sending PingAll to server");

				RF::net::message msg;
				msg.id = static_cast<std::uint32_t>(msg_t::PingAll);
				msg.payload = {};

				client.send(msg);
			}
			else if (c == 'q')
			{
				client.disconnect();
				break;
			}
		}
	});

	while (process_running)
	{
		client.process();
		io.poll();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	input_thread.join();

	return 0;
}
