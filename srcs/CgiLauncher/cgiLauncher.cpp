#include "cgiLauncher.h"

/** recupere l'environnement global, peut-être serait-il mieux de faire un environnement custom? */
extern char **environ;

/** a lancer à l'intérieur du processus fils pour que les changements d'env ne durent que
 * le temps de son éxécution */

void		setCgiVariable(std::string name, std::string& value)
{
	name.append(value);
	putenv((char*)name.c_str());
}

int		setCgiParamsAsEnvironmentVariables(t_CgiParams& params)
{
	setCgiVariable("REDIRECT_STATUS=", params.redirectStatus);
	setCgiVariable("REQUEST_METHOD=", params.requestMethod);
	setCgiVariable("SCRIPT_FILENAME=", params.scriptFilename);
	setCgiVariable("SCRIPT_NAME=", params.scriptName);
	setCgiVariable("PATH_INFO=", params.pathInfo);
	setCgiVariable("SERVER_NAME=", params.serverName);
	setCgiVariable("SERVER_PROTOCOL=", params.scriptFilename);
	setCgiVariable("REQUEST_URI=", params.requestUri);
	setCgiVariable("HTTP_HOST=", params.httpHost);

	return (0);


}

int		launchCgiScript(t_CgiParams& params, char **output)
{
	int			pipefd[2];
	int 		pid;
	std::string	output_str;
	int			read_ret;
	int			wait_ret;
	int			wait_status;
	char		read_buffer[4096];

	std::string	execname("/usr/bin/php-cgi");
	std::string	argname("test.php");
	char*	args[] = {(char*)execname.c_str(), (char*)argname.c_str()};

	if (pipe(pipefd) < 0)
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
		close(pipefd[0]);
		dup2(pipefd[1], 1);
		close (pipefd[1]); //pas sur, mais ça semble marcher
		setCgiParamsAsEnvironmentVariables(params);
		if (execve("/usr/bin/php-cgi", args, environ) == -1)
			perror("execve");
		std::cout << "execve failed" << std::endl;
		close(pipefd[1]);
		return (-1);
	}
	else
	{
		close(pipefd[1]);
		dup2(pipefd[0], 0);
//		close (pipefd[0]);
//		sleep(1);
		while ((read_ret = read(pipefd[0], read_buffer, 4096)) > 0)
		{
//			std::cout << "output_Str: " << output_str << std::endl;
			output_str.append(read_buffer, read_ret);
//			if (read_ret < 4096)
//				break;
		}
		close(pipefd[0]);
		if (read_ret == -1)
		{
			perror("read");
			return (-1);
		}
		wait_ret = wait(&wait_status);
		*output = new char[output_str.length()];
		strcpy(*output, output_str.c_str());
	}
	return (0);
}

int		main()
{
	t_CgiParams	params;
	char		*ptr;

	params.redirectStatus = "200";
	params.requestMethod = "GET";
	params.scriptFilename = "test.php";
	params.scriptName = "/test.php";
	params.pathInfo = "/home/user42/webserv/git_webserv/srcs/CgiLauncher/";
	params.serverName = "hello.local";
	params.serverProtocol = "HTTP/1.1";
	params.requestUri = "/kuku/kiki";
	params.httpHost = "hello.local";

	launchCgiScript(params, &ptr);
	std::cout << "output: " << ptr << std::endl;
	return (0);
}