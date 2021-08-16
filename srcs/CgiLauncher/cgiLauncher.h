#ifndef CGILAUNCHER_H
# define CGILAUNCHER_H

# include <iostream>
# include <unistd.h>
# include <cstdlib>
# include <cstring>
# include <sys/wait.h>
# include <stdio.h>

/** cette structure contient les paramètres nécessaire au lancement de scripts CGI. elle est
 * passée en paramètre de la fonction de lancement.
 */
typedef struct s_CgiParams
{

	std::string		redirectStatus;

	/**	"The SCRIPT_NAME variable MUST be set to a URI path (not URL-encoded)
	*	which could identify the CGI script (rather than the script's
 	*	output).  The syntax is the same as for PATH_INFO (section 4.1.5)

      SCRIPT_NAME = "" | ( "/" path )

   The leading "/" is not part of the path.  It is optional if the path
   is NULL; however, the variable MUST still be set in that case"." */
	std::string scriptName;


	std::string	requestMethod;

	/** scritpt's absolute path */
	std::string	scriptFilename;

	/** "The PATH_INFO variable specifies a path to be interpreted by the CGI
   *	script.  It identifies the resource or sub-resource to be returned by
   *	the CGI script, and is derived from the portion of the URI path
   *	hierarchy following the part that identifies the script itself.
   *	Unlike a URI path, the PATH_INFO is not URL-encoded, and cannot
   *	contain path-segment parameters.  A PATH_INFO of "/" represents a
   *	single void path segment." */
	std::string pathInfo;

	/** "The PATH_TRANSLATED variable is derived by taking the PATH_INFO
	*	value, parsing it as a local URI in its own right, and performing any
	*	virtual-to-physical translation appropriate to map it onto the
	*	server's document repository structure." */
	std::string pathTranslated;

	/** "   The QUERY_STRING variable contains a URL-encoded search or parameter
	*	string; it provides information to the CGI script to affect or refine
	*	the document to be returned by the script." */
	std::string queryString;

	/** "   The REMOTE_ADDR variable MUST be set to the network address of the
	*	client sending the request to the server." */
	std::string	remoteAddress;

	/** "The REMOTE_HOST (= httpHost dans php) variable contains the fully qualified domain name of
	*	the client sending the request to the server, if available, otherwise
	*	NULL" */
	std::string	httpHost;

	/** "   The REMOTE_IDENT variable MAY be used to provide identity information
	*	reported about the connection by an RFC 1413 [20] request to the
	*	remote agent, if available.  The server may choose not to support
	*	this feature, or not to request the data for efficiency reasons, or
	*	not to return available identity data." */
	std::string	remoteIdent;

	/** "   The REMOTE_USER variable provides a user identification string
	*	supplied by client as part of user authentication." */
	std::string	remoteUser;

	
	std::string serverName;

	/** server's IP address*/
	std::string	serverAddr;
	std::string	serverProtocol;

	/** software's identification string, given in http responses*/
	std::string serverSoftware;

	/** The URI which was given in order to access this page; for instance, '/index.html'.  */
	std::string	requestUri;
	std::string authType;

	/** "The server MUST set this meta-variable if and only if the request is
	*	accompanied by a message-body entity.  The CONTENT_LENGTH value must
	*	reflect the length of the message-body after the server has removed
	*	any transfer-codings or content-codings." */
	int			contentLength;

	/** "If the request includes a message-body, the CONTENT_TYPE variable is
	*	set to the Internet Media Type [6] of the message-body." */
	std::string contentType;

	/**    "The GATEWAY_INTERFACE variable MUST be set to the dialect of CGI
	*	being used by the server to communicate with the script.  
	*	Syntax: GATEWAY_INTERFACE = "CGI" "/" 1*digit "." 1*digit    " */
	std::string gatewayInterface;

}				t_CgiParams;

int		setCgiParamsAsEnvironmentVariables(t_CgiParams& params);
int		launchCgiScript(t_CgiParams& params, char **output);

#endif
