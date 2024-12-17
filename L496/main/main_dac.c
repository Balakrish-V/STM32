#include "lib_headers.h"
int main(void) {
    DAC_Init();                     // Initialize DAC
    while (1) {
        for (uint32_t value = 0; value < 4096; value++) {
            DAC_SetValue(value);    // Output increasing values
            for (volatile int i = 0; i < 1000; i++); 
        }
        for (uint32_t value = 4095; value > 0; value--) {
            DAC_SetValue(value);    // Output decreasing values
            for (volatile int i = 0; i < 1000; i++); 
        }
    }
}