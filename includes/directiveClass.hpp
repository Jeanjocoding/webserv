/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directiveClass.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/12 14:11:07 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/21 18:25:41 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRECTIVECLASS_HPP 
# define DIRECTIVECLASS_HPP

#include <vector>
#include <map>
#include <string>

#define ON_OFF	1
#define	STRING	2
#define	INT		3
#define	BLOCK	4

class directiveClass
{
	public:
			directiveClass();
			directiveClass(std::string context_name, std::string buff);
			directiveClass(directiveClass const&);

			directiveClass& operator = (directiveClass const& copy);

			~directiveClass();

			bool	isInContext(std::string);

			std::string								_name;
			std::vector<std::string>				_contexts;
			unsigned short							_syntax;

	private:

};

#endif
