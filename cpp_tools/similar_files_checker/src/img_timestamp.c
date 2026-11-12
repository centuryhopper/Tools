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

static inline size_t min(size_t a, size_t b) { return (a < b) ? a : b; }

static inline bool match_hex(const char jpeg_check[], const char *hex_string,
                             size_t size)
{
    return memcmp(jpeg_check, hex_string, size) == 0;
}

//  grep -obUaP '\d{4}:\d{2}:\d{2} \d{2}:\d{2}:\d{2}'
//  /home/leo_zhang/projects/Tools/cpp_tools/similar_files_checker/tests/images/unique_imgs/IMG_3234.jpg
bool get_img_timestamp(const char *data, char *output_buffer,
                       size_t buffer_size)
{

    if (!output_buffer || buffer_size == 0)
    {
        return 0;
    }

    FILE *f = fopen(data, "rb");
    if (!f)
    {
        perror("Couldnt open file or file doesn't exist");
        return 0;
    }

    unsigned char date[20];

    // if (fseek(f, 0, SEEK_SET) != 0) {
    //   /* seek failed */
    // }
    // if (fread(date, 1, sizeof date, f) != sizeof date) {
    //   /* file shorter than expected */
    // }

    // char tmp[2];
    // size_t bytes_read = fread(tmp, 1, sizeof tmp, f);
    //
    // if (bytes_read > 0) {
    //   printf("Byte 1: 0x%02X (%d)\n", tmp[0], tmp[0]);
    // }
    // if (bytes_read > 1) {
    //   printf("Byte 2: 0x%02X (%d)\n", tmp[1], tmp[1]);
    // }
    //
    // for (int i = 0; i < 20; i++) {
    //   printf("%c ", date[i]);
    // }
    // printf("\n");

    /*
     Check it’s a JPEG: the first bytes are FF D8.

  Find the EXIF block: hop segment to segment (each says its length) until an FF
  E1 segment starting with Exif\0\0. The byte after that is base.

  Read the header at base: byte order (MM/II), the number 42, then where IFD0
  is.

  In IFD0, find tag 0x8769: its value says where the Exif IFD is. In the Exif
  IFD, find tag 0x9003: its value says where the date is. Read the date there:
  19 characters, YYYY:MM:DD HH:MM:SS. Check it looks valid.

  Every position is base + a number from the file. If any step fails or points
  outside the block, return “no date”.
     */

    uint8_t buffer[BUFFER_SIZE] = {0};
    size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, f);

    // step 1 make sure file is jpeg
    if (buffer[0] != 0xFF || buffer[1] != 0xD8)
    {
        fprintf(stderr, "File is not jpeg\n");
        return 0;
    }

    // step 2 find the base
    int pos = 2;
    if (buffer[pos] == 0xFF && buffer[pos + 1] == 0xE0)
    {
        // segment E0
        // first byte counts how many 256s there are and the second counts
        // what’s left over
        /*

          box:   2  3 | 4  5 | 6  7  8  9 10 11 12 13 14 15 16 17 18 19 | 20
          FF E0 | 00 10| ............ JFIF data ............... | FF E1 ...
          marker| length = 16 (these 2 + the 14 data boxes)       | next segment

         */
        int len = buffer[pos + 2] * 256 + buffer[pos + 3]; /* its length */
        pos = pos + 2 + len;                               /* jump over it */
    }

    int base;
    if (buffer[pos] == 0xFF && buffer[pos + 1] == 0xE1 &&
        memcmp(&buffer[pos + 4], "Exif\0\0", 6) == 0)
    {
        base = pos + 10; /* after marker, length, label */
        printf("base = %d\n", base);
    }
    else
    {
        printf("no EXIF found\n");
        return 0;
    }

    // step 3 read header at base
    printf("big-endian base: %c %c\n", buffer[base], buffer[base + 1]);
    if (buffer[base] != 'M' || buffer[base + 1] != 'M')
    {
        fprintf(stderr, "not big-endian (MM), not supported yet\n");
        return 0;
    }

    if (buffer[base + 2] * 256 + buffer[base + 3] != 42)
    {
        fprintf(stderr,
                "Should be 42 after our big endian (MSB) so we got the base "
                "wrong\n");
        return 0;
    }

    unsigned long ifd0_offset = buffer[base + 4] * (256UL * 256 * 256) +
                                buffer[base + 5] * (256UL * 256) +
                                buffer[base + 6] * 256UL + buffer[base + 7];
    printf("ifd0 offset: %d\n", ifd0_offset);

    int cnt = 0;
    for (size_t i = 0; i < 72; i += 1, cnt += 1)
    {
        printf("%02X ", buffer[i]);
        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");

    size_t numEntries = buffer[ifd0_offset + 1] * 256 + buffer[ifd0_offset + 2];

    // Every entry is exactly 12 bytes, always in the same layout:
    int entry_offset = 0;
    for (; entry_offset < numEntries; entry_offset++)
    {
        for (size_t e = 0; e < 12; e++)
        {
        }
    }

    fclose(f);

    return 1;
}
