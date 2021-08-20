# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: asablayr <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/11/26 10:50:04 by asablayr          #+#    #+#              #
#    Updated: 2021/07/04 19:52:34 by asablayr         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

SRCS_PATH = srcs/
OBJS_PATH = objs/
HDRS_PATH = includes/
DEFS_PATH = defines/

SETUP_PATH = setup/
CONNECTION_PATH = connection/
CGILAUNCHER_PATH = CgiLauncher/
POSTHANDLER_PATH = POST_handler/
HTTPMSG_PATH = HttpMessage/

HDRS =	webserv.hpp \
		serverClass.hpp \
		HttpMessage.hpp \
		HttpRequest.hpp \
		ConnectionUtils.hpp \
		CgiLauncher.hpp \
		PostHandler.hpp \
		ConnectionClass.hpp

SRCS =	main.cpp \
	handler.cpp \
	serverClass.cpp

include main_module.mk

TMP = $(SRCS:.cpp=.o)
OBJS = $(addprefix $(OBJS_PATH), $(TMP))
INCS = $(addprefix $(HDRS_PATH), $(HDRS))

CC = clang++
C_FLAGS = -Wall -Werror -Wextra -std=c++98 -g -fsanitize=address

all : $(NAME)

$(NAME): $(OBJS)
	$(CC) $(C_FLAGS) $(OBJS) -o $(NAME)

$(OBJS_PATH)%.o: $(SRCS_PATH)%.cpp $(INCS)
#	mkdir -p $(OBJS_PATH) $(OBJS_PATH)$(CONNECTION_PATH)
#	mkdir -p $(OBJS_PATH) $(OBJS_PATH)$(HTTPMSG_PATH)HttpMessage
	$(CC) $(C_FLAGS) -c $< -o $@ -I $(HDRS_PATH) -I $(DEFS_PATH)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: 	fclean
	make all
