#include<filesystem>
#include<chrono>
#include<iostream>
#include<fstream>
#include<vector>
#include<ctime> 
#include<io.h>
using namespace std;


void testFileTraverse() {
    string dir = "./";
    filesystem::directory_iterator begin(dir);
    filesystem::directory_iterator end;
    cout << "遍历 " << dir << " 下所有文件，目录提示:" << endl;
    for (auto it = begin; it != end; ++it) {
        //it->path() 获取文件filesystem::path 对象
        string pathname = it->path().string();
        string name = it->path().filename().string();
        //判断文件是否为目录
        if (filesystem::is_directory(it->status())) {
            cout << pathname << " is a directory!" << endl;
            continue;
        }
        cout << "文件路径为：" << pathname << "\t" << "文件名为：" << name << endl;
    }
}


void recursionFileTraverse(ofstream &outfile,string dir) {
    filesystem::directory_iterator begin(dir);
    filesystem::directory_iterator end;
    cout << "遍历 " << dir << " 下所有文件，目录提示:" << endl;
    for (auto it = begin; it != end; ++it) {
        //it->path() 获取文件filesystem::path 对象
        string pathname = it->path().string();
        string name = it->path().filename().string();
        //判断文件是否为目录
        if (filesystem::is_directory(it->status())) {
            cout << pathname << " is a directory!" << endl;
            // if(pathname.compare("./.git") != 0)
            if(pathname.find(".git") == std::string::npos)
                recursionFileTraverse(outfile,pathname);
            continue;
        }
        outfile << pathname <<endl;
        cout << "文件路径为：" << pathname << "\t" << "文件名为：" << name << endl;
    }
}


void testFileLastTime() {    
    filesystem::path  file = "./Filesystem.cc";
    int64_t fsize = filesystem::file_size(file);
    auto mtime = filesystem::last_write_time(file);
    //time_t ftime = file_time_type::clock::to_time_t(filesystem::last_write_time("./a.cc"))
    cout << "文件大小为：" << to_string(fsize) << endl;
    //cout << "文件最后修改时间戳为：" << to_string(ftime) << endl;
    //time_t cftime = decltype(mtime)::clock::to_time_t(mtime);
    // auto cftime = chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(mtime));
    //cout << "文件最后修改时间戳为：" << asctime(localtime(&cftime)) << endl;
    // struct tm* stm_ptr;
    // stm_ptr = localtime(&cftime);
    // char buf[1024];
    // strftime(buf, 1024, "%Y-%m-%d %H:%M:%S", stm_ptr);
    // cout << "文件最后被修改时间为：" << buf << endl;
}

void testFilesystem(){
    cout << "Hello, c++17 filesystem!" <<endl;

   string abs_path1 = ".\\stable-diffusion.cpp";
   string abs_path2 = ".\\stable-diffusion.cpp\\CMakeLists.txt";
   string abs_path3 = ".\\stable-diffusion.cpp\\thirdparty";
   string abs_path4 = ".\\stable-diffusion.cpp\\thirdparty\\";
   string abs_path5 = ".\\stable-diffusion.cpp\\ggml";
   string abs_path6 = ".\\stable-diffusion.cpp\\ggml\\";

    // 1. path
    filesystem::path path1(abs_path1);
    filesystem::path path2(abs_path2);
    filesystem::path path3(abs_path3);
    filesystem::path path4(abs_path4);
    filesystem::path path5(abs_path5);
    filesystem::path path6(abs_path6);
   cout << "输出字符串形式的路径: " << path1.string() <<endl; // ./filesystem_demo
   cout << "文件名，不带扩展名: " << path1.stem() <<endl; // "filesystem_demo"
   cout << "返回文件扩展名: " << path1.extension() <<endl; // ""
   cout << "获取文件或目录的绝对路径: " << filesystem::absolute(path1) <<endl; // ./filesystem_demo
   cout << "当前路径: " << filesystem::current_path() <<endl; // 如果没有参数传入，则返回当前工作目录；否则，则将传入的目录设为当前工作目录
   cout << "拼接路径1: " << path5/"jjjj" <<endl;
   cout << "拼接路径2: " << path6/"jjjj" <<endl;


    // 2. 判断文件
    auto status = filesystem::status(path1);
   // cout << "文件状态-权限: " << status.permissions() <<endl;
   // cout << "文件状态-类型: " << status.type() <<endl;

   cout << "判断-目录是否存在: " << filesystem::is_directory(path1) <<endl; // 1
   cout << "判断-目录是否存在: " << filesystem::is_directory(path2) <<endl; // 0
   cout << "判断-目录是否为空: " << filesystem::is_empty(path1) <<endl; // 0
   cout << "判断-目录是否为空: " << filesystem::is_empty(path2) <<endl; // 0
   cout << "判断-是否为文件: " << filesystem::is_regular_file(path1) <<endl; // 0
   cout << "判断-是否为文件: " << filesystem::is_regular_file(path2) <<endl; // 1
   cout << "判断-是否为链接文件: " << filesystem::is_symlink(path1) <<endl; // 0
   cout << "判断-是否为链接文件: " << filesystem::is_symlink(path2) <<endl; // 0
   cout << "判断-是否存在: " << filesystem::exists(path1) <<endl; // 1
   cout << "判断-是否存在: " << filesystem::exists(path2) <<endl; // 1
//   exceptioncout << "文件大小: " << filesystem::file_size(path1) <<endl; // exception
   cout << "文件大小: " << filesystem::file_size(path2) <<endl; // 2961 bytes
   // cout << "最后修改时间: " << filesystem::last_write_time(path1) <<endl; // 1640914873 时间戳
   // cout << "最后修改时间: " << filesystem::last_write_time(path2) <<endl; // 1640914873 时间戳

    // 3. 操作文件
    filesystem::create_directory("C:\\Logs");
    filesystem::create_directories("C:\\Log\\aa\\bb\\cc");
    // filesystem::create_directory(path3); // 创建目录
    // filesystem::create_directories(path3); // 创建目录
    // filesystem::create_directories(path4); // 创建目录

//    filesystem::remove(path3); // 删除目录
   cout << "-----删除 " <<endl;
    string abs_remove_all_path = "C:\\Log";
    filesystem::path remove_all_path(abs_remove_all_path);
    filesystem::remove_all(remove_all_path); // 递归删除目录
   cout << "-----删除 " <<endl;

//    filesystem::rename(path3,path5); // 重命名目录(目录存在时)

//    filesystem::copy_file() // 复制文件
//    filesystem::copy_directory() // 复制目录


//    // 4. 遍历文件夹
    filesystem::directory_iterator end_iter1; // 遍历当前目录
    for(filesystem::directory_iterator file_iter(path1); file_iter != end_iter1; ++file_iter ) {
       cout << "file: " << *file_iter  <<endl;
    }
   cout << "-------" <<endl;
    filesystem::recursive_directory_iterator end_iter2; // 递归地遍历一个目录和子目录，也就是迭代整个目录结构下的所有文件
    for(filesystem::recursive_directory_iterator file_iter(path1); file_iter != end_iter2; ++file_iter ) {
       cout << "file: " << *file_iter  <<endl;
    }

    // 5. 文件流
    // // 头文件<fstream>定义的文件流不能将filesystem::path定义的目录作为参数。如果非要使用path对象打开文件流的话，那就添加头文件<boost/filesystem/fstream.hpp>
    filesystem::path p{"haha.txt"};
    ofstream ofs{p};
    ofs << "Hello, world!\n";   
}


// vector<filesystem::path> myDirectoryIterator(char * filePath)
// {
//     vector<filesystem::path> file_list;
//     for (auto& i : filesystem::directory_iterator("D:\\WorkZone\\")) {
//         cout << i.path().string()<< endl;
//         if(filesystem::is_directory(i))
//         {
//             file_list.push_back(i.path());
//         }else{
//             vector<filesystem::path> tmp = myDirectoryIterator(i.path().c_str());
//             file_list.insert(file_list.end(), tmp.begin(), tmp.end());
//         }
//     }
//     return file_list;
// }

/// <summary>
/// (递归,非递归) 的获取指定目录下所有的(文件或者目录)的(绝对路径或者文件名称)
/// </summary>
/// <param name="directoryPath">指定目录</param>
/// <param name="isFile">true:文件，false:目录</param>
/// <param name="returnFullPath">true表示完整路径，false仅表示名称</param>
/// <param name="recursive">false:非递归,true:递归</param>
/// <returns></returns>
std::vector<std::string> collectFileOrDirEntries(const std::string& directoryPath, bool isFile = true, bool returnFullPath = true, bool recursive = false)
{
    std::vector<std::string> entries;
    try
    {
        std::filesystem::path dirPath(directoryPath);
        if (recursive)  // 递归
        {
            for (auto& entry : std::filesystem::recursive_directory_iterator(dirPath))
            {
                if ((isFile && entry.is_regular_file()) || (!isFile && entry.is_directory()))
                {
                    if (returnFullPath)
                    {
                        entries.push_back(entry.path().string());
                    }
                    else
                    {
                        entries.push_back(entry.path().filename().make_preferred().string());
                    }
                }
            }
        }
        else // 非递归
        {
            for (auto& entry : std::filesystem::directory_iterator(dirPath))
            {
                if ((isFile && entry.is_regular_file()) || (!isFile && entry.is_directory()))
                {
                    if (returnFullPath)
                    {
                        entries.push_back(entry.path().string());
                    }
                    else
                    {
                        entries.push_back(entry.path().filename().make_preferred().string());
                    }
                }
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cout << "Error accessing directory: " << e.what() << std::endl;
    }
    return entries;
}


int main() {
    //io.h 遍历文件夹
    _finddata64i32_t fileInfo;
    intptr_t hFile=_findfirst("./*",&fileInfo);
    if (hFile == -1) {
        return -1;
    }
    do
    {
        cout << fileInfo.name << endl;
    } while (_findnext(hFile,&fileInfo)==0);
    
    //filesystem 遍历文件夹
    for (auto& i : filesystem::directory_iterator("./")) {
        cout << i.path().string()<< endl;
        if(filesystem::is_directory(i))
        {
            
        }
    }
    
    //filesystem 遍历文件夹以及子文件夹
    // 创建一个vector来存储目录中的文件列表
    vector<filesystem::path> file_list;
    for (auto& i : filesystem::recursive_directory_iterator ("./")) {
        cout << i.path().string()<< endl;
    }
    
    
    testFilesystem();

    // 对文件列表进行排序
    // 创建一个vector来存储目录中的文件列表
    // vector<filesystem::path> file_list;
    // filesystem::directory_iterator end_itr;
    // for (filesystem::directory_iterator itr("./"); itr != end_itr; ++itr)
    // {
    //     if (filesystem::is_regular_file(itr->status()))
    //     {
    //         file_list.push_back(itr->path());
    //     }
    // }
    // 对file_list进行排序
    // sort(file_list.begin(), file_list.end());
    // for (filesystem::path i : file_list) {
    //     cout << i.string()<< endl;
    // }
    
     testFileTraverse();
     testFileLastTime();
     
    ofstream outfile("Path.txt", ios::out);
    if (outfile) {
        recursionFileTraverse(outfile, "./");
        outfile.close();
        ifstream infile("Path.txt", ios::in);
        string line;
        if (infile.is_open()) {
            while (getline(infile, line)) {
                cout << line << endl;
            }
            infile.close();
        } else {
            cout << "Failed to open the file." << endl;
        }
	}else{
		cout << "Failed to write the file." << endl;
	}
   
     
    
     
    // 遍历目录下的所有文件
    // for (const auto& entry :filesystem::directory_iterator(m_imgPath))
    // {
    //     // 如果是文件，则输出文件名
    //     /*if (entry.is_regular_file())
    //     {
    //         m_imgFilelist.push_back(entry.path().filename().string());
    //     }*/
    //     // add file path
    //     if (entry.path().extension() == ".png")
    //     {
    //         m_imgFilelist.push_back(m_imgPath + entry.path().filename().string());
    //     }
    // }
    return 0;
}