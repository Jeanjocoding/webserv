/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   default_server_settings.hpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/04 19:45:13 by asablayr          #+#    #+#             */
/*   Updated: 2021/09/13 20:38:01 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFAULT_SERVER_SETTINGS_HPP
# define DEFAULT_SERVER_SETTINGS_HPP

# define DEFAULT_PORT "8001"
# ifdef __APPLE__
#  define DEFAULT_HOST "0.0.0.0"
#  define DEFAULT_LISTEN "0.0.0.0:8001"
# else
#  define DEFAULT_HOST "*"
#  define DEFAULT_LISTEN "*:8001"
# endif
# define DEFAULT_SERVER_NAME "webserver"
# define DEFAULT_ROOT "."
# define DEFAULT_INDEX "index.html"
# define DEFAULT_ERROR_LOG "error.log"
# define DEFAULT_ACCESS_LOG "access.log"
# define DEFAULT_FASTCGI_PASS "false"
# define DEFAULT_BODY_MAX "1000000"
# define DEFAULT_KEEPALIVE_TIMEOUT "45"
# define DEFAULT_SENDFILE "on"
# define DEFAULT_UPLOAD_STORE "./test_pages/test_upload/upload_dir/"
# define DEFAULT_AUTOINDEX "off"

#endif
