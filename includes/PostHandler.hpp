#ifndef POSTHANDLER_H
# define POSTHANDLER_H

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "LocationClass.hpp"

HttpResponse	answer_post(HttpRequest const& request, LocationClass const& location);

#endif