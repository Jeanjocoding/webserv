/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   contextClass.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/12 14:11:07 by asablayr          #+#    #+#             */
/*   Updated: 2021/07/16 18:42:37 by asablayr         ###   ########.fr       */
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
			contextClass();
			contextClass(std::string const& context_name, std::string buff);
			contextClass(contextClass const&);

			contextClass& operator = (contextClass const& copy);

			virtual ~contextClass();

			virtual void	setContext(std::string name, std::string buff);

			void	getBlocksInContext(void);
			void	getDirectivesInContext(void);
			void	getAcceptedDirectivesInContext(void);

			std::string								_name;
			std::map<std::string, std::string>		_directives;
			std::vector<contextClass*>				_blocks;

	protected:
			void								setBlocks(void);
			void								setDirectives(void);
			std::vector<std::string>			setAcceptedDirectiveContext(std::string const& buff);
			void								setAcceptedDirectives(void);
			std::pair<bool, std::string>		getBlock(std::string const& block_name, std::string const& buff) const;
			std::pair<std::string, std::string>	getParamedBlock(std::string const& block_name, std::string const& buff) const;

	private:

			std::pair<bool, std::string>		getSingleDirective(std::string const& directive_name, std::string const& buff) const;
			bool								check_before(std::string const& buff, std::size_t i) const;
			bool								check_after(std::string const& buff, std::size_t& i) const;

			std::string								_block_content;
			std::vector<std::string>				_block_set;
			std::vector<directiveClass>				_directive_set;
			std::map<std::string, directiveClass>	_accepted_directive_set;
};

#endif
