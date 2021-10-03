#include "cgiLauncher.hpp"
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include "webserv.hpp"
#include "utils.hpp"

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

	std::cout << "in exec and set pipes" << std::endl;

	if (stat(execname.c_str(), &st_stat) == -1)
	{
		std::cout << "the path to the php-cgi \"" << execname << "\" is wrong. Please update it with the path to the php-cgi binnary on your machine" << std::endl;
		connection.errorOccured();
		return (-1);
	}
	args[0] = new char[execname.length() + 1];
	std::strncpy(args[0], execname.c_str(), execname.length());
	args[0][execname.length()] = '\0';
	args[1] = (char*)0;
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
	if (connection._request_pipeline[0].getContentLength())
	{
		if (write(connection.getInputFd(), connection._request_pipeline[0].getContent(), connection._request_pipeline[0].getContentLength()) == -1)
		{
			perror("write");
			connection.errorOccured(); // sets response to 500 et connection status to CO_HAS_TO_SEND
			return (-1);
		}
	}
	close (connection.getInputFd());
	connection.setStatus(CO_HAS_TO_READ_CGI);
	return (0);
}

int		cgiReadOnPipe(ConnectionClass& connection)
{
	int read_ret;
	char read_buffer[4096];
	int	wait_ret;
	int	wait_status;

	std::cout << "in read on pipe" << std::endl;
	read_ret = read(connection.getOutputFd(), read_buffer, 4096);
	if (read_ret == -1)
	{
		perror("read in cgiReadonPipe");
		connection.errorOccured(); // sets response to 500 et connection status to CO_HAS_TO_SEND
		return (-1);
	}
	else if (read_ret == 0)
	{
		connection.setStatus(CO_HAS_TO_ANSWER);
		close(connection.getOutputFd());
		wait_ret = waitpid(connection.getChildPid(), &wait_status, 0);
	}
	else
	{
		append_to_buffer(&connection._cgiOutput, connection._cgiOutput_len, read_buffer, read_ret);
		std::cout << "appended buffer" << std::endl;
	}
	return (0);
}

bool	setup_CGI(ConnectionClass& connection)
{
	t_CgiParams     params;
    int             retset;
    struct stat     st_stat;

    std::cout << "in setup CGI\n";//testing
    retset = setCgiParams(params, connection._request_pipeline[0], *(connection._request_pipeline[0].getLocation()));
    if (retset == EXTENSION_NOT_VALID)
    {
		std::cout << "method not allowed found\n";
        connection._currentResponse = HttpResponse(405, connection._request_pipeline[0].getLocation()->getErrorPage(405));
		connection.setStatus(CO_HAS_TO_SEND);
        return false;
    }
    else if (retset == FILE_NOT_FOUND || stat(params.scriptFilename.c_str(), &st_stat) == -1)
    {
		std::cout << "file not found\n";
        connection._currentResponse = HttpResponse(404, connection._request_pipeline[0].getLocation()->getErrorPage(404));
		connection.setStatus(CO_HAS_TO_SEND);
        return false;
    }
    if (ExecAndSetPipes(params, *(connection._request_pipeline[0].getLocation()), connection) == -1)
    {
		std::cout << "error\n";
        connection.errorOccured();
        return false;
    }
	connection.setStatus(CO_HAS_TO_WRITE_CGI);
	return true;
}

void	answer_CGI(ConnectionClass& connection)
{
	size_t body_beginning = 0;

    std::cout << "in answer CGI\n";//testing
	add_header_part(connection._currentResponse, connection._cgiOutput, connection._cgiOutput_len, body_beginning);
	connection._currentResponse.setBody(&(connection._cgiOutput[body_beginning]), connection._cgiOutput_len - body_beginning);
	connection._currentResponse.setHeader(200);
	if (!connection.isPersistent() || connection._request_pipeline[0].getLocation()->getKeepaliveTimeout() == 0)
		connection._currentResponse.setConnectionStatus(false);
	connection.sendResponse(connection._currentResponse.toString());// Handles all of the response sending and adjust the connection accordingly (cf: pop request list close connection etc...)
	delete [] connection._cgiOutput;
	connection._cgiOutput = 0;
	connection._cgiOutput_len = 0;
}
