#include "PostHandler.hpp"
#include "cgiLauncher.hpp"

HttpResponse	answer_post(HttpRequest const& request, LocationClass const& location)
{
	HttpResponse	response;
	std::string		tmp = location.getRoot();

	//TODO
	t_CgiParams	params;
	char		*ptr;

	params.redirectStatus = "200";
	params.requestMethod = "POST";
	params.contentLength = "10";
	params.scriptFilename = "/home/user42/webserv/git_webserv/srcs/CgiLauncher/test.php";
	params.scriptName = "/test.php";
//	params.scriptName = "/blorg.bla";
//	params.pathInfo = "/home/user42/webserv/git_webserv/srcs/CgiLauncher/test.php";
//	params.pathInfo = "/blorg.bla";
	params.pathInfo = "/test.php";
//	params.pathInfo = "/test.php";
//	params.pathInfo = "/test.php/resultat";
//	params.pathInfo = "/specific";
	params.serverName = "localhost";
	params.serverProtocol = "HTTP/1.1";
	params.requestUri = "/test.php";
//	params.requestUri = "/blorg.bla";
	params.httpHost = "localhost";
	params.queryString = "/kaka/kiki/kuku";

	launchCgiScript(params, request, location, &ptr);
	std::cout << "output: " << ptr << std::endl;
	tmp.append(request.getRequestLineInfos().target);
	std::cout << "answering post request\n";
	return response;
}