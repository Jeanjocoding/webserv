/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationClass.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/12 14:11:07 by asablayr          #+#    #+#             */
/*   Updated: 2021/07/15 14:55:40 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCLASS_HPP
# define LOCATIONCLASS_HPP

#include <vector>
#include <map>
#include <string>
#include "contextClass.hpp"

#define GET_METHOD 0
#define POST_METHOD 1
#define DELETE_METHOD 2

class LocationClass : protected contextClass
{
	public:
			LocationClass();
			LocationClass(std::string context_name, std::string buff);
			LocationClass(LocationClass const&);

			LocationClass& operator = (LocationClass const& copy);

			~LocationClass();

			std::string								_uri;
			std::string								_param;
			bool									_methods[3];

			unsigned int 							matchUri(std::string const& s);

	private:

};

#endif
