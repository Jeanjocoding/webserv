#include "cgiLauncher.h"

/** a lancer à l'intérieur du processus fils pour que les changements d'env ne durent que
 * le temps de son éxécution */

int		setCgiVariable(std::string name, std::string& value)
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
	int		pipefd[2];
	int 	pid;
	char

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
		close (pipefd[1]);
		setCgiParamsAsEnvironmentVariables(params);
	}





}