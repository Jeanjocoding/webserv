# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    includes_makefile                                  :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: asablayr <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/06/09 17:24:54 by asablayr          #+#    #+#              #
#    Updated: 2021/06/09 17:26:47 by asablayr         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

include $(addprefix $(SRCS_PATH), $(SETUP_PATH))module.mk
include $(addprefix $(SRCS_PATH), $(CONNECTION_PATH))module.mk
include $(addprefix $(SRCS_PATH), $(CGILAUNCHER_PATH))module.mk
include $(addprefix $(SRCS_PATH), $(HTTPMSG_PATH))module.mk
include $(addprefix $(SRCS_PATH), $(POSTHANDLER_PATH))module.mk
include $(addprefix $(SRCS_PATH), $(UTILS_PATH))module.mk
