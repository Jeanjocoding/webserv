#include "cgiLauncher.hpp"
#include <cstdlib>
#include <cstring>
#include "utils.hpp"
#include <sys/types.h>
#include <sys/stat.h>

void		printtab(char ** tab, int length)
{
	int  i = 0;

	std::cout << "----------- print tab: -------------" << std::endl;
	while (i < length)
	{
		std::cout << tab[i] << std::endl;
		i++;
	}
	std::cout << " ----------------------- " << std::endl;
}

/** recupere l'environnement global, peut-être serait-il mieux de faire un environnement custom? */
extern char **environ;

/** a lancer à l'intérieur du processus fils pour que les changements d'env ne durent que
 * le temps de son éxécution */

int			allocateCustomEnv(char ***customEnv)
{
	*customEnv = new char*[ENV_SIZE];
	return (1);
}

void		setCgiVariable(std::string name, std::string& value, char **customEnv, int& envIndex)
{
	name.append(value);
	customEnv[envIndex] = new char[name.length() + 1];
	std::strncpy(customEnv[envIndex], name.c_str(), name.length());
	customEnv[envIndex][name.length()] = '\0';
	envIndex++;
}

int		setCgiParamsAsEnvironmentVariables(t_CgiParams& params, char **customEnv)
{
	int		index = 0;

	setCgiVariable("REQUEST_METHOD=", params.requestMethod, customEnv, index);
	setCgiVariable("REDIRECT_STATUS=", params.redirectStatus, customEnv, index);
	setCgiVariable("SCRIPT_FILENAME=", params.scriptFilename, customEnv, index);
	setCgiVariable("SCRIPT_NAME=", params.scriptName, customEnv, index);
	setCgiVariable("PATH_INFO=", params.pathInfo, customEnv, index);
	setCgiVariable("SERVER_NAME=", params.serverName, customEnv, index);
	setCgiVariable("SERVER_PROTOCOL=", params.serverProtocol, customEnv, index);
	setCgiVariable("REQUEST_URI=", params.requestUri, customEnv, index);
	setCgiVariable("HTTP_HOST=", params.httpHost, customEnv, index);
	setCgiVariable("QUERY_STRING=", params.queryString, customEnv, index);
	setCgiVariable("CONTENT_LENGTH=", params.contentLength, customEnv, index);
	setCgiVariable("CONTENT_TYPE=", params.contentType, customEnv, index);
	customEnv[index] = (char*)0;

	return (0);


}



/*int		launchCgiScript(t_CgiParams& params, HttpRequest const& request, LocationClass const& location, char **output, size_t& output_len)
{
	int			script_output_pipe[2];
	int			script_input_pipe[2];
	int 		pid;
	std::string	output_str;
	int			read_ret;
	int			wait_ret;
	int			wait_status;
	long			buffer_size = 0;
	char		read_buffer[4096];
	char 		**customEnv;
	char	**args = new char*[2];

	std::string	execname(location.getCGI());
	std::string	argname("php-cgi");

	args[0] = new char[execname.length() + 1];
	std::strncpy(args[0], execname.c_str(), execname.length());
	args[0][execname.length()] = '\0';
	args[1] = (char*)0;
	location.getUri(); // pour eviter pbs de compilation
	if (pipe(script_output_pipe) < 0)
	{
		perror("pipe");
		return (-1);
	}
	if (pipe(script_input_pipe) < 0)
	{
		perror("pipe");
		return (-1);
	}
	if ((pid = fork()) < 0)
	{
		perror("fork");
		return (-1);
	}
	else if (pid == 0)
	{
		close(script_output_pipe[0]);
		dup2(script_output_pipe[1], 1);
		close(script_output_pipe[1]);
		close(script_input_pipe[1]);
		dup2(script_input_pipe[0], 0);
		close(script_input_pipe[0]);
		allocateCustomEnv(&customEnv);
		setCgiParamsAsEnvironmentVariables(params, customEnv);
		if (execve(args[0], (char *const *) args, customEnv) == -1)
			perror("execvezz");
		std::cout << "execve failed" << std::endl;
		return (-1);
	}
	else
	{
		close(script_input_pipe[0]);
		if (request.getMethod() == POST_METHOD && request.getContentLength())
		{
			if (write(script_input_pipe[1], request.getContent(), request.getContentLength()) == -1)
				perror("write");
		}
		else if (request.getMethod() == GET_METHOD && request.getRequestLineInfos().target.find("?") != std::string::npos)
		{
			std::string tmp(request.getRequestLineInfos().target.find("?"), request.getRequestLineInfos().target.size());
			tmp.erase(0, 1);
			if (!tmp.empty())
				if (write(script_input_pipe[1], tmp.c_str(), tmp.size()) == -1)
					perror("write");
		}
		close (script_input_pipe[1]);
		close(script_output_pipe[1]);
		while ((read_ret = read(script_output_pipe[0], read_buffer, 4096)) > 0)
		{
			append_to_buffer(output, buffer_size, read_buffer, read_ret);
		}
		close (script_output_pipe[0]);
		delete [] args[0];
		delete [] args;
		if (read_ret == -1)
		{
			perror("read");
			return (-1);
		}
		wait_ret = wait(&wait_status);
		output_len = buffer_size;
	}
	return (0);
}*/

int		ExecAndSetPipes(t_CgiParams& params, LocationClass const& location, ConnectionClass& connection)
{
	int			script_output_pipe[2];
	int			script_input_pipe[2];
	int 		pid;
//	std::string	output_str;
//	int			read_ret;
//	int			wait_ret;
//	int			wait_status;
//	long			buffer_size = 0;
//	char		read_buffer[4096];
	char 		**customEnv;
	char	**args = new char*[2];
	struct stat	st_stat;
//	std::string	execname("/Users/tlucille/.brew/bin/php-cgi");
	std::string	execname(location.getCGI());
	std::string	argname("php-cgi");

	std::cout << "in exec and set pipes" << std::endl;
	std::cout << "execname: " << execname << std::endl;

	if (stat(execname.c_str(), &st_stat) == -1)
	{
		std::cout << "MANUAL SETUP REQUIRED: the path to the php-cgi binary given in srcs/CgiLauncher/CgiLauncher.cpp is wrong. Please update it with the path to the php-cgi binnary on your machine" << std::endl;
		return (-1);
	}
	args[0] = new char[execname.length() + 1];
	std::strncpy(args[0], execname.c_str(), execname.length());
	args[0][execname.length()] = '\0';
	args[1] = (char*)0;
	location.getUri(); // pour eviter pbs de compilation
	if (pipe(script_output_pipe) < 0)
	{
		perror("pipe");
		return (-1);
	}
	connection.setOutputFd(script_output_pipe[0]);
	if (pipe(script_input_pipe) < 0)
	{
		perror("pipe");
		return (-1);
	}
	connection.setInputFd(script_input_pipe[1]);
	connection.setHasToWriteOnPipe(1);
	if ((pid = fork()) < 0)
	{
		perror("fork");
		return (-1);
	}
	else if (pid == 0)
	{
		close(script_output_pipe[0]);
		dup2(script_output_pipe[1], 1);
		close(script_output_pipe[1]);
		close(script_input_pipe[1]);
		dup2(script_input_pipe[0], 0);
		close(script_input_pipe[0]);
		allocateCustomEnv(&customEnv);
		setCgiParamsAsEnvironmentVariables(params, customEnv);
		if (execve(args[0], (char *const *) args, customEnv) == -1)
			perror("execvezz");
		std::cout << "execve failed" << std::endl;
		return (-1);
	}
	else
	{
		close(script_input_pipe[0]);
		close(script_output_pipe[1]);
		connection.setChildPid(pid);
		delete [] args[0];
		delete [] args;
		return (0);
	}
}

int		cgiWriteOnPipe(ConnectionClass& connection)
{
	std::cout << "in write on pipes" << std::endl;
//	std::cout << "pipeline length: " << connection._request_pipeline.size() << std::endl;
	if (/*connection._request_pipeline[0].getMethod() == POST_METHOD && */connection._request_pipeline[0].getContentLength())
	{
		if (write(connection.getInputFd(), connection._request_pipeline[0].getContent(), connection._request_pipeline[0].getContentLength()) == -1)
		{
			perror("write");
			return (-1);
		}
	}
/*	else if (connection._request_pipeline[0].getMethod() == GET_METHOD && connection._request_pipeline[0].getRequestLineInfos().target.find("?") != std::string::npos)
	{
		std::string tmp(connection._request_pipeline[0].getRequestLineInfos().target.find("?"), connection._request_pipeline[0].getRequestLineInfos().target.size());
		tmp.erase(0, 1);
		if (!tmp.empty())
			if (write(connection.getInputFd(), tmp.c_str(), tmp.size()) == -1)
			{
				perror("write");
				return (-1);
			}
	}*/
	close (connection.getInputFd());
	connection.setHasToWriteOnPipe(0);
	connection.setHasToReadOnPipe(1);
	return (0);
}

int		cgiReadOnPipe(ConnectionClass& connection)
{
	int read_ret;
	char read_buffer[4096];
//	int	buffer_size = 0;
	int	wait_ret;
	int	wait_status;

	std::cout << "in read on pipe" << std::endl;
	read_ret = read(connection.getOutputFd(), read_buffer, 4096);
//		connection._currentResponse->appendToContent(read_buffer, )
	if (read_ret == -1)
	{
		perror("read in cgiReadonPipe");
		return (-1);
	}
	else if (read_ret == 0)
	{
		connection.setHasToReadOnPipe(0);
		close(connection.getOutputFd());
		wait_ret = waitpid(connection.getChildPid(), &wait_status, 0);
	}
	else
	{
		append_to_buffer(&connection._cgiOutput, connection._cgiOutput_len, read_buffer, read_ret);
//		std::cout << std::endl << "length read for now: " << connection._cgiOutput_len << std::endl;
//		write(1, connection._cgiOutput, connection._cgiOutput_len);
		std::cout << std::endl;
	}
//	output_len = buffer_size;
//	std::cout << "exiting cgiRead.." << std::endl;
	return (0);
}
