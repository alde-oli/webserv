#include "../include/webserv.hpp"
#include "../include/config/ServConfig.hpp"
#include "../include/ServRunner.hpp"

int main(int argc, char **argv)
{
	signal(SIGINT, signalHandler);

	std::vector<ServConfig>	servers;
	if (argc > 2)
		{std::cerr << "Usage: ./webserv [config_file.ini]" << std::endl; return 1;}
	else if (argc == 1)
		servers = ServerParsing("config/defaultConfig.ini");
	else
		servers = ServerParsing(argv[1]);

	std::fstream file("log/loadedConfig.log", std::ios::out | std::ios::trunc); file.close();
	std::fstream file1("log/exec.log", std::ios::out | std::ios::trunc); file1.close();
	std::fstream file2("log/connections.log", std::ios::out | std::ios::trunc); file2.close();
	std::fstream file3("log/error.log", std::ios::out | std::ios::trunc); file3.close();
	std::fstream file4("log/read.log", std::ios::out | std::ios::trunc); file4.close();
	std::fstream file5("log/write.log", std::ios::out | std::ios::trunc); file5.close();

	for (std::vector<ServConfig>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		it->checkValidity();
		CONFLOG(*it)
	}
	
	ServRunner::run(servers);
}
