/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   contextClass.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/12 14:11:07 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/18 13:26:13 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTEXTCLASS_HPP
# define CONTEXTCLASS_HPP

#include <vector>
#include <map>
#include <string>

class contextClass
{
	public:
			contextClass(std::string context_name, std::string buff);
			contextClass(contextClass const&);

			contextClass& operator = (contextClass const& copy);

			~contextClass();

			void	getBlocksInContext(std::vector<std::string> const& block_set);
			void	getDirectivesInContext(std::vector<std::string> const& directive_set);

			std::string								_name;
			std::string								_block_content;
			std::vector<std::string>				_directive_set;
			std::vector<std::string>				_block_set;
			std::map<std::string, std::string>		_directives;
			std::vector<contextClass*>				_blocks;

	private:
			contextClass();

			void							setBlocks(void);
			void							setDirectives(void);
			std::pair<bool, std::string>	getBlock(std::string const& block_name, std::string const& buff) const;
			std::pair<bool, std::string>	getSingleDirective(std::string const& directive_name, std::string const& buff) const;
};

#endif
