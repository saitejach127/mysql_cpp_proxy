#include <sys/socket.h>	 // For socket
#include <unistd.h>		 // For socket reading
#include <netinet/in.h>	 // For IP PROTO TCP
#include <arpa/inet.h>	 // For inet addr ip mentioning
#include <bits/stdc++.h> // For bzero and cin cout
using namespace std;
#define PORT 4322

int main(){
	string text, output, path;
    char buffer[1024];
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd<0){
        cout<<"Error creating Socket "<<endl;
        exit(1);
    }
	struct sockaddr_in seraddr;
	//printf("%d\n",sfd);
	seraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(PORT);
	bzero(&seraddr.sin_zero, 8);
	int k1 = connect(sfd, (struct sockaddr *)&seraddr, sizeof(struct sockaddr_in));
    if(k1<0){
        cout<<"Error connecting to server "<<endl;
        exit(2);
    }
    while(1){
        cout<<"Enter cmd to execute"<<endl;
        string cmd;
        getline(cin, cmd);
        text = "1234@";
        text += cmd;
        send(sfd, text.c_str(),1000,0);
        cout<<"Sent cmd "<<text<<endl;
        if(cmd == "exit"){
            break;
        }
        char buffer[2048];
        bzero(buffer, sizeof(buffer));
        int temp = recv(sfd, buffer, 10000, 0);
        string error = buffer;
        if(error.length() != 0){
            cout<<"\033[1;31mError executing command "<<endl<<error<<"\033[0m\n";
        }
    }
	close(sfd);
	return 0;
}