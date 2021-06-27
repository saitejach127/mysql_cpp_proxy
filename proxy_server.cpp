#include <sys/socket.h>  // For socket
#include <netinet/in.h>  // For IP PROTO TCP
#include <arpa/inet.h>   // For inet addr ip mentioning
#include <bits/stdc++.h> // For memset
#include <unistd.h> // For close socket
#include <queue>

// Mysql Connection imports
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <driver.h>

using namespace std;
#define PORT 4322

void closeAndExit(string msg);

const string server = "tcp://localhost:3306";
const string username = "root";
const string password = "ML#tensorflow@122";

int sfd,nsfd;

sql::Driver *driver;
sql::Connection *con;
sql::Statement *stmt;

struct Request {
    string cmd;
    string token;
};

queue<Request> reqQueue;

Request parseRequest(string data){
    Request newRequest;
    string token = "";
    string cmd = "";
    bool flag = true;
    for(int i=0;i<data.length();i++){
        if(data[i] == '@'){
            flag = false;
            continue;
        }
        flag == true ? token += data[i] : cmd += data[i];
    }
    newRequest.token = token;
    newRequest.cmd = cmd;
    return newRequest;
}

void connectMysql(){
    try
	{
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
	}
	catch (sql::SQLException e)
	{
		closeAndExit(e.what());
	}
    con->setSchema("startDB");
}

string executeCmd(string cmd){
    try {
        stmt = con->createStatement();
        stmt->execute(cmd);
        cout<<"Finished executing "<<cmd<<endl;
    } catch (sql::SQLException e) {
        cout<<"Error executing cmd "<<cmd<<endl;
        cout<<e.what()<<endl;
        delete stmt;
        return e.what();
    }
    delete stmt;
    return "";
}

void closeAndExit(string msg){
    cout<<msg<<endl;
    close(nsfd);
    close(sfd);
    if(stmt){
        delete stmt;
    }
    if(con){
        delete con;
    }
    exit(1);
}

bool verifyToken(string token){
    return false;
}

int main(){
    string text;
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd < 0){
        closeAndExit("Server Socket Could not be created");
    }
    struct sockaddr_in myaddr, paddr;
    myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(PORT);
    bzero(&myaddr.sin_zero, 8);
    int k2 = bind(sfd, (struct sockaddr *)&myaddr, sizeof(struct sockaddr_in));
    if(k2 < 0){
        closeAndExit("Error occured in binding the server socket");
    }
    int k3 = listen(sfd, 10);
    if(k3 < 0){
        closeAndExit("Error occured in Listening for connections");
    }
    connectMysql();
    socklen_t addrlen;
    addrlen = sizeof(paddr);
    nsfd = accept(sfd, (struct sockaddr *)&paddr, &addrlen);
    if(nsfd < 0){
        closeAndExit("Connection with Client could not be established");
    }

    while(1){
        char buffer[2048];
        bzero(buffer, sizeof(buffer));
        int temp = recv(nsfd, buffer, 10000, 0);
        if(temp < 0){
            closeAndExit("Error receiving Message from client");
        }
        string tstr = buffer;
        cout<<"Request from client"<<tstr<<endl;
        Request parsedRequest = parseRequest(tstr);
        reqQueue.push(parsedRequest);

        Request popReq = reqQueue.front();
        reqQueue.pop();
        cout<<"Pop cmd "<<popReq.cmd<<endl;

        if(popReq.cmd == "exit"){
            break;
        }
        bool auth = verifyToken(popReq.token);
        if(!auth){
            string noAuthError = "Request not Authenticated";
            send(nsfd, noAuthError.c_str(),10000,0);
            continue;
        }
        string error = executeCmd(popReq.cmd);
        send(nsfd, error.c_str(),10000,0);
    }
    closeAndExit("");
    return 0;
}