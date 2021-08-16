#include "cgiLauncher.h"

int		setCgiParamsAsEnvironmentVariables(t_CgiParams& params)
{
	
}

int		launchCgiScript(t_CgiParams& params)
{
	int		pipefd[2];
	int 	pid;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (-1);
	}




}