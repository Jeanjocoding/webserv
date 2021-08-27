#include "deleteHandler.hpp"

HttpResponse	answer_delete(HttpRequest const& request, LocationClass const& location)
{
	HttpResponse	response;
	std::string		tmp = location.getRoot();

	//TODO
	tmp.append(request.getRequestLineInfos().target);
	std::cout << "answering delete request\n";
	return response;
}