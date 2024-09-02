#include <iostream>
#include <vector>
#include <io.h>

using namespace std;

void get_need_file(string path, vector<string>& file, string ext);

int main() {
    string file_path = R"(D:\)";
    vector<string> my_file;
    string need_extension = ".7z";
    get_need_file(file_path, my_file, need_extension);
    for (int i = 0; i < my_file.size(); i++)
    {
        cout << "File " << i+1 << " is:" << endl;
        cout << my_file[i] << endl;
    }
    if (my_file.size() == 0)
    {
        cout << "No file can be found!" << endl;
    }
    else
    {
        cout << endl << "Find " << my_file.size() << " file(s)." << endl;
    }
    return 0;
}

void get_need_file(string path, vector<string>& file, string ext)
{
    intptr_t file_handle = 0;
    struct _finddata_t file_info;
    string temp;
    if ((file_handle = _findfirst(temp.assign(path).append("/*" + ext).c_str(), &file_info)) != -1)
    {
        do
        {
            file.push_back(temp.assign(path).append("/").append(file_info.name));
        } while (_findnext(file_handle, &file_info) == 0);
        _findclose(file_handle);
    }
}