#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <sstream>
#define BUFFSIZE 512
#define CHUNKSIZE 1024
#define MAXLISTEN 5
using namespace std;
/*TODO :
1)change recv in client to a fixed no of time as rev is blocking 
*/
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void hello()
{
    cout << "hello " << endl;
}
void sendData(int comsfd)
{
    fstream fio;
    fio.open("sample2.txt", ios::app | ios::out | ios::in);
    char buff[BUFFSIZE];
    int tot = 0;
    fio.seekg(0, ios_base::end);
    int length = fio.tellg();
    fio.seekg(0, ios::beg);
    int i = 1;
    while (!fio.eof() && (length - tot) > BUFFSIZE && i < 4)
    {
        bzero(buff, sizeof(buff));
        fio.read(buff, BUFFSIZE);
        //buff[sizeof(buff) - 1] = '\n';
        int n = write(comsfd, buff, sizeof(buff));
        if (n < 0)
        {
            cout << "error sending data" << endl;
        }
        tot += n;
        i += 1;
        //fio.seekg(tot, ios::beg);
    }
    if ((length - tot) != 0 && i == 1)
    {
        bzero(buff, sizeof(buff));
        fio.read(buff, BUFFSIZE);
        //buff[sizeof(buff) - 1] = '\n';
        int n = write(comsfd, buff, length - tot);
        if (n < 0)
        {
            cout << "error sending data" << endl;
        }
        tot += n;
    }
    cout << tot << "bytes tranferred" << endl;
    /*
    string str = "hellooooooo from server hoohahahha";
    char buff[255];
    bzero(buff, sizeof(buff));
    strcpy(buff, str.c_str());
    int n = write(comsfd, str.c_str(), str.length());
    if (n < 0)
    {
        cout << "error sending data" << endl;
    }
    cout << "data transferred : " << n << " bytes ";
    */
}

void serve(int servportno)
{
    cout << "executing thread" << endl;
    int servsockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (servsockfd < 0)
    {
        error("failed to create server socket");
        exit(1);
    }
    cout << "here1" << endl;
    sockaddr_in servaddr;
    bzero((char *)&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(servportno);
    cout << "here 2 " << endl;
    int comsfd; //communication sockfd
    struct sockaddr_in cliaddr;
    int n;
    int bstat = bind(servsockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (bstat < 0)
    {
        cout << "server sockfd failed to bind" << endl;
        exit(1);
    }
    int lstat = listen(servsockfd, MAXLISTEN);
    if (lstat < 0)
    {
        cout << "listening error " << endl;
        exit(1);
    }
    socklen_t clilen = sizeof(cliaddr);
    while (1)
    {
        int comsfd = accept(servsockfd, (struct sockaddr *)&cliaddr, &clilen);
        if (comsfd < 0)
        {
            cout << "error accepting conection ";
            exit(1);
        }
        thread twork(sendData, comsfd);
        twork.detach();
        /*
        string str = "hellooooooo from server hoohahahha";
        char buff[255];
        bzero(buff, sizeof(buff));
        strcpy(buff, str.c_str());
        n = write(comsfd, str.c_str(), str.length());
        if (n < 0)
        {
            cout << "error sending data" << endl;
        }
        cout << "data transferred : " << n << " bytes ";
        */
    }
}

sockaddr_in getservaddr(int toconp)
{
    cout << "in servaddr" << endl;
    sockaddr_in serv_addr;
    struct hostent *server;
    int clisockfd;
    string st = "localhost";
    cout << "here2" << endl;
    server = gethostbyname(st.c_str());
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    //bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(toconp);
    return serv_addr;
}
//sends tosend to clisockfd
void sendtoany1(int clisockfd, string tosend)
{
    char buff[BUFFSIZE];
    strcpy(buff, tosend.c_str());
    int n = send(clisockfd, buff, strlen(buff), 0);
    if (n < 0)
    {
        error("sending error/download file");
    }
    char status[BUFFSIZE];
    strcpy(buff, tosend.c_str());
    /*int x = recv(clisockfd, status, sizeof(status), 0);
    cout << status << endl;*/
}

void create_user(vector<string> &tokens, int clisockfd)
{
    string ipport = tokens[0];
    string cmd = tokens[1];
    string uid = tokens[2];
    string pass = tokens[3];
    string s = " ";
    string tosend = ipport + s + cmd + s + uid + s + pass;
    sendtoany1(clisockfd, tosend);
}

void login(vector<string> &tokens, int clisockfd)
{
    string ipport = tokens[0];
    string cmd = tokens[1];
    string uid = tokens[2];
    string pass = tokens[3];
    string s = " ";
    string tosend = ipport + s + cmd + s + uid + s + pass;
    sendtoany1(clisockfd, tosend);
}

void create_group(vector<string> &tokens, int clisockfd)
{
    string cmd = tokens[0];
    string gid = tokens[1];
    string s = " ";
    string tosend = cmd + s + gid;
    sendtoany1(clisockfd, tosend);
}

void join_group(vector<string> &tokens, int clisockfd)
{
    string cmd = tokens[1];
    string gid = tokens[2];
    string s = " ";
    string tosend = cmd + s + gid;
    sendtoany1(clisockfd, tosend);
}

void leave_group(vector<string> &tokens, int clisockfd)
{
    string cmd = tokens[0];
    string gid = tokens[1];
    string s = " ";
    string tosend = cmd + s + gid;
    sendtoany1(clisockfd, tosend);
}

void list_request(vector<string> &tokens, int clisockfd)
{
    string cmd = tokens[0];
    string gid = tokens[1];
    string s = " ";
    string tosend = cmd + s + gid;
    sendtoany1(clisockfd, tosend);
}

void accept_request(vector<string> &tokens, int clisockfd)
{
    string cmd = tokens[0];
    string gid = tokens[1];
    string uid = tokens[2];
    string s = " ";
    string tosend = cmd + s + gid + s + uid;
    sendtoany1(clisockfd, tosend);
}

void list_group(vector<string> &tokens, int clisockfd)
{
    string cmd = tokens[0];
    string tosend = cmd;
    sendtoany1(clisockfd, tosend);
}

void list_files(vector<string> &tokens, int clisockfd)
{
    string cmd = tokens[0];
    string gid = tokens[1];
    string s = " ";
    string tosend = cmd + s + gid;
    sendtoany1(clisockfd, tosend);
}

void upload_file(vector<string> &tokens, int clisockfd)
{
    string ipport = tokens[0];
    string cmd = tokens[1];
    string path = tokens[2];
    string gid = tokens[3];
    string s = " ";
    string tosend = ipport + s + cmd + s + path + s + gid;
    sendtoany1(clisockfd, tosend);
}

long long recvsize(string sport) //sport = string port
{
    char cport[BUFFSIZE];
    strcpy(cport, sport.c_str());
    int port = atoi(cport);
    port = htons(port);
    sockaddr_in serv_addr;
    struct hostent *server;
    int clisockfd;
    server = gethostbyname("localhost");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    //bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    clisockfd = socket(AF_INET, SOCK_STREAM, 0);
    int cstat = connect(clisockfd, (sockaddr *)&serv_addr, sizeof(serv_addr));
    if (cstat < 0)
    {
        error("opening client socket");
        exit(1);
    }
    char siz[BUFFSIZE];
    int n = recv(port, siz, strlen(siz), 0);
    {
        error("unable to get size from peer");
    }
    return (atoll(siz));
    close(clisockfd);
}

void creategarbagefile(string dest, long long size)
{
    fstream fio;

    string line;
    fio.open(dest, ios::app | ios::out | ios::in);
    for (int i = 1; i < size; i++)
    {
        // Press -1 to exit
        // Write line in file
        fio << "A";
    }
}

void download_file(vector<string> &tokens, int clisockfd)
{
    string ipport = tokens[0];
    string cmd = tokens[1];
    string gid = tokens[2];
    string fname = tokens[3];
    string destpath = tokens[4];
    string s = " ";
    string tosend = ipport + s + cmd + s + gid + s + fname;
    sendtoany1(clisockfd, tosend);
    char portbuff[BUFFSIZE];
    int n = recv(clisockfd, portbuff, strlen(portbuff), 0);
    if (n < 0)
    {
        error("unable to get ports data");
    }
    string portstring = portbuff;
    vector<string> portsvec;
    stringstream ss(portstring);
    string inter;
    //cout << "tokens[1] = " << tokens[1] << endl;
    while (getline(ss, inter, ' '))
    {
        portsvec.push_back(inter);
    }
    long long index = rand() % portsvec.size();
    long long sizeoffile = recvsize(portsvec[index]); //taking file size from 1st port number
    creategarbagefile(destpath, sizeoffile);
    int numofchunks = sizeoffile / CHUNKSIZE);
    int numofthreads = min(numofchunks, portsvec.size());
    while (numofthreads--)
    {
    }
}

void logout(vector<string> &tokens, int clisockfd)
{
    string cmd = tokens[0];
    string tosend = cmd;
    sendtoany1(clisockfd, tosend);
}

void show_downloads(vector<string> &tokens, int clisockfd)
{
    string cmd = tokens[0];
    string tosend = cmd;
    sendtoany1(clisockfd, tosend);
}

string stop_share(vector<string> &tokens, int clisockfd)
{
    string cmd = tokens[0];
    string gid = tokens[1];
    string fname = tokens[2];
    string s = " ";
    string tosend = cmd + s + gid + s + fname;
    sendtoany1(clisockfd, tosend);
}

/*
void client(int toconp)
{

/*
    memset(buff, 0, sizeof(buff));
    //cout << sizeof(buff) << endl;
    int n;
    int tot = 0;
    fstream fcli("recievedfile.txt", ios::out | ios::in);
    int i = 0;
    int cp = 1556;
    fcli.seekp(cp, ios::beg);
    while ((n = recv(clisockfd, buff, sizeof(buff), 0)) > 0)
    {
        cout << "ftell before : " << fcli.tellp() << endl;
        //n = recv(clisockfd, buff, sizeof(buff), 0);
        // buffer[n] = '\0';
        //cout << "n = " << n;
        //fcli << buff;
        fcli.write(buff, n);
        // cout << "seekpos " << fcli.tellp();
        memset(buff, 0, sizeof(buff));
        tot += n;
        cp += n;
        fcli.seekp(cp);
        cout << "ftell after : " << fcli.tellp() << endl;
        i++;
    }
    cout << tot << " bytes recvd" << endl;
    fcli.close();
    */

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(1);
    }
    int servportno = atoi(argv[1]);
    thread tserv(serve, servportno);
    cout << "waiting for commands : ";

    int trackerportno = 9009;
    sockaddr_in serv_addr = getservaddr(trackerportno);
    int clisockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clisockfd < 0)
    {
        error("error opening client socket");
        exit(1);
    }
    int cstat = connect(clisockfd, (sockaddr *)&serv_addr, sizeof(serv_addr));
    if (cstat < 0)
    {
        error("opening client socket");
        exit(1);
    }
    cout << "connected" << endl;
    while (1)
    {
        //making tokens
        //-------------------------------
        string cmd;
        cout << "enter cmd : " << endl;
        getline(cin, cmd);
        vector<string> tokens;
        stringstream ss(cmd);
        string inter;
        tokens.push_back(argv[1]);
        while (getline(ss, inter, ' '))
        {
            tokens.push_back(inter);
        }
        for (auto x : tokens)
        {
            cout << x << endl;
        }
        /*
        if (tokens[0] == "client")
        {
            cout << "enter port to connect to : " << endl;
            int toconp;
            cin >> toconp;
            cout << "enter hostname : " << endl;
            string hname;
            cin >> hname;
            thread tcli(client, toconp);
            tcli.detach();
        }
        */
        if (tokens[1] == "download_file")
        {
            download_file(tokens, clisockfd);
        }
        else if (tokens[1] == "create_user")
        {
            create_user(tokens, clisockfd);
        }
        else if (tokens[1] == "login")
        {
            login(tokens, clisockfd);
        }
        else if (tokens[1] == "create_group")
        {
            create_group(tokens, clisockfd);
        }
        else if (tokens[1] == "join_group")
        {
            join_group(tokens, clisockfd);
        }
        else if (tokens[1] == "leave_group")
        {
            leave_group(tokens, clisockfd);
        }
        else if (tokens[1] == "list_requests")
        {
            list_request(tokens, clisockfd);
        }
        else if (tokens[1] == "accept_requests")
        {
            accept_request(tokens, clisockfd);
        }
        else if (tokens[1] == "list_files")
        {
            list_files(tokens, clisockfd);
        }
        else if (tokens[1] == "upload_file")
        {
            upload_file(tokens, clisockfd);
        }
        else if (tokens[1] == "download_file")
        {
            download_file(tokens, clisockfd);
        }
        else if (tokens[1] == "logout")
        {
            logout(tokens, clisockfd);
        }
        else if (tokens[1] == "show_downloads")
        {
            show_downloads(tokens, clisockfd);
        }
        else if (tokens[1] == "stop_share")
        {
            stop_share(tokens, clisockfd);
        }
    }
    close(clisockfd);
    return 0;
}
