#pragma once

#include <string>
#include "cocos2d-CCData.h"

class Crypt {
public:
    virtual ~Crypt() {}
    virtual bool isEncrypted(const cocos2d::Data& buffer) const = 0;
    virtual void decrypt(const cocos2d::Data& in, cocos2d::Data* out) const = 0;
    virtual void encrypt(const cocos2d::Data& in, cocos2d::Data* out) const = 0;
};


class XXTeaCrypt : public Crypt {
public:
    XXTeaCrypt(const std::string& key, const std::string& sign);
    virtual bool isEncrypted(const cocos2d::Data& buffer) const override;
    virtual void decrypt(const cocos2d::Data& in, cocos2d::Data* out) const override;
    virtual void encrypt(const cocos2d::Data& in, cocos2d::Data* out) const override;
    
private:
    std::string _key;
    std::string _sign;
};
