#include "common.h"

uint32_t timeDiff(uint32_t future, uint32_t past) {
    // Compare two timestamps considering possible time overflow.
    // It works for time intervals less than ~24 days
    if (past <= UINT32_MAX/2 || future > past) {
        return future - past;
    } else {
        // time overflow
        return (UINT32_MAX - past) + future;
    }
}
