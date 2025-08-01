/****************************************************************************
Copyright (c) 2011      Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __CCGLPROGRAMCACHE_H__
#define __CCGLPROGRAMCACHE_H__

#include <string>
#include <unordered_map>

#include "cocos_CCRef.h"

NS_CC_BEGIN

class GLProgram;

/**
 * @addtogroup shaders
 * @{
 */

/** GLProgramCache
 Singleton that stores manages GLProgram objects (shaders)
 @since v2.0
 */
class CC_DLL GLProgramCache : public Ref
{
public:
    /**
     * @js ctor
     */
    GLProgramCache();
    /**
     * @js NA
     * @lua NA
     */
    ~GLProgramCache();

    /** returns the shared instance */
    static GLProgramCache* getInstance();

    /** purges the cache. It releases the retained instance. */
    static void destroyInstance();
    
    /** special for android reload GL */
    void reloadAllGLPrograms();

    /** returns a GL program for a given key 
     */
    GLProgram * getGLProgram(const std::string &key);

    /** adds a GLProgram to the cache for a given name */
    void addGLProgram(GLProgram* program, const std::string &key);

private:
    bool init();
    void loadDefaultGLProgram(GLProgram *program, int type);
    void loadDefaultGLPrograms();//for the first time
    void reloadDefaultGLPrograms();// for the second time
    
    std::string getShaderMacrosForLight() const;

//    Dictionary* _programs;
    std::unordered_map<std::string, GLProgram*> _programs;
};

// end of shaders group
/// @}

NS_CC_END

#endif /* __CCGLPROGRAMCACHE_H__ */
