#ifndef BACKENDS_WAVE_H
#define BACKENDS_WAVE_H

#include "al_alc_backends_base.h"

struct WaveBackendFactory final : public BackendFactory {
public:
    bool init() override;

    bool querySupport(BackendType type) override;

    std::string probe(BackendType type) override;

    BackendPtr createBackend(DeviceBase *device, BackendType type) override;

    static BackendFactory &getFactory();
};

#endif /* BACKENDS_WAVE_H */
