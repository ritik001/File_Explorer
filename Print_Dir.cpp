#include "File_Explorer_Main.h"

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