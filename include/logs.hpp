#pragma once

#ifdef DEBUG
# define CONFLOG(message) if (true){std::fstream afile("log/loadedConfig.log", std::ios::out | std::ios::app); afile << message << std::endl;}
# define EXECLOG(message) if (true){std::fstream bfile("log/exec.log", std::ios::out | std::ios::app); bfile << message << std::endl;}
# define CONNECTLOG(message) if (true){std::fstream cfile("log/connections.log", std::ios::out | std::ios::app); cfile << message << std::endl;}
# define ERRLOG(message) if (true){std::fstream dfile("log/error.log", std::ios::out | std::ios::app); dfile << message << std::endl;}
# define READLOG(message) if (true){std::fstream efile("log/read.log", std::ios::out | std::ios::app); efile << message << std::endl;}
# define WRITELOG(message) if (true){std::fstream ffile("log/write.log", std::ios::out | std::ios::app); ffile << message << std::endl;}
#else
# define CONFLOG(message) (void)0;
# define EXECLOG(message) (void)0;
# define CONNECTLOG(message) (void)0;
# define ERRLOG(message) if (true){std::fstream dfile("log/error.log", std::ios::out | std::ios::app); dfile << message << std::endl;}
# define READLOG(message) (void)0;
# define WRITELOG(message) (void)0;
#endif