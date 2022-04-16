//
// Created by KrisnaPranav on 19/01/22.
//

#pragma once

#include "manager.h"
#include <internal/pci.h>

namespace Kernel {
    struct pciDriverEntry {
        uint16_t vendorID;
        uint16_t deviceID;
        char* driverString;
    } __attribute__((packed));

    class pciDriver {
      public:
        static assignPCIDriver(pciController* pci, driverManager* driverManager);
    };
}