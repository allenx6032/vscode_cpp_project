/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.
 
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

#ifndef __CC_BUNDLE_READER_H__
#define __CC_BUNDLE_READER_H__

#include <string>
#include <vector>

#include "cocos_CCRef.h"
#include "cocos_CCPlatformMacros.h"
#include "cocos_CCConsole.h"

NS_CC_BEGIN

/**
 * BundleReader is an interface for reading sequence of bytes.
 */
class BundleReader: public cocos2d::Ref
{
public:
    /**
     * Structor
     */
    BundleReader();
    
    /**
     * inicial
     */
    ~BundleReader();
    
    /**
     * initialise
     * @param lpbuffer The data buffer pointer
     * @param length The data buffer size
     */
    void init(char* buffer, ssize_t length);

    /**
     * Reads an array of elements.
     *
     * @param ptr   The pointer to the memory to copy into.
     *              The available size should be at least bytes.
     * @param size  The size of each element to be read, in bytes.
     * @param count The number of elements to read.
     *
     * @return The number of elements read.
     */
    ssize_t read(void* ptr, ssize_t size, ssize_t count);

    /**
     * Reads a line from the buffer.
     */
    char* readLine(int num, char* line);

    /**
     * Returns true if the end of the buffer has been reached.
     */
    bool eof();

    /**
     * Returns the length of the buffer in bytes.
     */
    ssize_t length();

    /**
     * Returns the position of the file pointer.
     */
    ssize_t tell();

    /**
     * Sets the position of the file pointer.
     */
    bool seek(long int offset, int origin);

    /**
     * Sets the file pointer at the start of the file.
     */
    bool rewind();

    /**
     * read binary typed value.
     */
    template<typename T> bool read(T* ptr);
    template<typename T> bool readArray(unsigned int* length, std::vector<T>* values);

    /**
     * first read length, then read string text
     */
    std::string readString();
    bool readMatrix(float* m);

private:
    ssize_t _position;
    ssize_t  _length;
    char* _buffer;
};

/**
* template read routines
*/
template<typename T>
inline bool BundleReader::read(T *ptr)
{
    return (read(ptr, sizeof(T), 1) == 1);
}

/**
* template function to read array of value.
*/
template<typename T>
inline bool BundleReader::readArray(unsigned int *length, std::vector<T> *values)
{
    if (!read(length))
    {
        return false;
    }
    
    if (*length > 0 && values)
    {
        values->resize(*length);
        if (read(&(*values)[0], sizeof(T), *length) != *length)
        {
            return false;
        }
    }
    return true;
}

/**
* specalization for char
*/
template<>
inline bool BundleReader::read<char>(char *ptr)
{
    if (read(ptr, sizeof(char), 1) == 1)
    {
        return true;
    }
    else
    {
        *ptr = -1;
        return false;
    }
}

/**
* specalization for std::string
*/
template<>
inline bool BundleReader::read<std::string>(std::string *ptr)
{
    CCLOG("can not read std::string, use readString() instead");
    return false;
}

/**
* template function to read array of value.
*/
template<>
inline bool BundleReader::readArray<std::string>(unsigned int *length, std::vector<std::string> *values)
{
    if (!read(length))
    {
        return false;
    }
    values->clear();
    if (*length > 0 && values)
    {
        for (int i = 0; i < (int)*length; ++i)
        {
            values->push_back(readString());
        }
    }
    return true;
}


NS_CC_END

#endif
