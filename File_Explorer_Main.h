/**
 * Code By: Ritik Agarwal
 * Roll Number: 2018202017
 * Subject: Operating System- Assignment File Explorer
**/

#ifndef FileExplorer_LIB
#define FileExplorer_LIB

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

extern char cwd[PATH_MAX];
extern vector<string> directory_list;
extern vector<string> files_list;
extern  vector<string> :: iterator it;


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

void MoveBack_ParentDir();
void MoveCursorLeft();
void MoveCursorRight();
void MoveCursorUp();
void MoveCursorDown();
void Open_Non_Canonical_Mode();
void clear_screen();
void Window_Size();
void Listing_files(string);


extern char cwd[PATH_MAX];
extern string home;
extern string cur_dir;
extern int rows;
extern int columns;
extern int index_directory_list_position;
extern int index_directory_list_count;

extern FILE *input;
extern FILE *output;
extern int count_files;
extern struct termios initial_settings,new_settings;

void Listing_files(string);
void Window_Size();
void Open_New_Screen();
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

#endif