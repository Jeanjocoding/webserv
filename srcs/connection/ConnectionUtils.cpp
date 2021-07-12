#include "ConnectionUtils.hpp"

void	print_vec(std::vector<std::string>& to_print)
{
	size_t i = 0;

	std::cout << "deb vector: " << std::endl;
	while (i < to_print.size())
	{
		std::cout << to_print[i] << std::endl;
		i++;
	}
	std::cout << "------ end vector ---- " << std::endl << std::endl;
	return;
}

int	ft_strtrim(std::string& to_trim)
{
	int 	i = 0;

	while (to_trim[i] == ' ')
	{
		i++;
	}
	to_trim.erase(0, i);
	i = to_trim.length();
	if (i == 0)
		return (0);
	i--;
	while (to_trim[i] == ' ')
	{
		i--;
	}
	i++;
	to_trim.erase(i, to_trim.npos);
	return (1);
}

void	ft_strsplit_and_trim(std::string to_split, std::vector<std::string>& result)
{
	size_t	deb_word = 0;
	size_t	index_coma;
//	size_t	end_word;

	index_coma = to_split.find(',');
	if (index_coma == to_split.npos)
	{
		if (ft_strtrim(to_split))
			result.push_back(to_split);
		return;
	}
	while (index_coma != to_split.npos)
	{
		result.push_back(to_split.substr(deb_word, index_coma - deb_word));
		if (!ft_strtrim(result.back()))
			result.pop_back();
		deb_word = index_coma + 1;
		if (deb_word >= to_split.length())
			return;
		index_coma = to_split.find(',', deb_word);
	}
//	deb_word = index_coma + 1;
	if (deb_word >= to_split.length())
		return;
	result.push_back(to_split.substr(deb_word, to_split.length() - deb_word));
	if (!ft_strtrim(result.back()))
		result.pop_back();
	return;
}

void	ft_strsplit(std::string to_split, std::vector<std::string>& result)
{
	size_t	deb_word = 0;
	size_t	index_coma;
//	size_t	end_word;

	index_coma = to_split.find(',');
	if (index_coma == to_split.npos)
	{
		result.push_back(to_split);
		return;
	}
	while (index_coma != to_split.npos)
	{
		result.push_back(to_split.substr(deb_word, index_coma - deb_word));
		deb_word = index_coma + 1;
		if (deb_word >= to_split.length())
			return;
		index_coma = to_split.find(',', deb_word);
	}
//	deb_word = index_coma + 1;
	if (deb_word >= to_split.length())
		return;
	result.push_back(to_split.substr(deb_word, to_split.length() - deb_word));
}

int	upperize_string(std::string& str)
{
	size_t i = 0;

	while (i < str.length())
	{
		str[i] = toupper(str[i]);
		i++;
	}
	return (i);
}

int		caseInsensitiveComparison(std::string s1, std::string s2)
{
	if (s1.length() != s2.length())
		return (0);
	int	i = -1;
	int	len = s1.length();
	while (++i < len)
	{
		if (s1[i] >= 65 && s1[i] <= 90)
		{
			if (s1[i] == s2[i] - 32)
				continue;
		}
		else if (s1[i] >= 97 && s1[i] <= 122)
		{
			if (s1[i] == s2[i] + 32)
				continue;
		}
		if (s1[i] != s2[i])
			return (0);
	}
	return (1);
}

int	find_in_vec_insensitive(std::vector<std::string>& vec, std::string to_find)
{
	int i = 0;
	int length = vec.size();

	while (i < length)
	{
		if (caseInsensitiveComparison(vec[i], to_find))
			return (1);
		i++;
	}
	return (0);
}

/*int main(void)
{
	std::vector<std::string> result;

	ft_strsplit_and_trim("gzip, chunked      , boogiedance , hihihaha", result);
	print_vec(result);
	result.clear();
	ft_strsplit_and_trim("hello      ,bibi", result);
	print_vec(result);
	result.clear();
	ft_strsplit_and_trim("    cocorico", result);
	print_vec(result);
	result.clear();
	ft_strsplit_and_trim("     ", result);
	print_vec(result);
	result.clear();
	ft_strsplit_and_trim(",   cdzvde", result);
	print_vec(result);
	result.clear();
	ft_strsplit_and_trim("cdzvde   , ", result);
	print_vec(result);
	result.clear();
	ft_strsplit_and_trim("cdzvde ,  zfzef", result);
	print_vec(result);
	result.clear();
	return (0);
} */