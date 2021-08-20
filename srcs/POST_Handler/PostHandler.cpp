#include "PostHandler.hpp"

HttpResponse	answer_post(HttpRequest const& request, LocationClass const& location)
{
	HttpResponse	response;
	std::string		tmp = location.getRoot();

	//TODO

	tmp.append(request.getRequestLineInfos().target);
	std::cout << "answering post request\n";
	return response;
}