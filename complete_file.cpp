#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <limits.h>
#include <dirent.h>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <iostream>
using namespace std;

char cwd[PATH_MAX];
vector<string> directory_list;
vector<string> files_list;
vector<char> mode_list;
vector<string> :: iterator it;


#define cursordownward(x) printf("\033[%dB", (x))
#define cursorupward(x) printf("\033[%dA", (x))
#define cursorforward(x) printf("\033[%dC", (x))
#define cursorbackward(x) printf("\033[%dD", (x))

#define KEY_ESCAPE 27
#define KEY_ENTER 10
#define KEY_UP 0x0105
#define KEY_DOWN 0x0106
#define KEY_LEFT 0x0107
#define KEY_RIGHT 0x0108

bool default_open;
bool flag_check_left_right;
bool scroll_check;
bool up_or_down_scroll;
bool command_mode=false;
string home;
string cur_dir;
int rows;
int columns;
int max_rows=0;
int max_columns=0;
int index_directory_list_position;
int index_directory_list_count;
int initial_pointer;
int final_pointer;

FILE *input;
FILE *output;
int count_files;
struct termios initial_settings,new_settings;

void OpenStatusBar();
void cursor_position();
void OpenFile(string);
void MoveBack_ParentDir();
void Listing_files(string);
void Window_Size();
void Open_New_Screen();
void HandleScrolling();
void Open_Non_Canonical_Mode();
void clear_screen();
void MoveCursorDown();
void MoveCursorUp();
void MoveCursorRight();
void MoveCursorLeft();
int kbget();
int kbesc();
int kbhit();
void PrintFileAttr();
void NormalMode(string);
void accept_input();
void accept_input_from_terminal_Normal_Mode();
void accept_input_from_terminal_Command_Mode();

/* Command Mode variables and functions */

vector<string> search_list_result;

string inp_command="";
vector<string> input_command;
string str_command_param="";
string operation;

bool is_dir(const char* path);
bool is_file(const char* path);

void copy_dir(string,string);
void copy_files(vector<string>);
void rename_files(string,string);
void create_file(string,string);
void create_dir(string,string);
void delete_file(string);
void delete_dir(string);
vector<string> search(string,string);

/* void move_files();
void move_dir(); */
void goto_directory(string);

void Error_Handler(string);
void Print_Search_Result();
void Clear_Command_Mode_Params();
void Execute_Command_Mode();


int main(int argc,char *argv[])
{

    scroll_check=false;
    getcwd(cwd,100);
    home=cwd;
    cur_dir=home;
    rows=0;
    columns=0;
    index_directory_list_position=-1;
    index_directory_list_count=0;
    count_files=0;
    directory_list=vector<string>();
    files_list=vector<string>();
    mode_list=vector<char>();
    flag_check_left_right=false;

    Open_Non_Canonical_Mode();
    if(argc != 2)
    {
        Listing_files(home);
    }
    else
    {
            Listing_files(home+"/"+string(argv[1]));
    }
    accept_input();

    return 0;
}


void Open_Non_Canonical_Mode()
{
	
if(!isatty(fileno(stdout)))
	fprintf(stderr,"You are not a terminal \n");

	input=fopen("/dev/tty","r");
	output=fopen("/dev/tty","w");
	if(!input || !output)
	{
		fprintf(stderr,"Unable to open this file");
		exit(1);
	}
	tcgetattr(fileno(input),&initial_settings);
	new_settings=initial_settings;
	new_settings.c_lflag &= ~ICANON;
	new_settings.c_lflag &= ~ECHO;
	new_settings.c_cc[VMIN]=1;
	new_settings.c_cc[VTIME]=0;
	new_settings.c_lflag &= ~ISIG;
	
	if(tcsetattr(fileno(input),TCSANOW,&new_settings)!=0)
		fprintf(stderr,"could not set attributes\n");

}

void Open_New_Screen()
{
	cout << "\033[?1049h\033[H" ;
    char ch;
    do{
        ch=getchar();
        putchar(ch);
    }while(int(ch) != 27);

	cout << "\033[?1049l";
}


void Window_Size()
{
	struct winsize max;
	ioctl(0,TIOCGWINSZ,&max);
	max_rows=max.ws_row;
	max_columns=max.ws_col;
}


void clear_screen()
{
    files_list.erase(files_list.begin(),files_list.end());
    rows=0;
	cout << "\33[2J";
    cout << "\33[H";
}


void NormalMode(string argv)
{

    if(flag_check_left_right==false)
    {
/*         cout << "fdf " << directory_list.size(); */
        directory_list.push_back(string(cwd));
        index_directory_list_position+=1;
        index_directory_list_count+=1;
    }
    else
    {}
    flag_check_left_right=false;

    cout << argv << "\n";
	DIR *dp;
	struct dirent *dirp;

	if((dp = opendir(argv.c_str())) == NULL )
		fprintf(stderr,"unable to open home directory");
	while(dirp = readdir(dp))
	{
		files_list.push_back(dirp->d_name);
	}
	closedir(dp);
	
	
}


void Listing_files(string argv)
{
    scroll_check=false; 
    clear_screen();
    mode_list.erase(mode_list.begin(),mode_list.end());
    getcwd(cwd,sizeof(cwd));
    NormalMode(argv);
    sort(files_list.begin(),files_list.end());
    Window_Size();
    initial_pointer=0;
    final_pointer=max_rows-2;
    if(files_list.size()<max_rows)
        PrintFileAttr();
    else
    {
        up_or_down_scroll=false;
        mode_list.assign(200,'#');
        scroll_check=true;
        rows=0;
        HandleScrolling();
    }
    if(default_open == false)
    {}
    else
        accept_input();


}


int kbhit()
{
    int ch ;
    ch = getchar();
    if (ch != -1) ungetc(ch, stdin);
    return ((ch != -1) ? 1 : 0);
}

int kbesc()
{
    int c;

    if (!kbhit) return KEY_ESCAPE;
    c = getchar();
    if (c == '[') {
        switch (getchar()) {
            case 'A':
                c = KEY_UP;
                break;
            case 'B':
                c = KEY_DOWN;
                break;
            case 'C':
                c = KEY_RIGHT;
                break;
            case 'D':
                c = KEY_LEFT;
                break;
            default:
                c = 0;
                break;
        }
    } else {
        c = 0;
    }
/*     if (c == 0) while (kbhit()) getchar(); */
    return c;
}

int kbget()
{
    int c;

    c = getchar();
    return (c == KEY_ESCAPE) ? kbesc() : c;
}

void MoveCursorLeft()
{
    if(index_directory_list_position == 0)
    {}
    else
    {
        flag_check_left_right=true;
        index_directory_list_position-=1;
        cur_dir=directory_list[index_directory_list_position];
        chdir(cur_dir.c_str());
        Listing_files(cur_dir); 
    }
}

void MoveCursorRight()
{
    if(index_directory_list_position == index_directory_list_count-1)
    {}
    else
    {
        flag_check_left_right=true;
        index_directory_list_position+=1;
        cur_dir=directory_list[index_directory_list_position];
        chdir(cur_dir.c_str());
        Listing_files(cur_dir); 
    }
}
void MoveCursorUp()
{
    if(scroll_check==true)
    {
        if(rows == 0 && initial_pointer == 0)
            {}
        else if(rows == initial_pointer)
        {
            initial_pointer-=1;
            final_pointer-=1;
            rows=initial_pointer;
            up_or_down_scroll=false;
            HandleScrolling();
        }
        else
            {rows=rows-1;    
            cursorupward(1);}
    }
    else
    {
        if(rows!=-1)
        {
            rows=rows-1;   
            cursorupward(1);
        }
        else{}
   }

}

void MoveCursorDown()
{
    if(scroll_check==true)
    {
        if(final_pointer==rows && (final_pointer+1)!=files_list.size())
        {
            initial_pointer+=1;
            final_pointer+=1;
            rows=final_pointer;
            up_or_down_scroll=true;
            HandleScrolling();
        }
        else if((rows+1)==files_list.size())
        {}
        else
            {rows=rows+1;
            cursordownward(1);}
    }
    else
    {
    if((int)rows<(int)files_list.size()-1)
    {rows=rows+1;
    //cout<<"hello";
	cursordownward(1);}
    }

}

void MoveBack_ParentDir()
{
    if(cur_dir==home)
    {}
    else
    {

    flag_check_left_right=false;
    cur_dir=cur_dir.substr(0,cur_dir.find_last_of("/"));
    chdir(cur_dir.c_str());
    Listing_files(cur_dir);
    }
}
void PrintFileAttr()
{
	struct stat fileInfo;
	struct passwd *pws;
	char mode[10];
    count_files=files_list.size();
	for(int i=0;i<count_files;i++)
	{
	stat(files_list.at(i).c_str(),&fileInfo);
	mode_t permission=fileInfo.st_mode;
	mode[0]=permission & S_IFDIR ? 'd' : '-';
    mode[1]=permission & S_IRUSR ? 'r' : '-';
    mode[2]=permission & S_IWUSR ? 'w' : '-';
    mode[3]=permission & S_IXUSR ? 'x' : '-';
    mode[4]=permission & S_IRGRP ? 'r' : '-';
    mode[5]=permission & S_IWGRP ? 'w' : '-';
    mode[6]=permission & S_IXGRP ? 'x' : '-';
    mode[7]=permission & S_IROTH ? 'r' : '-';
    mode[8]=permission & S_IWOTH ? 'w' : '-';
    mode[9]=permission & S_IXOTH ? 'x' : '-';

	mode_list.push_back(mode[0]);
	size_t file_size=fileInfo.st_size;
	string modified_time=ctime(&fileInfo.st_mtime);
	modified_time[modified_time.length()-1]='\0';

	auto group_id=getgrgid(fileInfo.st_gid);
	auto pws = getpwuid(fileInfo.st_uid);
	auto user_name=pws->pw_name;
	auto group_name=group_id->gr_name;

	cout << mode << " " << group_name << " " << user_name << " " << file_size << " " << modified_time << " ";
	cout << files_list[i] << "\n";

	}
    OpenStatusBar();
    rows=-1;
    count_files=0;
    cout << "\033[0;0H";

	
}


void OpenFile(string file)
{
	if(!fork())
    	execlp("gedit", "gedit", file, NULL);
}



void HandleScrolling()
{
    cout << "\33[2J";
	struct stat fileInfo;
	struct passwd *pws;
	char mode[10];
    count_files=files_list.size();
	for(int i=initial_pointer;i<=final_pointer;i++)
	{
	stat(files_list.at(i).c_str(),&fileInfo);
	mode_t permission=fileInfo.st_mode;
	mode[0]=permission & S_IFDIR ? 'd' : '-';
    mode[1]=permission & S_IRUSR ? 'r' : '-';
    mode[2]=permission & S_IWUSR ? 'w' : '-';
    mode[3]=permission & S_IXUSR ? 'x' : '-';
    mode[4]=permission & S_IRGRP ? 'r' : '-';
    mode[5]=permission & S_IWGRP ? 'w' : '-';
    mode[6]=permission & S_IXGRP ? 'x' : '-';
    mode[7]=permission & S_IROTH ? 'r' : '-';
    mode[8]=permission & S_IWOTH ? 'w' : '-';
    mode[9]=permission & S_IXOTH ? 'x' : '-';

/*     mode_list.push_back(mode[0]); */

    if(mode_list[i] == '#')
	    mode_list[i]=mode[0];
   else
        {}
	size_t file_size=fileInfo.st_size;
	string modified_time=ctime(&fileInfo.st_mtime);
	modified_time[modified_time.length()-1]='\0';

	auto group_id=getgrgid(fileInfo.st_gid);
	auto pws = getpwuid(fileInfo.st_uid);
	auto user_name=pws->pw_name;
	auto group_name=group_id->gr_name;

	cout << mode << " " << group_name << " " << user_name << " " << file_size << " " << modified_time << " ";
	cout << files_list[i] << "\n";

	}

    cursor_position();

}

void cursor_position()
{
    OpenStatusBar();
    if(up_or_down_scroll == false)
        cout << "\033[0;0H";
    else
    {
        int temp=final_pointer-initial_pointer+1;
        cout << "\033[" << temp << ";0H";
    }
}


void OpenStatusBar()
{
    if(command_mode == false)
        cout<<"\033[7m --- Normal Mode --- Press : to enter the Command Mode and q to exit from here  \033[m";
    else
        {
        default_open=true;
        cout << "\033[" << max_rows << ";0H";
        cout << "\033[K" ;
        cout << "Command Mode :";
        cout << "\033[" << max_rows << ";15H";
        accept_input();
        }
}




/* ################################################################################################################### */


void Execute_Command_Mode()
{
    int count=0;
    for(int i=0;i<inp_command.size();i++)
        {
            if(inp_command[i]=='\\')
            {}
            else if(inp_command[i]=='\"' && inp_command[i-1]!='\\')
                count=count+1;
            else if(inp_command[i]!=' ' || (inp_command[i]==' ' && count%2!=0))
                {
                str_command_param=str_command_param+inp_command[i];
                }
            else if(inp_command[i]==' ' && count%2==0)
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
/*     else if(operation == "move")
        move_files(); */
    else if(operation == "rename")
        rename_files(input_command[1],input_command[2]);
    else if(operation == "create_file")
        create_file(input_command[1],input_command[2]);
    else if(operation == "create_dir")
        create_dir(input_command[1],input_command[2]);
    else if(operation == "delete_file")
        delete_file(input_command[1]);
    else if(operation == "goto")
        goto_directory(input_command[1]);
    else if(operation == "search")
        {
            search_list_result=search(input_command[2],input_command[1]);
            Print_Search_Result();
        }
    else if(operation == "snapshot")
        Error_Handler("Wrong Operation.Please enter a valid one.");



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
    output = fopen(temp.c_str(), "w");
    if (output == NULL)
    {
        exit(0);
    }
}


void delete_file(string filename)
{
    remove(filename.c_str());
}

void delete_dir(string direc)
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
            delete_file(temp);
        }
        else if(temp[temp.size()-1]=='.' || temp[temp.size()-1]=='.' && temp[temp.size()-2]=='.')
        {
            rmdir(temp.c_str());
        }
        else if(is_dir(temp.c_str()))
        {
            delete_dir(temp);
            rmdir(direc.c_str());
        }

	}
    rmdir(direc.c_str());

	closedir(dp);
    
}


/* void move_files()
{
    copy_files();
    delete_file();
}

void move_dir(strinf filenamw)
{
    copy_dir(filename);
    delete_dir(filename);
}
*/

void goto_directory(string filename)
{
    Clear_Command_Mode_Params();
    command_mode=false;
    flag_check_left_right=false;
    cur_dir=filename;
    Listing_files(filename);
}



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


/* ##############################################################################################################################    */

void accept_input()
{
    if(command_mode == false)
        accept_input_from_terminal_Normal_Mode();
    else
        accept_input_from_terminal_Command_Mode();
}



void accept_input_from_terminal_Normal_Mode()
{
    int c;
    while (1) {
        c = kbget();
        if (c == KEY_ENTER) {
            if(files_list[rows] == cur_dir || rows==-1)
            {}
            else if(files_list[rows] == "..")
                {
                    MoveBack_ParentDir();
                }
            else if(files_list[rows] == ".")
            {}
            else if(mode_list[rows]=='d'){
            cur_dir=cur_dir+"/"+files_list[rows];
            chdir(cur_dir.c_str());
            Listing_files(cur_dir);
            }
            else
            {
                OpenFile(files_list[rows]);
            }       
        } 
        else if (c == KEY_RIGHT) {
            MoveCursorRight();
        } 
        else if (c == KEY_LEFT) {
            MoveCursorLeft();
        } 
        else if (c == KEY_UP) {

            MoveCursorUp();
        }
        else if(c == KEY_DOWN)
        {

            MoveCursorDown();
        }
        else if( char(c) == ':')
        {
            new_settings.c_lflag &= ECHO;	
	        if(tcsetattr(fileno(input),TCSANOW,&new_settings)!=0)
		        fprintf(stderr,"could not set attributes\n");
            command_mode=true;
            OpenStatusBar();
        }
        else if(char(c) == 'q')
        {
            tcsetattr(fileno(input),TCSANOW,&initial_settings);
            system("clear");
            exit(1);
        }
        else if(char(c) == 'h')
        {
            cur_dir=home;
            chdir(cur_dir.c_str());
            Listing_files(cur_dir);
        }
        else if(c==127)
        {
            MoveBack_ParentDir();
        }
        else{}
    }
}



void accept_input_from_terminal_Command_Mode()
{

    int c;
    while (1) {
        c = kbget();
        if(c == KEY_ESCAPE)
            {
            new_settings.c_lflag &= ~ECHO;	
	        if(tcsetattr(fileno(input),TCSANOW,&new_settings)!=0)
		        fprintf(stderr,"could not set attributes\n");
            command_mode=false;
            exit(1);
            }    
        else if(c==KEY_ENTER)
        {
            cout << " input :" << inp_command << "\n";
            Execute_Command_Mode();
        }
        else if(c>=0 && c<=126)
            {
                cout << (char)c;
                inp_command=inp_command+(char)c;
            }
        else if(c==127)
            {
                if(inp_command.length()!=0)
                {
                    printf("\b \b");
                    fflush(stdout);
                    inp_command=inp_command.substr(0,inp_command.size()-1);
                }
                else
                    {}
            }
        else
            {}
}

}