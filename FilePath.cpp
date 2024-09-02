#include <io.h>
#include <iostream>  
using namespace std;
 
#define MAX_PATH_LENGTH 100
 
int Traverse(char dir[]);
 
int main()
{
    char dir[MAX_PATH_LENGTH] = "D:\\VscodiumZone\\*.*";
 
    Traverse(dir);
 
    return 0;
}
 
int Traverse(char dir[])
{
    intptr_t handle;
    _finddata_t findData;
 
    handle = _findfirst(dir, &findData);
    if (handle == -1)
    {
        cout << "no file exsit\n";
        return -1;
    }
 
    do
    {
        if ((findData.attrib & _A_SUBDIR) && (strcmp(findData.name, ".") != 0) && (strcmp(findData.name, "..") != 0))
        {
            //it is a directory
            cout << "subdir:" << findData.name << endl;
            char sub_dir[MAX_PATH_LENGTH] = { 0 };
            string s(dir);
            sprintf_s(sub_dir, "%s%s\\*.*", s.substr(0, s.length() - 3).c_str(), findData.name);
            Traverse(sub_dir);
        }
        else if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
        {
            //it is . or .. , do nothing
        }
        else
        {
            //it is a file
            cout << "file:" << findData.name << endl;
        }
    } while (_findnext(handle, &findData) == 0);
 
    _findclose(handle);
}