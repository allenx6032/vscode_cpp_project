#include "demo_Crypt.hpp"

#include "xxtea.h"

XXTeaCrypt::XXTeaCrypt(const std::string& key, const std::string& sign)
: _key(key)
, _sign(sign)
{
    _key.resize(16); // xxtea will resize key to 16, it key is not size of 16.
}

bool XXTeaCrypt::isEncrypted(const cocos2d::Data& buffer) const
{
    return (buffer.getSize() >= _sign.size())
            && (memcmp(_sign.data(), buffer.getBytes(), _sign.size()) == 0);
}

void XXTeaCrypt::decrypt(const cocos2d::Data& in, cocos2d::Data* out) const
{
    xxtea_long len;
    auto buf = xxtea_decrypt(in.getBytes() + _sign.size(), (xxtea_long)(in.getSize()-_sign.size()),
                             (unsigned char*)(&_key[0]), (xxtea_long) _key.size(), &len);
    out->fastSet(buf, len);
}

void XXTeaCrypt::encrypt(const cocos2d::Data& in, cocos2d::Data* out) const
{
    xxtea_long len;
    auto buf = xxtea_encrypt(in.getBytes(), (xxtea_long)in.getSize(),
                             (unsigned char*)(&_key[0]), (xxtea_long) _key.size(), &len);
    out->fastSet(buf, len);
}
