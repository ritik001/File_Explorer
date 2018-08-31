#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>

using namespace std;

FILE *in , *out;
vector<string> search_list_result;

string input="search /home/ritik/testing/1.txt";
string home="/home/ritik/";
string cur_dir="/home/ritik/testing";
vector<string> input_command;
string str_command_param="";
string operation;

/* string source_dir;
string destination_dir; */

bool is_dir(const char* path);
bool is_file(const char* path);


void copy_dir(string,string);
void copy_files(vector<string>);
void rename_files(string,string);
void create_file(string,string);
void create_dir(string,string);
vector<string> search(string,string);

/* void move_files();
void move_dir();
void delete_file();
void delete_dir();
void goto_directory(); */

void delete_dir(string);

void Error_Handler(string);
void Print_Search_Result();
void Clear_Command_Mode_Params();


int main()
{
    int count=0;
    for(int i=0;i<input.size();i++)
        {
            if(input[i]=='\\')
            {}
            else if(input[i]=='"' && input[i-1]!='\\')
                count=count+1;
            else if(input[i]!=' ' || (input[i]==' ' && count%2!=0))
                {
                str_command_param=str_command_param+input[i];
                }
            else if(input[i]==' ' && count%2==0)
            {
                input_command.push_back(str_command_param);                
                str_command_param="";
            }
                
        }
    input_command.push_back(str_command_param);
    operation = input_command[0];

    if(operation == "copy" || operation == "move" || operation == "rename" || operation == "create_dir" || operation == "create_file")
    {
        if(input_command.size()<3)
            Error_Handler("Not enough arguments for specified operation");
    }
    else if(operation == "delete_file" || operation =="delete_dir" || operation == "goto" || operation == "search")
    {
        if(input_command.size()<2)
            Error_Handler("Not enough arguments for specified operation");
    }
    else
    {
        Error_Handler("Invalid operation selected.");
    }

    if(operation == "copy")
    {
        copy_files(input_command);
    }
    else if(operation == "move")
        move_files();
    else if(operation == "rename")
        rename_files(input_command[1],input_command[2]);
    else if(operation == "create_file")
        create_file(input_command[1],input_command[2]);
    else if(operation == "create_dir")
        create_dir(input_command[1],input_command[2]);
    else if(operation == "delete_file")
        delete_file();
    else if(operation == "goto")
        goto_directory();
    else if(operation == "search")
        {
            search_list_result=search(input_command[2],input_command[1]);
            Print_Search_Result();
        }
    else if(operation == "snapshot")
        Error_Handler("Wrong Operation.Please enter a valid one.");


    return 0;
}

void copy_files(vector<string> input_command)
{
    char c;
    string s1;
    string s2;
    string temp;
    for(int i=1;i<input_command.size()-1;i++)
    {
    s1=input_command[i];
    temp=input_command[i].substr(input_command[i].find_last_of("/"));
    s2=input_command[input_command.size()-1]+"/"+temp;

    FILE *in, *out;
    in = fopen(s1.c_str(), "r");
    if (in == NULL)
    {
        exit(0);
    }

    out = fopen(s2.c_str(), "w");
    if (out == NULL)
    {
        exit(0);
    }
 
    c = fgetc(in);
    while (c != EOF)
    {
        fputc(c, out);
        c = fgetc(in);
    }
 
    fclose(in);
    fclose(out);
    s1="";
    s2="";
    temp="";
    }
}

void copy_dir(string source_dir,string destination_dir)
{
    vector<string> v;
    queue<string> q=queue<string>();
    queue<string> q1=queue<string>();

    string temp;
    string temp1;
    DIR *dp;
	struct dirent *dirp;
    destination_dir=destination_dir+"/"+source_dir.substr(source_dir.find_last_of("/"));
    mkdir(destination_dir.c_str(),(mode_t)0777);
    q.push(destination_dir);
    q1.push(source_dir);


    while(q.empty() == false)
    {
    v.push_back("copy");
    source_dir=q1.front();
    q1.pop();

	if((dp = opendir(source_dir.c_str())) == NULL )
		fprintf(stderr,"unable to open source directory");
	while(dirp = readdir(dp))
	{
        temp1=source_dir+"/"+dirp->d_name;
/*         cout << "source :" << temp1 << "\n"; */
        if(is_file(temp1.c_str()))
        {
            temp=source_dir+"/"+dirp->d_name;
            v.push_back(temp);
        }
        else if(temp1[temp1.size()-1]=='.' || temp1[temp1.size()-1]=='.' && temp1[temp1.size()-2]=='.')
        {
/*             cout << "hello" << "\n"; */
        }
        else if(is_dir(temp1.c_str()))
        {
/*             cout << "hello1" << "\n"; */
            temp=destination_dir+"/"+dirp->d_name;
            q.push(temp);
            q1.push(source_dir+"/"+dirp->d_name);
            mkdir(temp.c_str(),(mode_t)0777);
        }

	}
    v.push_back(q.front());
    q.pop();
    copy_files(v);
    v.clear();

	closedir(dp);
    }

    
}

vector<string> search(string file_name_to_find , string direc)
{
    
    string temp;
    DIR *dp;
	struct dirent *dirp;

	if((dp = opendir(direc.c_str())) == NULL )
		fprintf(stderr,"unable to open source directory");
	while(dirp = readdir(dp))
	{
        temp=direc+"/"+dirp->d_name;
        if(is_file(temp.c_str()))
        {
            if(file_name_to_find == dirp->d_name)
            search_list_result.push_back(temp);
        }
        else if(temp[temp.size()-1]=='.' || temp[temp.size()-1]=='.' && temp[temp.size()-2]=='.')
        {
            if(file_name_to_find == dirp->d_name)
                search_list_result.push_back(temp);
        }
        else if(is_dir(temp.c_str()))
        {
            if(file_name_to_find == dirp->d_name)
                search_list_result.push_back(temp);
            search(file_name_to_find,temp);
        }

	}
	closedir(dp);

    return search_list_result; 
} 


void create_dir(string a,string b)
{
    mkdir((b+"/"+a).c_str(),0777);
}

void rename_files(string a,string b)
{
    rename(a.c_str(),b.c_str());
}
void create_file(string a,string b)
{
    string temp=b+"/"+a;
    out = fopen(temp.c_str(), "w");
    if (out == NULL)
    {
        exit(0);
    }
}


/* void move_files()
{
    copy_files();
    delete_file();
}

void move_dir()
{
    copy_dir();
    delete_dir();
}

void delete_file()
{
    string filename;
    remove(filename.c_str());
}

void delete_dir(string direc)
{
    
}

void goto_directory()
{
    string filename;
    chdir(filename);
} */



bool is_file(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
}

bool is_dir(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

void Print_Search_Result()
{
    for(int i=0;i<search_list_result.size();i++)
        cout << search_list_result[i] << "\n";

    search_list_result.clear();
}


void Clear_Command_Mode_Params()
{
    input_command.clear();
    str_command_param="";
    operation="";
    search_list_result.clear();
}

void Error_Handler(string e)
{
    cout << e;
}