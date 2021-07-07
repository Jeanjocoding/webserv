#ifndef CONNECTIONUTILS_HPP
# define CONNECTIONUTILS_HPP

#include <vector>
#include <iostream>

void	print_vec(std::vector<std::string>& to_print);
int	ft_strtrim(std::string& to_trim);
void	ft_strsplit_and_trim(std::string to_split, std::vector<std::string>& result);
void	ft_strsplit(std::string to_split, std::vector<std::string>& result);
int	upperize_string(std::string& str);


#endif