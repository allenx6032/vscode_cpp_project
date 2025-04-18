#ifndef BACKENDS_WASAPI_H
#define BACKENDS_WASAPI_H

#include "al_alc_backends_base.h"

struct WasapiBackendFactory final : public BackendFactory {
public:
    bool init() override;

    bool querySupport(BackendType type) override;

    alc::EventSupport queryEventSupport(alc::EventType eventType, BackendType type) override;

    std::string probe(BackendType type) override;

    BackendPtr createBackend(DeviceBase *device, BackendType type) override;

    static BackendFactory &getFactory();
};

#endif /* BACKENDS_WASAPI_H */
