#include "PostHandler.hpp"
#include "cgiLauncher.hpp"
#include <sstream>



HttpResponse	answer_post(HttpRequest const& request, LocationClass const& location)
{
	HttpResponse	response;
	std::string		tmp = location.getRoot();

	//TODO
	t_CgiParams	params;
	std::string	target = request.getRequestLineInfos().target;
	char		*ptr;

	tmp.append(target);
	if ( target.length() > 4 && target.find(".php") == (target.length() - 4))
	{
		params.scriptFilename = tmp;
		params.scriptName = target.substr(target.find_last_of('/'));
	}
	else
	{
		params.scriptFilename = tmp.append("/" + location.getIndex());
		params.scriptName = "/" + location.getIndex();
	}
	std::cout << "tmp: " << tmp << std::endl;
	params.redirectStatus = "200";
	params.requestMethod = "POST";
	if (request.getContentLength())
	{
		std::stringstream	stream;
		stream << request.getContentLength();
		stream >> params.contentLength;
	}
//	params.scriptName = "/blorg.bla";
//	params.pathInfo = "/home/user42/webserv/git_webserv/srcs/CgiLauncher/test.php";
//	params.pathInfo = "/blorg.bla";
	params.pathInfo = target;
//	params.pathInfo = "/test.php";
//	params.pathInfo = "/test.php/resultat";
//	params.pathInfo = "/specific";
	params.serverName = "localhost"; // a modif
	params.serverProtocol = "HTTP/1.1"; // a modif
	params.requestUri = target;
//	params.requestUri = "/blorg.bla";
	params.httpHost = "localhost"; // a modif
//	params.queryString = "/kaka/kiki/kuku";

	launchCgiScript(params, request, location, &ptr);
	std::cout << "output: " << ptr << std::endl;
	std::cout << "answering post request\n";
	return response;
}