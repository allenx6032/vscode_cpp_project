
#include <fcntl.h>
#include <Shlobj.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <shellapi.h>
#include <windows.h>
#include <iostream>
#include <sstream>

#include "quick_ProjectConfig.h"
#include "quick_ProjectCommon.h"

#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif

#if defined(_WINDOWS)
#define DIRECTORY_SEPARATOR "\\"
#define DIRECTORY_SEPARATOR_CHAR '\\'
#else
#define DIRECTORY_SEPARATOR "/"
#define DIRECTORY_SEPARATOR_CHAR '/'
#endif


ProjectConfig::ProjectConfig()
    : _isWelcome(false)
    , _scriptFile("/src/main.lua")
    , _writablePath("")
    , _packagePath("")
    , _frameSize(960, 640)
    , _frameScale(1.0f)
    , _showConsole(true)
    , _writeDebugLogToFile(true)
	, _isMultiLogFiles(false)
    , _windowOffset(0, 0)
    , _debuggerType(kCCLuaDebuggerNone)
    , _isAppMenu(true)
    , _isResizeWindow(false)
    , _isRetinaDisplay(false)
{
    normalize();
}

bool ProjectConfig::isWelcome() const
{
    return _isWelcome;
}

void ProjectConfig::resetToWelcome()
{
    _isWelcome = true;
    auto path = getQuickCocos2dxRootPath();
    path.append("welcome/");
    
    std::cout << "resetToWelcome path = " << path << std::endl;
    //path.append("quick/samples/ccsloader");
    makeNormalizePath(&path);
    setProjectDir(path);
    setWritablePath(path);
    setScriptFile(path + "src/main.lua");
    setFrameSize(cocos2d::Size(640, 960));
    setFrameScale(1.0f);
    setPackagePath("");
    setShowConsole(false);
    setWindowOffset(cocos2d::Vec2::ZERO);
    setWriteDebugLogToFile(false);
    _isAppMenu = false;
    _isResizeWindow = false;
    _isRetinaDisplay = true;
}

void ProjectConfig::resetToCreator()
{
    _isWelcome = true;
    auto path = getQuickCocos2dxRootPath();
    path.append("creator/");
    std::cout << "resetToCreatorpath = " << path << std::endl;
    makeNormalizePath(&path);
    setProjectDir(path);
    setWritablePath(path);
    setScriptFile(path + "src/main.lua");
    setFrameSize(cocos2d::Size(960, 640));
    setFrameScale(1.0f);
    setPackagePath("");
    setShowConsole(false);
    setWindowOffset(cocos2d::Vec2::ZERO);
    setWriteDebugLogToFile(false);
    _isAppMenu = true;
    _isResizeWindow = true;
    _isRetinaDisplay = true;
}

string ProjectConfig::getProjectDir() const
{
    return _projectDir;
}

void ProjectConfig::setProjectDir(const string &projectDir)
{
    _projectDir = projectDir;
    normalize();
}

string ProjectConfig::getScriptFile() const
{
    
    std::cout << "getScriptFile path = " << _scriptFile.c_str() << std::endl;
    return _scriptFile;
}

string ProjectConfig::getScriptFileRealPath() const
{
    return replaceProjectDirToFullPath(_scriptFile);
}

void ProjectConfig::setScriptFile(const string &scriptFile)
{
    _scriptFile = scriptFile;
    normalize();
}

string ProjectConfig::getWritablePath() const
{
    return _writablePath;
}

string ProjectConfig::getWritableRealPath() const
{
    return replaceProjectDirToFullPath(_writablePath);
}

void ProjectConfig::setWritablePath(const string &writablePath)
{
    _writablePath = writablePath;
    normalize();
}

string ProjectConfig::getPackagePath() const
{
    return _packagePath;
}

string ProjectConfig::getNormalizedPackagePath() const
{
    // replace $(PROJDIR)
    auto path = _packagePath;
    auto pos = string::npos;
    while ((pos = path.find("$(PROJDIR)")) != string::npos)
    {
        path = path.substr(0, pos) + _projectDir + path.substr(pos + 10);
    }
    auto len = path.length();
    if (len && path[len - 1] != ';')
    {
        path.append(";");
    }
    path.append(";");
    makeNormalizePath(&path, "/");
    return path;
}

void ProjectConfig::setPackagePath(const string &packagePath)
{
    _packagePath = packagePath;
}

void ProjectConfig::addPackagePath(const string &packagePath)
{
    if (packagePath.length())
    {
        if (_packagePath.length())
        {
            _packagePath.append(";");
        }
        _packagePath.append(packagePath);
        normalize();
    }
}

vector<string> ProjectConfig::getPackagePathArray() const
{
    vector<string> arr;

    size_t pos = string::npos;
    size_t prev = 0;
    while ((pos = _packagePath.find_first_of(";", pos + 1)) != string::npos)
    {
        auto path = _packagePath.substr(prev, pos - prev);
        if (path.length() > 0) arr.push_back(path);
        prev = pos + 1;
    }
    auto path = _packagePath.substr(prev);
    if (path.length() > 0) arr.push_back(path);
    return arr;
}

cocos2d::Size ProjectConfig::getFrameSize() const
{
    return _frameSize;
}

void ProjectConfig::setFrameSize(const cocos2d::Size &frameSize)
{
    if (frameSize.width > 0 && frameSize.height > 0)
    {
        _frameSize = frameSize;
    }
}

bool ProjectConfig::isLandscapeFrame() const
{
    return _frameSize.width > _frameSize.height;
}

bool ProjectConfig::isPortraitFrame() const
{
    return _frameSize.width < _frameSize.height;
}

void ProjectConfig::changeFrameOrientation()
{
    float w = _frameSize.width;
    _frameSize.width = _frameSize.height;
    _frameSize.height = w;
}

void ProjectConfig::changeFrameOrientationToPortait()
{
    if (isLandscapeFrame()) changeFrameOrientation();
}

void ProjectConfig::changeFrameOrientationToLandscape()
{
    if (!isLandscapeFrame()) changeFrameOrientation();
}

float ProjectConfig::getFrameScale() const
{
    return _frameScale;
}

void ProjectConfig::setFrameScale(float frameScale)
{
    if (frameScale > 0)
    {
        _frameScale = frameScale;
    }
}

bool ProjectConfig::isShowConsole() const
{
    return _showConsole;
}

void ProjectConfig::setShowConsole(bool showConsole)
{
    _showConsole = showConsole;
}

bool ProjectConfig::isWriteDebugLogToFile() const
{
    return _writeDebugLogToFile;
}

void ProjectConfig::setWriteDebugLogToFile(bool writeDebugLogToFile)
{
    _writeDebugLogToFile = writeDebugLogToFile;
}

string ProjectConfig::getDebugLogFilePath() const
{
    auto path(getWritablePath());
    // if not set "-writable" in command line, log to ProjectDir.
    if (path.compare(0, 10, "$(PROJDIR)") == 0) {
        path = getProjectDir();
    }
    path.append("debug");
    if (_isMultiLogFiles)
    {
        time_t t = time(0);
        char tmp[64] = {};
        strftime(tmp, sizeof(tmp), "_%Y%m%d_%H%M%S", localtime(&t));
        path.append(tmp);
    }
    path.append(".log");
    return path;
}

cocos2d::Vec2 ProjectConfig::getWindowOffset() const
{
    return _windowOffset;
}

void ProjectConfig::setWindowOffset(const cocos2d::Vec2 &windowOffset)
{
    _windowOffset = windowOffset;
}

int ProjectConfig::getDebuggerType() const
{
    return _debuggerType;
}

void ProjectConfig::setDebuggerType(int debuggerType)
{
    _debuggerType = debuggerType;
}

/*void ProjectConfig::parseCommandLine(const vector<string> &args)
{
    auto it = args.begin();
    while (it != args.end())
    {
        string arg = *it;
        std::cout << "ProjectConfig.parseCommandLine path = " << arg << std::endl;
        if (arg.compare("-quick") == 0)
        {
            ++it;
            if (it == args.end()) break;
            setQuickCocos2dxRootPath((*it).c_str());
        }
        else if (arg.compare("-workdir") == 0)
        {
            ++it;
            if (it == args.end()) break;
            setProjectDir(*it);
            if (_writablePath.length() == 0) setWritablePath(*it);
        }
        else if (arg.compare("-writable") == 0)
        {
            ++it;
            if (it == args.end()) break;
            setWritablePath(*it);
        }
        else if (arg.compare("-file") == 0)
        {
            ++it;
            if (it == args.end()) break;
            setScriptFile(*it);
        }
        else if (arg.compare("-package.path") == 0)
        {
            ++it;
            if (it == args.end()) break;
            setPackagePath(*it);
        }
        else if (arg.compare("-landscape") == 0)
        {
            setFrameSize(cocos2d::Size(DEFAULT_HEIGHT, DEFAULT_WIDTH));
        }
        else if (arg.compare("-portrait") == 0)
        {
            setFrameSize(cocos2d::Size(DEFAULT_WIDTH, DEFAULT_HEIGHT));
        }
        else if (arg.compare("-size") == 0)
        {
            ++it;
            if (it == args.end()) break;
            const string& sizeStr(*it);
            size_t pos = sizeStr.find('x');
            int width = 0;
            int height = 0;
            if (pos != sizeStr.npos && pos > 0)
            {
                string widthStr, heightStr;
                widthStr.assign(sizeStr, 0, pos);
                heightStr.assign(sizeStr, pos + 1, sizeStr.length() - pos);
                width = atoi(widthStr.c_str());
                height = atoi(heightStr.c_str());
                setFrameSize(cocos2d::Size(width, height));
            }
        }
        else if (arg.compare("-scale") == 0)
        {
            ++it;
            if (it == args.end()) break;
            float scale = atof((*it).c_str());
            setFrameScale(scale);
        }
        else if (arg.compare("-write-debug-log") == 0)
        {
            setWriteDebugLogToFile(true);
        }
        else if (arg.compare("-disable-write-debug-log") == 0)
        {
            setWriteDebugLogToFile(false);
        }
		else if (arg.compare("-multi-log-files") == 0)
		{
			_isMultiLogFiles = true;
		}
        else if (arg.compare("-console") == 0)
        {
            setShowConsole(true);
        }
        else if (arg.compare("-disable-console") == 0)
        {
            setShowConsole(false);
        }
        else if (arg.compare("-offset") == 0)
        {
            ++it;
            if (it == args.end()) break;
            setWindowOffset(cocos2d::PointFromString((*it).c_str()));
        }
        else if (arg.compare("-app-menu") == 0)
        {
            _isAppMenu = true;
        }
        else if (arg.compare("-resize-window") == 0)
        {
            _isResizeWindow = true;
        }
        else if (arg.compare("-retina-display") == 0)
        {
            _isRetinaDisplay = true;
        }

        ++it;
    }
    setDebuggerType(kCCLuaDebuggerNone);
}*/

// string ProjectConfig::makeCommandLine(unsigned int mask /* = kProjectConfigAll */) const
string ProjectConfig::makeCommandLine(unsigned int mask) const
{
    stringstream buff;

    if (mask & kProjectConfigQuickRootPath)
    {
        auto path = getQuickCocos2dxRootPath();
        if (path.length())
        {
            buff << " -quick ";
            buff << path;
        }
    }

    if (mask & kProjectConfigProjectDir)
    {
        auto path = getProjectDir();
        if (path.length())
        {
            buff << " -workdir ";
            buff << path;
        }
    }

    if (mask & kProjectConfigScriptFile)
    {
        auto path = getScriptFileRealPath();
        if (path.length())
        {
            buff << " -file ";
            buff << path;
        }
    }

    if (mask & kProjectConfigWritablePath)
    {
        auto path = getWritableRealPath();
        if (path.length())
        {
            buff << " -writable ";
            buff << path;
        }
    }

    if (mask & kProjectConfigPackagePath)
    {
        auto packagePath = getPackagePath();
        if (packagePath.length())
        {
            buff << " -package.path ";
            buff << packagePath;
        }
    }

    if (mask & kProjectConfigFrameSize)
    {
        buff << " -size ";
        buff << (int)getFrameSize().width;
        buff << "x";
        buff << (int)getFrameSize().height;
    }

    if (mask & kProjectConfigFrameScale)
    {
        if (getFrameScale() < 1.0f)
        {
            buff << " -scale ";
            buff.precision(2);
            buff << getFrameScale();
        }
    }

    if (mask & kProjectConfigWriteDebugLogToFile)
    {
        if (isWriteDebugLogToFile())
        {
            buff << " -write-debug-log";
        }
        else
        {
            buff << " -disable-write-debug-log";
        }
    }

    if (mask & kProjectConfigShowConsole)
    {
        if (isShowConsole())
        {
            buff << " -console";
        }
        else
        {
            buff << " -disable-console";
        }
    }

    if (mask & kProjectConfigWindowOffset)
    {
        if (_windowOffset.x != 0 && _windowOffset.y != 0)
        {
            buff << " -offset {";
            buff << (int)_windowOffset.x;
            buff << ",";
            buff << (int)_windowOffset.y;
            buff << "}";
        }
    }

    // No debugger
    buff << " -disable-debugger";

    string result = buff.str();
    while (result.at(0) == ' ')
    {
        result = result.assign(result, 1, result.length());
    }

    return result;
}

bool ProjectConfig::isAppMenu() const
{
    return _isAppMenu;
}

bool ProjectConfig::isResizeWindow() const
{
    return _isResizeWindow;
}

bool ProjectConfig::isRetinaDisplay() const
{
    return _isRetinaDisplay;
}

bool ProjectConfig::validate() const
{
    auto utils = cocos2d::FileUtils::getInstance();
    if (!utils->isDirectoryExist(_projectDir)) return false;
    if (!utils->isDirectoryExist(getWritableRealPath())) return false;
    if (!utils->isFileExist(getScriptFileRealPath())) return false;
    return true;
}

void ProjectConfig::dump()
{
    CCLOG("Project Config:");
    CCLOG("    quick root path: %s", getQuickCocos2dxRootPath().c_str());
    CCLOG("    project dir: %s", _projectDir.c_str());
    CCLOG("    writable path: %s", _writablePath.length() ? _writablePath.c_str() : "-");
    CCLOG("    script file: %s", _scriptFile.c_str());
    CCLOG("    package.path: %s", _packagePath.length() ? _packagePath.c_str() : "-");
    CCLOG("    frame size: %0.0f x %0.0f", _frameSize.width, _frameSize.height);
    CCLOG("    frame scale: %0.2f", _frameScale);
    CCLOG("    show console: %s", _showConsole ? "YES" : "NO");
    CCLOG("    write debug log: %s", _writeDebugLogToFile ? "YES" : "NO");
	CCLOG("    create multi log files: %s", _isMultiLogFiles ? "YES" : "NO");
    CCLOG("    debugger: none");
    CCLOG("\n\n");
}

void ProjectConfig::normalize()
{

    
    CCLOG("_projectDir %s\n",_projectDir.c_str());
    CCLOG("_scriptFile %s\n",_scriptFile.c_str());
    CCLOG("_writablePath %s\n",_writablePath.c_str());
    CCLOG("_packagePath %s\n",_writablePath.c_str());
    makeNormalizePath(&_projectDir);
    makeNormalizePath(&_scriptFile);
    makeNormalizePath(&_writablePath);
    makeNormalizePath(&_packagePath);

    // projectDir
    size_t len = _projectDir.length();
    if (len > 0 && _projectDir[len - 1] != DIRECTORY_SEPARATOR_CHAR)
    {
        _projectDir.append(DIRECTORY_SEPARATOR);
        len++;
    }

    // writablePath
    if (len > 0 && _writablePath.length() == 0)
    {
        _writablePath = _projectDir;
    }
    len = _writablePath.length();
    if (len > 0 && _writablePath[len - 1] != DIRECTORY_SEPARATOR_CHAR)
    {
        _writablePath.append(DIRECTORY_SEPARATOR);
    }
    _writablePath = replaceProjectDirToMacro(_writablePath);

    // scriptFile
    _scriptFile = replaceProjectDirToMacro(_scriptFile);

    // package.path
    vector<string> arr = getPackagePathArray();
    _packagePath = string("");
    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        string path = replaceProjectDirToMacro(*it);
        _packagePath.append(path);
        _packagePath.append(";");
    }
    if (_packagePath.length() > 0 && _packagePath[_packagePath.length() - 1] == ';')
    {
        _packagePath = _packagePath.substr(0, _packagePath.length() - 1);
    }
}

string ProjectConfig::replaceProjectDirToMacro(const string &path) const
{
    if (!isAbsolutePath(path))
    {
        if (path.compare(0, 10, "$(PROJDIR)") == 0) return path;
        string result("$(PROJDIR)");
        result.append(DIRECTORY_SEPARATOR);
        result.append(path);
        return result;
    }

    string result = path;
    size_t len = _projectDir.length();
    if (len > 0 && result.compare(0, len, _projectDir) == 0)
    {
        result = "$(PROJDIR)";
        result.append(DIRECTORY_SEPARATOR);
        result.append(path.substr(len));
    }
    return result;
}

string ProjectConfig::replaceProjectDirToFullPath(const string &path) const
{
    if (isAbsolutePath(path)) return path;

    if (path.length() == 0) return _projectDir;

    string result = path;
    std::cout << "replaceProjectDirToFullPath path = " << path << std::endl;
    std::cout << "replaceProjectDirToFullPath _projectDir = " << _projectDir << std::endl;
    if (path.compare(0, 10, "$(PROJDIR)") == 0)
    {
        result = _projectDir;
        std::cout << "_projectDir path = " << _projectDir << std::endl;
        string suffix = path.substr(10);
        std::cout << "suffix path = " << suffix.c_str() << std::endl;
        if (suffix[0] == DIRECTORY_SEPARATOR_CHAR)
        {
            suffix = suffix.substr(1);
        }
        result.append(suffix);
    }
    return result;
}

bool ProjectConfig::isAbsolutePath(const string &path) const
{
    if (DIRECTORY_SEPARATOR_CHAR == '/')
    {
        return path.length() > 0 && path[0] == '/';
    }
    return path.length() > 2 && path[1] == ':';
}

void ProjectConfig::setQuickCocos2dxRootPath(const string &path)
{
    if (path.length())
    {
        _quickCocos2dxRootPath = path;
        std::cout << "setQuickCocos2dxRootPath path = " << _quickCocos2dxRootPath.c_str() << std::endl;
        makeNormalizePath(&_quickCocos2dxRootPath);
        if (_quickCocos2dxRootPath[_quickCocos2dxRootPath.length() - 1] != DIRECTORY_SEPARATOR_CHAR)
        {
            _quickCocos2dxRootPath.append(DIRECTORY_SEPARATOR);
        }
    }
}

string ProjectConfig::getQuickCocos2dxRootPath() const
{
    
    std::cout << "getQuickCocos2dxRootPath path = " << _quickCocos2dxRootPath.c_str() << std::endl;
    return _quickCocos2dxRootPath;
}

// helper
void ProjectConfig::makeNormalizePath(string *path, const char *directorySeparator/* = NULL*/)
{
    if (!directorySeparator) directorySeparator = DIRECTORY_SEPARATOR;
    size_t pos = std::string::npos;
    std::cout << "makeNormalizePath path = " << path->c_str() << std::endl;
    std::cout << "makeNormalizePath directorySeparator = " << directorySeparator << std::endl;
    while ((pos = path->find_first_of("/\\", pos + 1)) != std::string::npos)
    {
        path->replace(pos, 1, directorySeparator);
    }
    
    std::cout << "makeNormalizePath path = " << path->c_str() << std::endl;
}
