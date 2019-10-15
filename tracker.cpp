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
#include <thread>
#include <vector>
#include <sstream>
#include <mysql_connection.h>
#include <driver.h>
#include <exception.h>
#include <resultset.h>
#include <statement.h>
#define BUFFSIZE 512
#define MAXLISTEN 5
using namespace std;
using namespace sql;
/*
TODO : 
1)change from default port to arbitrary port no
*/

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void sendtoany1(int clisockfd, string tosend)
{
    char buff[BUFFSIZE];
    strcpy(buff, tosend.c_str());
    int n = send(clisockfd, buff, strlen(buff), 0);
    if (n < 0)
    {
        error("sending error");
    }
}

void download_file(vector<string> &tokens, int comsfd, string uid)
{
    string gid = tokens[2];
    string fname = tokens[3];
    sql::Driver *driver;
    sql::Connection *con;

    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "123");
    con->setSchema("osass2");
    sql::Statement *stmt;
    sql::Statement *stmt1;
    sql::ResultSet *res;
    sql::ResultSet *res1;
    stmt = con->createStatement();
    stmt1 = con->createStatement();
    vector < string portsvector;
    string q = "select uid from filedata where filename = '" + fname + "'" + " and status='1'";
    cout << "query run : " << q << endl;
    try
    {
        res = stmt->executeQuery(q);
        while (res->next())
        {
            string ruid = res->getString("uid");
            string q1 = "select ipport from users where uid = '" + ruid + "'";
            cout << "query going to run : " << q1 << endl;
            res1 = stmt1->executeQuery(q1);
            while (res1->next())
            {
                string ripp = res1->getString("ipport");
                portsvector.push_back(ripp);
            }
        }
        string tosend = "";
        for (int i = 0; i < portsvector.size() - 1; i++)
        {
            tosend = tosend + portsvector[i] + " ";
        }
        tosend = tosend + portvector[i];
        sendtoany1(comsfd, tosend);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

void login(vector<string> &tokens, int comsfd)
{
    sql::Driver *driver;
    sql::Connection *con;

    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "123");
    con->setSchema("osass2");
    sql::Statement *stmt;
    sql::Statement *stmt1;
    sql::ResultSet *res;
    stmt = con->createStatement();
    stmt1 = con->createStatement();
    string ipport = tokens[0];
    string uid = tokens[2];
    string pass = tokens[3];
    int x = 0;
    string check = "select * from users where uid = '" + uid + "'";
    cout << "executing : " << check << endl;
    try
    {
        res = stmt->executeQuery(check);
        if (!res->next())
        {
            cout << "unable to retrieve result" << endl;
        }
        //res->last();
        string result = "failed";
        //int numofrows = res->getRow();
        //res->beforeFirst();
        //if (numofrows == 1)
        //{
        res->beforeFirst();
        res->next();
        string ruid = res->getString("uid");
        cout << "uid ret" << ruid << endl;
        string rpass = res->getString("password");
        cout << "passs retrieved " << rpass << endl;
        if (rpass == pass)
        {
            cout << "password matched" << endl;
            result = "success";
            string changeip;
            changeip = "update users set ipport='" + ipport + "' where uid='" + uid + "'";
            cout << "changing ip : " << changeip << endl;
            int y = stmt1->executeUpdate(changeip);
            if (y > 0)
            {
                cout << "ip updated successfully" << endl;
            }
        }
        else
        {
            result = "failed";
        }
        sendtoany1(comsfd, result);
        //}
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    // int x = stmt->executeUpdate(q);
    cout << "x: " << x << endl;
    con->close();
}

/*
void join_group(vector<string> &tokens, int comsfd)
{
    sql::Driver *driver;
    sql::Connection *con;

    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "123");
    con->setSchema("osass2");
    sql::Statement *stmt;
    sql::ResultSet *res;
    stmt = con->createStatement();
    string gid = tokens[2];
    try
    {
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}
*/
void create_user(vector<string> &tokens, int comsfd)
{
    sql::Driver *driver;
    sql::Connection *con;

    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "123");
    con->setSchema("osass2");
    sql::Statement *stmt;
    sql::ResultSet *res;
    int x = 0;
    try
    {
        stmt = con->createStatement();
        string ipport = tokens[0];
        string uid = tokens[2];
        string pass = tokens[3];
        string q = "insert into users values ('" + uid + "'" + "," + "'" + pass + "'" + ", '" + ipport + "'"
                                                                                                         ")";
        cout << "executing " << q << endl;
        //string check = "select * from users where uid = '" + uid + "'";
        x = stmt->executeUpdate(q);
        cout << "x: " << x << endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    // int x = stmt->executeUpdate(q);
    //cout << "x: " << x << endl;
    con->close();
}
//NOT COMPLETED upload and create user
//----------------------------------------------------------------------
void upload_file(vector<string> &tokens, int comsfd, string &uid)
{
    //string path = tokens[2];
    //string gid = tokens[3];
    sql::Driver *driver;
    sql::Connection *con;
    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "123");
    con->setSchema("osass2");
    sql::Statement *stmt;
    sql::ResultSet *res;
    string filepath = tokens[2]; // to be changed to file path
    string gid = "A";            //to be changed
    string status = "1";
    int x = 0;
    try
    {
        stmt = con->createStatement();
        string q = "insert into filedata values('" + uid + "','" + filepath + "','" + status + "')";
        cout << "to be executed " << q << endl;
        x = stmt->executeUpdate(q);
        cout << "x: " << x << endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        con->close();
    }
    con->close();
}

// tokens = port cmd args1 args2....
void cmdparser(int comsfd)
{
    string userid = "";
    while (1)
    {
        cout << "in thread " << endl;
        char rcvcmd[BUFFSIZE];
        memset(rcvcmd, 0, sizeof(rcvcmd));
        int n = recv(comsfd, rcvcmd, sizeof(rcvcmd), 0);
        if (n < 0)
        {
            cout << "n : "
                 << "connection terminated" << endl;
        }
        string scmd = rcvcmd;
        vector<string> tokens;
        stringstream ss(scmd);
        string inter;
        //cout << "tokens[1] = " << tokens[1] << endl;
        while (getline(ss, inter, ' '))
        {
            tokens.push_back(inter);
        }
        cout << "tokens[1] = " << tokens[1] << endl;
        if (tokens[1] == "create_user")
        {
            userid = tokens[2];
            create_user(tokens, comsfd);
        }
        else if (tokens[1] == "login")
        {
            userid = tokens[2];
            login(tokens, comsfd);
        }
        else if (tokens[1] == "upload_file")
        {
            upload_file(tokens, comsfd, userid);
        }
        else if (tokens[1] == "download_file")
        {
            download_file(tokens, comsfd, userid);
        }
    }
}

int main(int argc, char *argv[])
{
    int servportno = 9009;
    int servsockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (servsockfd < 0)
    {
        error("failed to create server socket");
        exit(1);
    }
    cout << "here1" << endl; //DELETE
    sockaddr_in servaddr;
    bzero((char *)&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(servportno);
    cout << "here 2 " << endl; //DELETE                //communication sockfd
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
        error("listening error");
        exit(1);
    }
    cout << "listening " << endl;
    socklen_t clilen = sizeof(cliaddr);
    char rcvcmd[BUFFSIZE];
    string command = "";
    int comsfd;

    while (1)
    {
        //cout << "in while loop";
        comsfd = accept(servsockfd, (struct sockaddr *)&cliaddr, &clilen);
        if (comsfd < 0)
        {
            cout << "error accepting conection ";
            exit(1);
        }
        thread t(cmdparser, comsfd);
        t.detach();

        //cout << "connection accepted at " << ntohl(cliaddr.sin_addr.s_addr) << endl;
        //thread tcmd(commands, comsfd);
        /*
        char buff[BUFFSIZE];
        string s = "";
        int n = recv(comsfd, buff, sizeof(buff), 0);
        if (n > 0)
        {
            s = buff;
            cout << s << endl;
            cout << buff << endl;
        }
        else
        {
            error("unable to send");
        }
        */
    }
    close(comsfd);
}