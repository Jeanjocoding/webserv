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

int		ExecAndSetPipes(t_CgiParams& params, LocationClass const& location, ConnectionClass& connection)
{
	int			script_output_pipe[2];
	int			script_input_pipe[2];
	int 		pid;
	char 		**customEnv;
	char	**args = new char*[2];
	struct stat	st_stat;
	std::string	execname(location.getCGI());
	std::string	argname("php-cgi");

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
			perror("execve");
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
	if (connection._request_pipeline[0].getContentLength())
	{
		if (write(connection.getInputFd(), connection._request_pipeline[0].getContent(), connection._request_pipeline[0].getContentLength()) == -1)
		{
			connection.setHasToWriteOnPipe(0);
			connection.setHasToReadOnPipe(0);
			connection.setHasDoneCgi(1);
			connection.setCgiError(1);
			return (-1);
		}
	}
	close (connection.getInputFd());
	connection.setHasToWriteOnPipe(0);
	connection.setHasToReadOnPipe(1);
	return (0);
}

int		cgiReadOnPipe(ConnectionClass& connection)
{
	int read_ret;
	char read_buffer[4096];
	int	wait_ret;
	int	wait_status;

	read_ret = read(connection.getOutputFd(), read_buffer, 4096);
	if (read_ret == -1)
	{
		connection.setHasToWriteOnPipe(0);
		connection.setHasToReadOnPipe(0);
		connection.setHasDoneCgi(1);
		connection.setCgiError(1);
		if (connection._cgiOutput_len)
		{
			delete [] connection._cgiOutput;
			connection._cgiOutput = 0;
			connection._cgiOutput_len = 0;
		}
		return (-1);
	}
	else if (read_ret == 0)
	{
		connection.setHasToReadOnPipe(0);
		close(connection.getOutputFd());
		wait_ret = waitpid(connection.getChildPid(), &wait_status, 0);
	}
	else
		append_to_buffer(&connection._cgiOutput, connection._cgiOutput_len, read_buffer, read_ret);
	return (0);
}
