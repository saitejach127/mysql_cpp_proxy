sudo g++ -Wall -I/usr/include/cppconn -o server proxy_server.cpp -L/usr/lib -lmysqlcppconn
g++ client.cpp -o client
