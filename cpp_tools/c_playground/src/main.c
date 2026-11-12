#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
// #include <stdlib.h>
// #include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 70000 // 70 KB chunk

static inline size_t min(size_t a, size_t b) { return (a < b) ? a : b; }

static unsigned read2(const uint8_t *b, size_t pos, bool big_endian)
{
    if (big_endian)
        return b[pos] * 256 + b[pos + 1];
    return b[pos + 1] * 256 + b[pos];
}

static unsigned long read4(const uint8_t *b, size_t pos, bool big_endian)
{
    if (big_endian)
        return b[pos] * (256UL * 256 * 256) + b[pos + 1] * (256UL * 256) +
               b[pos + 2] * 256UL + b[pos + 3];
    return b[pos + 3] * (256UL * 256 * 256) + b[pos + 2] * (256UL * 256) +
           b[pos + 1] * 256UL + b[pos];
}

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

    uint8_t buffer[BUFFER_SIZE] = {0};
    size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, f);
    fclose(f); // Close immediately after reading into memory

    // step 1 make sure file is jpeg
    if (bytes_read < 4 || buffer[0] != 0xFF || buffer[1] != 0xD8)
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
    bool big_endian;
    if (buffer[base] == 'M' && buffer[base + 1] == 'M')
        big_endian = true;
    else if (buffer[base] == 'I' && buffer[base + 1] == 'I')
        big_endian = false;
    else
        return 0;

    if (read2(buffer, base + 2, big_endian) != 42)
    {
        fprintf(stderr,
                "Should be 42 after our big endian (MSB) so we got the base "
                "wrong\n");
        return 0;
    }

    unsigned long ifd0_offset = read4(buffer, base + 4, big_endian);
    printf("ifd0 offset: %d\n", ifd0_offset);

    // int cnt = 0;
    // for (size_t i = 0; i < 72; i += 1, cnt += 1)
    // {
    //     printf("%02X ", buffer[i]);
    //     if ((i + 1) % 16 == 0)
    //     {
    //         printf("\n");
    //     }
    // }
    // printf("\n");
    // p/x buffer[0]@12

    size_t numEntries =
        read2(buffer, base + ifd0_offset,
              big_endian); // pos 38-39  00 0B 11 entries in the list
    int found = 0;

    printf("%d\n", numEntries);

    int base_offset = base + ifd0_offset + 2;
    size_t exif_ifd = 0;

    // Every entry is exactly 12 bytes, always in the same layout:
    for (size_t entry_offset = 0; entry_offset < numEntries; entry_offset += 1)
    {

        size_t row = base_offset + entry_offset * 12;
        if (read2(buffer, row, big_endian) == 0x8769)
        {
            unsigned long exif_offset = read4(buffer, row + 8, big_endian);
            exif_ifd = base + exif_offset;
            printf("Exif IFD at %zu\n", exif_ifd);
            found = 1;
            for (size_t b = 0; b < 12; b++)
            {
                printf("%02X ", buffer[row + b]);
            }
            printf("\n");
            printf("\n");
            break;
        }
    }

    if (!found)
    {
        fprintf(stderr, "Couldnt find 87 69 bytes\n");
        return 0;
    }

    size_t exif_entries = read2(buffer, exif_ifd, big_endian);
    printf("Exif IFD has %zu entries\n", exif_entries);

    /*
     * Every IFD has the same layout (TIFF format, same as IFD0):
     *   exif_ifd + 0          2 bytes   number of rows (entries)
     *   exif_ifd + 2          12 bytes  row 0
     *   exif_ifd + 14         12 bytes  row 1
     *   ...                             row i starts at exif_ifd + 2 + i * 12
     *
     * Skip the 2-byte count to reach the first row.
     */
    int newBaseOffset = exif_ifd + 2; /* position of row 0 in the Exif IFD */

    int dateFindingOffset = base;

    for (size_t entry_offset = 0; entry_offset < exif_entries;
         entry_offset += 1)
    {
        size_t row = newBaseOffset + (entry_offset * 12);
        if (read2(buffer, row, big_endian) == 0x9003)
        {
            dateFindingOffset = base + read4(buffer, row + 8, big_endian);
            for (int b = 8; b < 12; b += 1)
            {
                printf("%02X ", buffer[row + b]);
            }
            printf("\n");
            break;
        }
    }

    // final step: read the date
    if (BUFFER_SIZE < 20)
    {
        fprintf(stderr,
                "buffer size needs to be at least 20 i.e. date is 19 "
                "characters and we add 1 to account for null terminator\n");
        return 0;
    }

    if (dateFindingOffset + 20 > bytes_read)
    {
        return 0;
    }

    // for (size_t i = 0; i < 19; i++)
    // {
    //     printf("%c", buffer[dateFindingOffset + i]);
    // }

    memcpy(output_buffer, &buffer[dateFindingOffset], 19);
    output_buffer[19] = '\0';

    return 1;
}

static bool file_exists(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0;
}

static bool is_directory(const char *path)
{
    struct stat st;
    if (stat(path, &st) != 0)
        return false; /* doesn't exist, or can't be read */
    return S_ISDIR(st.st_mode);
}

int main(int argc, char **argv)
{
    /*
     /home/leo_zhang/projects/Tools/cpp_tools/c_playground/imgs/20261007_104719.jpg
     /home/leo_zhang/projects/Tools/cpp_tools/c_playground/imgs/IMG_3234.jpg

     */
    char date[20];
    if (argc < 2)
    {
        perror("please add a path argument");
        return 1;
    }
    if (!file_exists(argv[1]))
    {
        perror("directory not found");
        return 1;
    }
    bool found = get_img_timestamp(argv[1], date, sizeof date);
    printf("%s\n", found ? "date found" : "date not found");
    if (found)
        printf("date: %s\n", date);

    return 0;
}
