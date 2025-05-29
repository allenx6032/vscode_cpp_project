#ifndef BOOMEFFECTENVELOPESINE_H
#define BOOMEFFECTENVELOPESINE_H

#include "deceptus_boomeffectenvelope.h"

class BoomEffectEnvelopeSine : public BoomEffectEnvelope
{
public:
   BoomEffectEnvelopeSine() = default;
   float shakeFunction(float t) override;
};

#endif  // BOOMEFFECTENVELOPESINE_H
