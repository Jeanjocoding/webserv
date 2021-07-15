SETUP_SRCS =	parse_conf.cpp \
				directiveClass.cpp \
				contextClass.cpp \
				LontextClass.cpp \
				setup_server.cpp

FULL_SETUP_SRCS = $(addprefix $(SETUP_PATH), $(SETUP_SRCS))

SRCS += $(FULL_SETUP_SRCS)
