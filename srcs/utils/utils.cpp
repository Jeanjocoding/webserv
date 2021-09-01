#include <cstring>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

int		append_to_buffer(char **buffer, long& buffer_size, char * to_append, int append_size)
{
//	std::cout << "in use" << std::endl;
	char *new_buf = new char[buffer_size + append_size];
	if (buffer_size)
		std::strncpy(new_buf, *buffer, buffer_size);
	std::memcpy(&new_buf[buffer_size], to_append, append_size);
	if (buffer_size > 0)
		delete *buffer;
	buffer_size += append_size;
	*buffer = new_buf;
//	std::cout << "buffer after append: ";
//	write(1, *buffer, buffer_size);
//	std::cout << std::endl;
	return (1);
}