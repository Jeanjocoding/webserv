/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directiveClass.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/12 14:11:07 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/28 13:08:26 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRECTIVECLASS_HPP 
# define DIRECTIVECLASS_HPP

#include <vector>
#include <map>
#include <string>

#define	SYNTAX_ON_OFF		1
#define	SYNTAX_STRING		2
#define	SYNTAX_FILE			3
#define	SYNTAX_BLOCK		4
#define	SYNTAX_SIZE			5
#define	SYNTAX_NUMBER		6
#define	SYNTAX_NUMBER_SIZE	7
#define	SYNTAX_TIME			8
#define	SYNTAX_PATH			9
#define	SYNTAX_ALL_ANY		10
#define	SYNTAX_NAME			11
#define	SYNTAX_ADDRESS		12
#define	SYNTAX_RATE			13
#define	SYNTAX_CODE_URI		14

class directiveClass
{
	public:
			directiveClass();
			directiveClass(std::string context_name, std::string buff);
			directiveClass(directiveClass const&);

			directiveClass& operator = (directiveClass const& copy);

			~directiveClass();

			bool	isInContext(std::string const& context_name) const;
			bool	parse(std::string);

			std::string								_name;
			std::vector<std::string>				_contexts;
			unsigned short							_syntax;

	private:

};

#endif
