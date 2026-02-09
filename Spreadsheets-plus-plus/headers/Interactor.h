#ifndef INTERACTOR_H
#define INTERACTOR_H

#include "Events.h"

class Interactor {
protected:
    template<LayerEventType Event>
    inline void processEvents(const Event&) {}
};

#endif
