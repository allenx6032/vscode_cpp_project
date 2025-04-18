#include "bl_app_jsonbuilder.h"

namespace blbench {

JSONBuilder::JSONBuilder(BLString* dst)
  : _dst(dst),
    _last(kTokenNone),
    _level(0) {}

JSONBuilder& JSONBuilder::openArray() {
  if (_last == kTokenValue)
    _dst->append(',');

  _dst->append('[');
  _last = kTokenNone;
  _level++;

  return *this;
}

JSONBuilder& JSONBuilder::closeArray(bool nl) {
  _level--;
  if (nl) {
    _dst->append('\n');
    _dst->append(' ', _level * 2);
  }

  _dst->append(']');
  _last = kTokenValue;

  return *this;
}

JSONBuilder& JSONBuilder::openObject() {
  if (_last == kTokenValue)
    _dst->append(',');

  _dst->append('{');
  _last = kTokenNone;
  _level++;

  return *this;
}

JSONBuilder& JSONBuilder::closeObject(bool nl) {
  _level--;
  if (nl) {
    _dst->append('\n');
    _dst->append(' ', _level * 2);
  }

  _dst->append('}');
  _last = kTokenValue;
  return *this;
}

JSONBuilder& JSONBuilder::comma() {
  if (_last == kTokenValue)
    _dst->append(',');
  _last = kTokenNone;

  return *this;
}

JSONBuilder& JSONBuilder::addKey(const char* str) {
  addString(str);

  _dst->append(':');
  _last = kTokenNone;

  return *this;
}

JSONBuilder& JSONBuilder::addBool(bool b) {
  if (_last == kTokenValue)
    _dst->append(',');

  _dst->append(b ? "true" : "false");
  _last = kTokenValue;

  return *this;
}

JSONBuilder& JSONBuilder::addInt(int64_t n) {
  if (_last == kTokenValue)
    _dst->append(',');

  _dst->appendFormat("%lld", (long long)n);
  _last = kTokenValue;

  return *this;
}

JSONBuilder& JSONBuilder::addUInt(uint64_t n) {
  if (_last == kTokenValue)
    _dst->append(',');

  _dst->appendFormat("%llu", (unsigned long long)n);
  _last = kTokenValue;

  return *this;
}

JSONBuilder& JSONBuilder::addDouble(double d) {
  if (_last == kTokenValue)
    _dst->append(',');

  _dst->appendFormat("%g", d);
  _last = kTokenValue;

  return *this;
}

JSONBuilder& JSONBuilder::addDoublef(const char* fmt, double d) {
  if (_last == kTokenValue)
    _dst->append(',');

  _dst->appendFormat(fmt, d);
  _last = kTokenValue;

  return *this;
}

JSONBuilder& JSONBuilder::addString(const char* str) {
  if (_last == kTokenValue)
    _dst->append(',');

  _dst->appendFormat("\"%s\"", str);
  _last = kTokenValue;

  return *this;
}

JSONBuilder& JSONBuilder::addStringf(const char* fmt, ...) {
  if (_last == kTokenValue)
    _dst->append(',');

  va_list ap;
  va_start(ap, fmt);

  _dst->append('\"');
  _dst->appendFormatV(fmt, ap);
  _dst->append('\"');
  _last = kTokenValue;

  va_end(ap);

  return *this;
}

JSONBuilder& JSONBuilder::addStringWithoutQuotes(const char* str) {
  if (_last == kTokenValue)
    _dst->append(',');

  _dst->append(str);
  _last = kTokenValue;

  return *this;
}

JSONBuilder& JSONBuilder::alignTo(size_t n) {
  size_t i = _dst->size();
  const char* p = _dst->data();

  while (i)
    if (p[--i] == '\n')
      break;

  size_t cur = _dst->size() - i;
  if (cur < n)
    _dst->append(' ', n - cur);

  return *this;
}

JSONBuilder& JSONBuilder::beforeRecord() {
  if (_last == kTokenValue)
    _dst->append(',');

  _dst->append('\n');
  _dst->append(' ', _level * 2);
  _last = kTokenNone;

  return *this;
}

} // {blbench} namespace
