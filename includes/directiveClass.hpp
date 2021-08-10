/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directiveClass.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/12 14:11:07 by asablayr          #+#    #+#             */
/*   Updated: 2021/08/06 18:49:54 by asablayr         ###   ########.fr       */
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
#define	SYNTAX_INPUT_FILE	4
#define	SYNTAX_BLOCK		5
#define	SYNTAX_SIZE			6
#define	SYNTAX_NUMBER		7
#define	SYNTAX_NUMBER_SIZE	8
#define	SYNTAX_TIME			9
#define	SYNTAX_PATH			10
#define	SYNTAX_ALL_ANY		11
#define	SYNTAX_NAME			12
#define	SYNTAX_ADDRESS		13
#define	SYNTAX_RATE			14
#define	SYNTAX_CODE_URI		15
#define	SYNTAX_CODE_FILE	16
#define	SYNTAX_METHOD		17

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
