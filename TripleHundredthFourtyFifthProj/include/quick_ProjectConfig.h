
#ifndef __PROJECT_CONFIG_H_
#define __PROJECT_CONFIG_H_

#include <string>
#include <vector>

using namespace std;

#include "cocos_cocos2d.h"
#include "lua_cocos2dx_CCLuaStack.h"

#define kCCLuaDebuggerNone      0

#define kProjectConfigQuickRootPath             1       // -quick "PATH"
#define kProjectConfigProjectDir                2       // -workdir "PATH"
#define kProjectConfigScriptFile                4       // -script "FILENAME"
#define kProjectConfigPackagePath               8       // -package.path "PATH;PATH"
#define kProjectConfigWritablePath              16      // -writable "PATH"
#define kProjectConfigFrameSize                 32      // -size 960x640
#define kProjectConfigFrameScale                64      // -scale 1.0
#define kProjectConfigShowConsole               128     // -console, -disable-console
#define kProjectConfigWriteDebugLogToFile       512     // -write-debug-log, -disable-write-debug-log
#define kProjectConfigWindowOffset              1024    // -offset {0,0}
#define kProjectConfigDebugger                  2048    // -debugger-ldt, -debugger-codeide, -disable-debugger

#define kProjectConfigOpenRecent (kProjectConfigProjectDir | kProjectConfigScriptFile | kProjectConfigPackagePath | kProjectConfigWritablePath | kProjectConfigFrameSize | kProjectConfigFrameScale | kProjectConfigShowConsole | kProjectConfigLoadPrecompiledFramework | kProjectConfigWriteDebugLogToFile)

#define kProjectConfigAll (kProjectConfigQuickRootPath | kProjectConfigProjectDir | kProjectConfigScriptFile | kProjectConfigPackagePath | kProjectConfigWritablePath | kProjectConfigFrameSize | kProjectConfigFrameScale | kProjectConfigShowConsole | kProjectConfigWriteDebugLogToFile | kProjectConfigWindowOffset | kProjectConfigDebugger)


class ProjectConfig
{
public:
    ProjectConfig();

    static const int DEFAULT_WIDTH = 640;
    static const int DEFAULT_HEIGHT = 960;

    bool isWelcome() const;
    void resetToWelcome();
    void resetToCreator();

    string getProjectDir() const;
    void setProjectDir(const string &projectDir);

    string getScriptFile() const;
    string getScriptFileRealPath() const;
    void setScriptFile(const string &scriptFile);

    string getWritablePath() const;
    string getWritableRealPath() const;
    void setWritablePath(const string &writablePath);

    string getPackagePath() const;
    string getNormalizedPackagePath() const;
    void setPackagePath(const string &packagePath);
    void addPackagePath(const string &packagePath);
    vector<string> getPackagePathArray() const;

    cocos2d::Size getFrameSize() const;
    void setFrameSize(const cocos2d::Size &frameSize);
    bool isLandscapeFrame() const;
    bool isPortraitFrame() const;
    void changeFrameOrientation();
    void changeFrameOrientationToPortait();
    void changeFrameOrientationToLandscape();

    float getFrameScale() const;
    void setFrameScale(float frameScale);

    bool isShowConsole() const;
    void setShowConsole(bool showConsole);

    bool isWriteDebugLogToFile() const;
    void setWriteDebugLogToFile(bool writeDebugLogToFile);
    string getDebugLogFilePath() const;

    cocos2d::Vec2 getWindowOffset() const;
    void setWindowOffset(const cocos2d::Vec2 &windowOffset);

    int getDebuggerType() const;
    void setDebuggerType(int debuggerType);

    //void parseCommandLine(const vector<string> &args);
    string makeCommandLine(unsigned int mask = kProjectConfigAll) const;

    bool isAppMenu() const;
    bool isResizeWindow() const;
    bool isRetinaDisplay() const;

    bool validate() const;
    void dump();
    
    // set Quick-Cocos2dx-Community root path
    void setQuickCocos2dxRootPath(const string &path);
    string getQuickCocos2dxRootPath() const;
    
    // helper
    static void makeNormalizePath(string *path, const char *directorySeparator = NULL);

private:
    bool _isWelcome;
    string _projectDir;
    string _scriptFile;
    string _packagePath;
    string _writablePath;
    string _quickCocos2dxRootPath;
    cocos2d::Size _frameSize;
    float _frameScale;
    bool _showConsole;
    bool _writeDebugLogToFile;
	bool _isMultiLogFiles;
    bool _restartProcess;
    cocos2d::Vec2 _windowOffset;
    int _debuggerType;
    bool _isAppMenu;
    bool _isResizeWindow;
    bool _isRetinaDisplay;

    void normalize();
    string replaceProjectDirToMacro(const string &path) const;
    string replaceProjectDirToFullPath(const string &path) const;
    bool isAbsolutePath(const string &path) const;
};

#endif // __PROJECT_CONFIG_H_
