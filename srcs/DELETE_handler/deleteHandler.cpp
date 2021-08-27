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
		std::cout << "not allowed" << std::endl;
		response = HttpResponse(404, location.getErrorPage(405));
		return (response);
	}
	tmp.append(request.getRequestLineInfos().target);
	body.open(tmp);
	if (!body.is_open())
	{
		response = HttpResponse(404, location.getErrorPage(404));
		body.close();
		return (response);
	}
	else
	{
		body.close();
//		std::cout << "i delete" << std::endl;
		std::remove(tmp.c_str());
	}
//	response = HttpResponse();
//	std::cout << "answering delete request\n";
	return response;
}