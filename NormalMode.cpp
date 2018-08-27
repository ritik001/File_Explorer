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
vector<string> directory_list=vector<string>();
vector<string> files_list=vector<string>();
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

void MoveCursorLeft();
void MoveCursorRight();
void MoveCursorUp();
void MoveCursorDown();
void Open_Non_Canonical_Mode();
void clear_screen();
void Window_Size();
void Listing_files(string);


string home="/home/ritik/Code/Ritik/Termios";
string cur_dir=home;
int rows=0;
int columns=0;
int index_directory_list_position=-1;
int index_directory_list_count=0;

FILE *input;
FILE *output;
int count_files=0;
struct termios initial_settings,new_settings;

void NormalMode(string argv)
{
    it=find(directory_list.begin(),directory_list.end(),string(cwd));
    if(it == directory_list.end())
    {
        directory_list.push_back(string(cwd));
        index_directory_list_position+=1;
        index_directory_list_count+=1;
    }
    else
    {
    }

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
    rows=count_files;
    count_files=0;

	
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
    cout << "hello" << "\n";
    cout << "dfdfdfd " << index_directory_list_position << "\n";
    if(index_directory_list_position == 0)
    {    cout << "hello1" << "\n";
    }
    else{
        index_directory_list_position-=1;
        cur_dir=directory_list[index_directory_list_position];
        chdir(cur_dir.c_str());
            cout << "hello3" << "\n";
        Listing_files(cur_dir); 
    }
}

void MoveCursorRight()
{
	cursorbackward(1);
}

void MoveCursorUp()
{
    if(rows!=0)
        rows=rows-1;
    cursorupward(1);
}

void MoveCursorDown()
{
    rows=rows+1;
	cursordownward(1);
}

void clear_screen()
{
    files_list.erase(files_list.begin(),files_list.end());
    rows=0;
	cout << "\33[2J";
    cout << "\33[H";
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
	rows=max.ws_row;
	columns=max.ws_col;
}

void Listing_files(string argv)
{
    clear_screen();
    getcwd(cwd,sizeof(cwd));
    NormalMode(argv);
    sort(files_list.begin(),files_list.end());
    PrintFileAttr();
}



int main(int argc,char *argv[])
{
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

            cur_dir=cur_dir+"/"+files_list[rows];
            chdir(cur_dir.c_str());
            Listing_files(cur_dir);       
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
			Open_New_Screen();
        }
        else if(char(c) == 'q')
        {
            tcsetattr(fileno(input),TCSANOW,&initial_settings);
        }
        else if(char(c) == 'h')
        {
            cur_dir=home;
            chdir(cur_dir.c_str());
            Listing_files(cur_dir);
        }
    }
    return 0;
}

