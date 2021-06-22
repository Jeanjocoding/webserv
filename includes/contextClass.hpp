/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   contextClass.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/12 14:11:07 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/21 19:06:13 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTEXTCLASS_HPP
# define CONTEXTCLASS_HPP

#include <vector>
#include <map>
#include <string>
#include "directiveClass.hpp"

class contextClass
{
	public:
			contextClass(std::string context_name, std::string buff);
			contextClass(contextClass const&);

			contextClass& operator = (contextClass const& copy);

			~contextClass();

			void	getBlocksInContext(void);
			void	getDirectivesInContext(void);
			void	getAcceptedDirectivesInContext(void);

			std::string								_name;
			std::string								_block_content;
			std::vector<std::string>				_directive_set;
			std::vector<std::string>				_block_set;
			std::map<std::string, std::string>		_directives;
			std::map<std::string, directiveClass>	_accepted_directive_set;
			std::vector<contextClass*>				_blocks;

	private:
			contextClass();

			void							setBlocks(void);
			void							setDirectives(void);
			void							setAcceptedDirectives(void);
			std::pair<bool, std::string>	getBlock(std::string const& block_name, std::string const& buff) const;
			std::pair<bool, std::string>	getSingleDirective(std::string const& directive_name, std::string const& buff) const;
};

#endif
