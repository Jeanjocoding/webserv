/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directiveClass.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/21 17:42:15 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/21 18:26:29 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "directiveClass.hpp"

directiveClass::directiveClass()
{
}

directiveClass::directiveClass(directiveClass const& copy) : _name(copy._name), _contexts(copy._contexts), _syntax(copy._syntax)
{
}

directiveClass& directiveClass::operator = (directiveClass const& copy)
{
	_name = copy._name;
	_contexts = copy._contexts;
	_syntax = copy._syntax;
	return *this;
}

directiveClass::~directiveClass()
{
}

bool	directiveClass::isInContext(std::string context_name)
{
	for (auto i = _contexts.begin(); i != _contexts.end(); i++)
	{
		if (context_name == *i)
			return true;
	}
	return false;
}
