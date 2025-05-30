/****************************************************************************
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

#include "cocos2d-ccRef.h"
#include "cocos2d-CCBool.h"
#include "cocos2d-CCInteger.h"
#include "cocos2d-CCFloat.h"
#include "cocos2d-CCDouble.h"
#include "cocos2d-CCString.h"
#include "cocos2d-CCArray.h"
#include "cocos2d-CCDictionary.h"
#include "cocos2d-CCSet.h"

NS_CC_BEGIN

void DataVisitor::visit(const __Bool *value)
{
    visitObject(value);
}

void DataVisitor::visit(const __Integer *value)
{
    visitObject(value);
}

void DataVisitor::visit(const __Float *value)
{
    visitObject(value);
}

void DataVisitor::visit(const __Double *value)
{
    visitObject(value);
}

void DataVisitor::visit(const __String *value)
{
    visitObject(value);
}

void DataVisitor::visit(const __Array *value)
{
    visitObject(value);
}

void DataVisitor::visit(const __Dictionary *value)
{
    visitObject(value);
}

void DataVisitor::visit(const __Set *value)
{
    visitObject(value);
}

// PrettyPrinter
PrettyPrinter::PrettyPrinter(int indentLevel/* = 0 */)
{
    setIndentLevel(indentLevel);
}

void PrettyPrinter::clear()
{
    _result.clear();
}

std::string PrettyPrinter::getResult()
{
    return _result;
}

void PrettyPrinter::visitObject(const Ref *p)
{
    char buf[50] = {0};
    snprintf(buf, sizeof(buf), "%p", p);
    _result += buf;
}

void PrettyPrinter::visit(const __Bool * p)
{
    char buf[50] = {0};
    snprintf(buf, sizeof(buf), "%s", p->getValue() ? "true" : "false");
   _result += buf;
}

void PrettyPrinter::visit(const __Integer *p)
{
    char buf[50] = {0};
    snprintf(buf, sizeof(buf), "%d", p->getValue());
    _result += buf;
}

void PrettyPrinter::visit(const __Float *p)
{
    char buf[50] = {0};
    snprintf(buf, sizeof(buf), "%f", p->getValue());
    _result += buf;
}

void PrettyPrinter::visit(const __Double *p)
{
    char buf[50] = {0};
    snprintf(buf, sizeof(buf), "%lf", p->getValue());
    _result += buf;
}

void PrettyPrinter::visit(const __String *p)
{
    _result += p->getCString();
}

void PrettyPrinter::visit(const __Array *p)
{
    _result += "\n";
    _result += _indentStr;
    _result += "<array>\n";

    setIndentLevel(_indentLevel+1);
    Ref* obj;
    int i = 0;
    char buf[50] = {0};
    CCARRAY_FOREACH(p, obj)
    {
        if (i > 0) {
            _result += "\n";
        }
        snprintf(buf, sizeof(buf), "%s%02d: ", _indentStr.c_str(), i);
        _result += buf;
        PrettyPrinter v(_indentLevel);
//FIXME:james        obj->acceptVisitor(v);
        _result += v.getResult();
        i++;
    }
    setIndentLevel(_indentLevel-1);
    
    _result += "\n";
    _result += _indentStr;
    _result += "</array>";
}

void PrettyPrinter::visit(const __Dictionary *p)
{
    _result += "\n";
    _result += _indentStr;
    _result += "<dict>\n";
    
    setIndentLevel(_indentLevel+1);
    DictElement* element;
    bool bFirstElement = true;
    char buf[1000] = {0};
    CCDICT_FOREACH(p, element)
    {
        if (!bFirstElement) {
            _result += "\n";
        }
        snprintf(buf, sizeof(buf), "%s%s: ", _indentStr.c_str(),element->getStrKey());
        _result += buf;
        PrettyPrinter v(_indentLevel);
//FIXME:james        element->getObject()->acceptVisitor(v);
        _result += v.getResult();
        bFirstElement = false;
    }
    setIndentLevel(_indentLevel-1);
    
    _result += "\n";
    _result += _indentStr;
    _result += "</dict>";
}

void PrettyPrinter::visit(const __Set *p)
{
    _result += "\n";
    _result += _indentStr;
    _result += "<set>\n";
    
    setIndentLevel(_indentLevel+1);

    int i = 0;
    __Set* tmp = const_cast<__Set*>(p);
    __SetIterator it = tmp->begin();

    for (; it != tmp->end(); ++it, ++i) {
        if (i > 0) {
            _result += "\n";
        }
        _result += _indentStr.c_str();
        PrettyPrinter v(_indentLevel);
//FIXME:james        (*it)->acceptVisitor(v);
        _result += v.getResult();
    }
    setIndentLevel(_indentLevel-1);
    
    _result += "\n";
    _result += _indentStr;
    _result += "</set>\n";
}

void PrettyPrinter::setIndentLevel(int indentLevel)
{
    _indentLevel = indentLevel;
    _indentStr.clear();
    for (int i = 0; i < _indentLevel; ++i) {
        _indentStr += "\t";
    }
}

NS_CC_END
