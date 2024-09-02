#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>

int list_dir(char * path, int recursive)
{
	DIR * p_dir;
	struct dirent * entry;
	size_t len;
	char * sub_path;
	
	p_dir = opendir(path);
	if (p_dir == NULL)
	{
		printf("Can not open %s/n", path);
		return -1;
	}
	while((entry = readdir(p_dir)) != NULL)
	{
		len = strlen(path) + strlen(entry->d_name) + 3;
		sub_path = calloc(len, 1);
		if (sub_path == NULL)
		{
			printf("out of memory/n");
			closedir(p_dir);
			return -1;
		}

		strcpy(sub_path, path);
		strcat(sub_path, "/");
		strcat(sub_path, entry->d_name);
		printf("%s\n", sub_path);
		if (entry->d_type == DT_DIR && recursive != 0 && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
			list_dir(sub_path, recursive);
		}
		free(sub_path);
	}
	closedir(p_dir);
	return -1;
}

int main()
{
	char path[1024];
	gets(path);
	list_dir(path, 1);
	return 0;
}