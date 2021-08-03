HTTPMSG_SRCS =	HttpMessage.cpp \
				HttpRequest.cpp \
				HttpResponse.cpp

FULL_HTTPMSG_SRCS = $(addprefix $(HTTPMSG_PATH), $(HTTPMSG_SRCS))

SRCS += $(FULL_HTTPMSG_SRCS)
