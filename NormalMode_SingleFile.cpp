#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <bits/stdc++.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <limits.h>
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

bool flag_check_left_right;
bool scroll_check;
bool up_or_down_scroll=false;
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

int main(int argc,char *argv[])
{
    scroll_check=false;
    home="/home/ritik/Code/Ritik/Termios";
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
            Listing_files(string(argv[1]));
		}

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
    }

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
    if (c == 0) while (kbhit()) getchar();
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
        cout << "\033[" << max_rows << ";0H";
        cout << "\033[K" ;
        cout << "Command Mode :";
        }
}

