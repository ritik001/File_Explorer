#include "File_Explorer_Main.h"

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

void MoveBack_ParentDir()
{
    cur_dir=cur_dir.substr(0,cur_dir.find_last_of("/"));
    chdir(cur_dir.c_str());
    Listing_files(cur_dir);
}