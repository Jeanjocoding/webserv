#ifndef POSTHANDLER_H
# define POSTHANDLER_H

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "LocationClass.hpp"
#include "ConnectionClass.hpp"
# define FILE_NOT_FOUND -1 
# define EXTENSION_NOT_VALID -2 

HttpResponse	answer_post(HttpRequest const& request, LocationClass const& location, ConnectionClass& connection);
void	add_header_part(HttpResponse& response , char *str, size_t buffer_size ,size_t& body_beginning);

#endif