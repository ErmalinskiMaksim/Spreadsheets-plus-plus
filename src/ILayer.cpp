#include "ILayer.h"

OptRequest ILayer::readRequest() {
    auto req = std::move(m_pendingRequest);
    m_pendingRequest.reset();
    return req;
}

