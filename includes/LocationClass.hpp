/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationClass.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/12 14:11:07 by asablayr          #+#    #+#             */
/*   Updated: 2021/07/17 18:12:59 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCLASS_HPP
# define LOCATIONCLASS_HPP

#include <vector>
#include <map>
#include <string>
#include "contextClass.hpp"

#define GET_RANK 0
#define POST_RANK 1
#define DELETE_RANK 2

class LocationClass : public contextClass
{
	public:
			LocationClass();
			LocationClass(std::string const& context_name, std::string const& buff);
			LocationClass(LocationClass const&);

			LocationClass& operator = (LocationClass const& copy);

			~LocationClass();

			std::string		_uri;
			std::string		_param;
			bool			_methods[3];

			void			setMethods(void);
			unsigned int 	matchUri(std::string const& s) const;

	private:

};

#endif
