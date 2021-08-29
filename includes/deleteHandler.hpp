#ifndef DELETEHANDLER_HPP
# define DELETEHANDLER_HPP

# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include "LocationClass.hpp"

HttpResponse	answer_delete(HttpRequest const& request, LocationClass const& location);

#endif