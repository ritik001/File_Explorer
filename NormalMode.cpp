#include "File_Explorer_Main.h"


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
    clear_screen();
    getcwd(cwd,sizeof(cwd));
    NormalMode(argv);
    sort(files_list.begin(),files_list.end());
    PrintFileAttr();
}



















