#include "deleteHandler.hpp"
#include <fstream>
#include <cstdio>

HttpResponse	answer_delete(HttpRequest const& request, LocationClass const& location)
{
	HttpResponse	response;
	std::string		tmp = location.getRoot();
	std::ifstream		body;

	//TODO
	if (!location.methodIsAllowed(DELETE_METHOD))
	{
		response = HttpResponse(405, location.getErrorPage(405));
		return (response);
	}
	tmp.append(request.getRequestLineInfos().target);
	body.open(tmp.c_str());
	if (!body.is_open())
	{
		response = HttpResponse(404, location.getErrorPage(404));
		body.close();
		return (response);
	}
	else
	{
		body.close();
		std::remove(tmp.c_str());
	}
	response.setStatusCode(204);
	response.setStatusMessage();
	response.setHeader();
//	std::cout << "answering delete request\n";
	return response;
}
