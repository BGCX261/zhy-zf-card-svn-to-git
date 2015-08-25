include ./Makefile.in

INCLUDE += -I /usr/local/include -I /usr/local/include/fastdfs 
LIB_CLUDE = 

LINK_LIB = -lmysqlclient_r

test_mysql:test_mysql.o
	$(CXX) $(CFLAGS) $(INCLUDE) test_mysql.o -o test_mysql $(INCLUDE) $(LIB_CLUDE) -lmysqlclient_r  

clean:
	rm test_mysql