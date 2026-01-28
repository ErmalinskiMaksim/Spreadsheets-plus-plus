#include "Layer.h"

OptRequest Layer::readRequest() {
    auto req = std::move(m_pendingRequest);
    m_pendingRequest.reset();
    return req;
}

