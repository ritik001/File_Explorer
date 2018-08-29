#include "File_Explorer_Main.h"

int main(int argc,char *argv[])
{
    string home="/home/ritik/Code/Ritik/Termios";
    string cur_dir=home;
    int rows=0;
    int columns=0;
    int index_directory_list_position=-1;
    int index_directory_list_count=0;
    int count_files=0;
    vector<string> directory_list=vector<string>();
    vector <string> files_list=vector<string>();
    vector <char> mode_list=vector<char>();
    bool flag_check_left_right=false;
    FILE *input;
    FILE *output;
    struct termios initial_settings,new_settings;

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
            if(files_list[rows] == "..")
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
			Open_New_Screen();
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
	rows=max.ws_row;
	columns=max.ws_col;
}


void clear_screen()
{
    files_list.erase(files_list.begin(),files_list.end());
    mode_list.erase(mode_list.begin(),mode_list.end());
    rows=0;
	cout << "\33[2J";
    cout << "\33[H";
}