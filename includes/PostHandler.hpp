#ifndef POSTHANDLER_H
# define POSTHANDLER_H

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "LocationClass.hpp"

HttpResponse	answer_post(HttpRequest const& request, LocationClass const& location);
void	add_header_part(HttpResponse& response , char *str, size_t buffer_size ,size_t& body_beginning);

#endif