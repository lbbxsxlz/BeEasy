#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>


long long get_dir_size(char* path)
{
	DIR *dir;
	struct dirent *dirp;
	struct stat f_stat;
	char filename[PATH_MAX];
	long long size = 0;
	
	dir = opendir(path);
	if (dir == NULL) {
		printf("open dir %s fail: %s, errno = %d \n", path, strerror(errno), errno);
		return -1;
	}	
	
	while(dirp = readdir(dir)) {
		if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) {
			continue;
		}
		
		memset(filename, 0, sizeof(filename));
		sprintf(filename, "%s/%s", path, dirp->d_name);
		memset(&f_stat, 0, sizeof(stat));
		if(stat(filename, &f_stat) < 0) {
			printf("get file attr fail: %s, errno = %d \n", strerror(errno), errno);
			closedir(dir);
			return -1;
		}
		
		if(S_ISDIR(f_stat.st_mode)) {
			size += get_dir_size(filename);
		} else {
			printf("file name : %s ,file_size = %d \n", filename, f_stat.st_size);
			size += f_stat.st_size;
		}	
	}
	
	closedir(dir);
	return size;
}
/* 删除path目录下的文件与目录 */
void deleteFileAndDir(char* path)
{
	DIR *dir;
	struct dirent *dirp;
	struct stat f_stat;
	char filename[PATH_MAX];
	
	dir = opendir(path);
	if (dir == NULL) {
		printf("open dir %s fail: %s, errno = %d \n", path, strerror(errno), errno);
		return;
	}	
	
	while(dirp = readdir(dir)) {
		if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) {
			continue;
		}
		
		memset(filename, 0, sizeof(filename));
		sprintf(filename, "%s/%s", path, dirp->d_name);
		memset(&f_stat, 0, sizeof(stat));
		if(stat(filename, &f_stat) < 0) {
			printf("get file attr fail: %s, errno = %d \n", strerror(errno), errno);
			closedir(dir);
			return;
		}
		
		if(S_ISDIR(f_stat.st_mode)) {
			deleteFileAndDir(filename);
			rmdir(filename);
		} else {
			remove(filename);
		}	
	}
	
	closedir(dir);
}

void deleteFile(char* path)
{
	DIR *dir = NULL;
	struct dirent *dirp;
	struct stat f_stat;
	char filename[PATH_MAX];
	
	struct tm mtime;
	struct tm today;
	time_t now;
	
	time(&now);
	localtime_r(&now, &today);
	
	dir = opendir(path);
	if (dir == NULL) {
		printf("open dir %s fail: %s, errno = %d \n", path, strerror(errno), errno);
		return;
	}	
	
	while(dirp = readdir(dir)) {
		if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) {
			continue;
		}
		
		memset(filename, 0, sizeof(filename));
		sprintf(filename, "%s/%s", path, dirp->d_name);
		memset(&f_stat, 0, sizeof(stat));
		if(stat(filename, &f_stat) < 0) {
			printf("get file attr fail: %s, errno = %d \n", strerror(errno), errno);
			closedir(dir);
			return;
		}
		
		//目录直接删除
		if(S_ISDIR(f_stat.st_mode)) {
			deleteFileAndDir(filename);
			rmdir(filename);
			continue;
		}
		
		//单个文件超过设定大小删除
		if (f_stat.st_size > 314572800)
		{
			remove(filename);
			continue;
		}
		
		//文件修改时间超过3天删除
		localtime_r(&f_stat.st_mtime, &mtime);
		if (today.tm_yday - mtime.tm_yday > 3)
		{
			printf("delete file \n");
			remove(filename);
		}	
	}
	
	closedir(dir);
}

/* 删除path目录最大的文件 */
void deleteMaxFile(char* path)
{
	DIR *dir = NULL;
	struct dirent *dirp;
	struct stat f_stat;
	char filename[PATH_MAX];
	char deleteName[PATH_MAX];
	int i = 0;
	long long size;
	
	dir = opendir(path);
	if (dir == NULL) {
		printf("open dir %s fail: %s, errno = %d \n", path, strerror(errno), errno);
		return;
	}	
	
	while((dirp = readdir(dir))) {
		if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) {
			continue;
		}
		
		memset(filename, 0, sizeof(filename));
		sprintf(filename, "%s/%s", path, dirp->d_name);
		memset(&f_stat, 0, sizeof(stat));
		if(stat(filename, &f_stat) < 0) {
			printf("get file attr fail: %s, errno = %d \n", strerror(errno), errno);
			closedir(dir);
			return;
		}
		
		if (i == 0)
		{
			memset(deleteName, 0, sizeof(deleteName));
			strncpy(deleteName, filename, strlen(filename));
			size = f_stat.st_size;
		}
		else
		{
			if (size < f_stat.st_size)
			{
				memset(deleteName, 0, sizeof(deleteName));
				strncpy(deleteName, filename, strlen(filename));
				size = f_stat.st_size;
			}
			i++;
		}	
	}
	printf("delete file : %s \n", deleteName);
	remove(deleteName);

	closedir(dir);
}

int main(int argc, char* argv[])
{
	long long dir_size;
	dir_size = get_dir_size(argv[1]);
	printf("%s dir  size = %ld \n", argv[1], dir_size);
	
	deleteMaxFile(argv[1]);
	//deleteAllFile(argv[1]);
	return 0;
}
