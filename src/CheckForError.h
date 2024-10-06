//
// Created by FabianKla on 04/10/2024.
//

#ifndef CHECKFORERROR_H
#define CHECKFORERROR_H

#include <cstdio>
#include "Canlib/INC/canlib.h"
#include "Canlib/INC/canstat.h"

void CheckForError(const char *cmd, canStatus stat) {
    if (stat != canOK) {
        char buf[255];
        canGetErrorText(stat, buf, sizeof(buf));
        printf("[%s] %s: failed, stat=%d\n", cmd, buf, (int) stat);
        abort();
    }
}

#endif //CHECKFORERROR_H
