#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
// #include <stdlib.h>
// #include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 70000 // 70 KB chunk

bool get_img_timestamp(const char *data, char *output_buffer,
                       size_t output_buffer_size);
