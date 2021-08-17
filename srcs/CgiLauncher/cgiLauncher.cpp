#include "cgiLauncher.h"

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
	printtab(customEnv, index);

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
	char 		**customEnv;

//	std::string	execname("/usr/bin/php-cgi");
	std::string	execname("/home/user42/webserv/git_webserv/srcs/CgiLauncher/ubuntu_cgi_tester");
//	std::string	argname("/home/user42/webserv/git_webserv/srcs/CgiLauncher/test.php");
	std::string	argname("/home/user42/webserv/git_webserv/srcs/CgiLauncher/test.php");
	char*	args[] = {(char*)execname.c_str(), (char*)argname.c_str(), NULL};

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
		allocateCustomEnv(&customEnv);
		setCgiParamsAsEnvironmentVariables(params, customEnv);
//		std::string method_to_check("REQUEST_METHOD");
//		std::cout << "REQUEST METHOD in env: " << std::getenv("REQUEST_METHOD") << std::endl;
//		if (execve("./ubuntu_cgi_tester", args, customEnv) == -1)
		if (execve("/home/user42/webserv/git_webserv/srcs/CgiLauncher/ubuntu_cgi_tester", args, customEnv) == -1)
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
	params.scriptFilename = "/home/user42/webserv/git_webserv/srcs/CgiLauncher/test.php";
	params.scriptName = "test.php";
//	params.pathInfo = "/home/user42/webserv/git_webserv/srcs/CgiLauncher/test.php";
	params.pathInfo = "/test.php";
//	params.pathInfo = "/test.php";
//	params.pathInfo = "/test.php/resultat";
//	params.pathInfo = "/specific";
	params.serverName = "hello.local";
	params.serverProtocol = "HTTP/1.1";
	params.requestUri = "/test.php";
	params.httpHost = "hello.local";
	params.queryString = "";

	launchCgiScript(params, &ptr);
	std::cout << "output: " << ptr << std::endl;
	return (0);
}