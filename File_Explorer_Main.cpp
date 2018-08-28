#include "File_Explorer_Main.h"

int main(int argc,char *argv[])
{
    home="/home/ritik/Code/Ritik/Termios";
    cur_dir=home;
    rows=0;
    columns=0;
    index_directory_list_position=-1;
    index_directory_list_count=0;
    count_files=0;
    directory_list=vector<string>();
    files_list=vector<string>();

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
                    getcwd(cwd,sizeof(cwd));
                }
            else if(files_list[rows] == ".")
            {}
            else{
            cur_dir=cur_dir+"/"+files_list[rows];
            chdir(cur_dir.c_str());
            Listing_files(cur_dir);
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
    rows=0;
	cout << "\33[2J";
    cout << "\33[H";
}