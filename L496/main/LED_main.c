#include "lib_headers.h"
int main(void) {
    init_clock();
    Initialize_LED_GPIOs();
    Green_LED_On();
    while (1) {
        // Main loop
    }
    return 0;
}
