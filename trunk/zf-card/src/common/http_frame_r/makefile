include ../Inc.mk

LINK_INC += -I../inc/ -I../inc/cgi-fw/

#LIB_INC = url_utils.h http_request.h http_response.h http_service.h http_handler.h http_logs.h base64.h rfc1867.h zend_llist.h
LIB_OBJS = http_request_r.o http_response_r.o http_service_r.o 

LIB_TARGET=libfcgi-framework_r.a

all:$(LIB_OBJS)
	ar rs $(LIB_TARGET) $(LIB_OBJS);
#-mkdir -p ../inc/cgi-fw;
#	cp *.h ../inc/cgi-fw/ -f;
#	-mkdir -p ../lib;
#	cp *.a ../lib -f

clean:
	rm -rf *.o *.a