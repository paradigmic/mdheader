#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <endian.h>
#include <iconv.h>
#include <stdlib.h>

#define HEADER_SIZE 0x100
#define HEADER_OFFSET 0x100

struct mdheader {
    char title_com[16];
    char copyright[8];
    char date[8];
    char title_dom[48];
    char title_over[48];
    char type[2];
    char product[14];
    char controls[16];
    uint32_t rom_start;
    uint32_t rom_end;
    uint32_t ram_start;
    uint32_t ram_end;
    char external[12];
    char modem[12];
    char memo[40];
    char regions[16];
} __attribute ((packed));

char *iosupport[] =
{
    "Master System Joystick",
    "Joystick",
    "Keyboard",
    "RS232C Serial",
    "Printer",
    "Tablet",
    "Control Ball",
    "Paddle Controller",
    "FDD",
    "CD-ROM",
    "Unknown",
};

char *regions[] =
{
    "Japan",
    "USA",
    "Europe",
};

char *io2string(char io)
{
    switch (io) {
        case '0':
            return iosupport[0];
        case 'J':
            return iosupport[1];
        case 'K':
            return iosupport[2];
        case 'R':
            return iosupport[3];
        case 'P':
            return iosupport[4];
        case 'T':
            return iosupport[5];
        case 'B':
            return iosupport[6];
        case 'V':
            return iosupport[7];
        case 'F':
            return iosupport[8];
        case 'C':
            return iosupport[9];
        default:
            return iosupport[10];
    }
}

char *region2string(char region)
{
    static char unk[] = "Unknown Region 'X'";
    switch (region) {
        case 'J' :
            return regions[0];
        case 'U' :
            return regions[1];
        case 'E' :
            return regions[2];
        default:
            unk[16] = region;
            return unk;
    }
}

char *term(char *in, char *out, int len)
{
    if (!in || !out || (len <= 0))
        return NULL;

    memcpy(out, in, len);
    out[len] = '\0';

    return out;
}

int main(int argc, char *argv[])
{
    FILE *rom = NULL;
    int ret;
    char buff[HEADER_SIZE];
    struct mdheader *mdh;
    char str[49];
    int inb = 48, outb=128;
    char *inm, *outm, *in_orig, *out_orig;
    int i;
    iconv_t id;

    if (argc != 2)
        return 1;

    id = iconv_open("UTF-8", "SHIFT_JIS");
    if (id == (iconv_t)(-1)) {
        printf("bad iconv_open\n");
        return 1;
    }

    rom = fopen(argv[1], "r");
    if (!rom)
        return 1;

    ret = fseek(rom, HEADER_OFFSET, SEEK_SET);
    if (ret != 0) {
        fclose(rom);
        return 1;
    }

    ret = fread(buff, HEADER_SIZE, 1, rom);
    if (ret != 1) {
        fclose(rom);
        return 1;
    }

    mdh = (struct mdheader *)buff;
    inm = in_orig = malloc(48);
    memcpy(inm, mdh->title_dom, 48);
    outm = out_orig = malloc(128);
    if (!inm || !outm)
        return 1;

    printf("************************************************\n");
    printf("Common title:\n%s\n", term(mdh->title_com, str, 16));
    printf("Copyright:\n%s\n", term(mdh->copyright, str, 8));
    printf("Date:\n%s\n", term(mdh->date, str, 8));

    printf("Game name (domestic):\n");
    ret = iconv(id, &inm, &inb, &outm, &outb);
    if (ret >= 0)
        printf("%s\n", out_orig);
    else
        printf("conversion failure\n");
    printf("Game name (overseas):\n");
    inm = in_orig;
    outm = out_orig;
    inb = 48;
    outb = 128;
    ret = iconv(id, &inm, &inb, &outm, &outb);
    if (ret >= 0)
        printf("%s\n", out_orig);
    else
        printf("conversion failure\n");

    printf("Type:\n%s\n", term(mdh->type, str, 2));
    printf("Product:\n%s\n", term(mdh->product, str, 14));
    printf("Controls:\n");
    for (i = 0; i < 16; i++) {
        if(mdh->controls[i] == ' ')
            continue;
        printf("%s\n", io2string(mdh->controls[i]));
    }

    printf("ROM Start:\n0x%x\n", be32toh(mdh->rom_start));
    printf("ROM End:\n0x%x\n",  be32toh(mdh->rom_end));
    printf("RAM Start:\n0x%x\n", be32toh(mdh->ram_start));
    printf("RAM End:\n0x%x\n",  be32toh(mdh->ram_end));

    printf("Modem:\n%s\n", term(mdh->modem, str, 12));
    printf("Memo:\n%s\n", term(mdh->memo, str, 40));
    printf("Regions:\n");
    for (i = 0; i < 16; i++) {
        if(mdh->regions[i] == ' ')
            continue;
        printf("%s\n", region2string(mdh->regions[i]));
    }
    printf("************************************************\n");

    fclose(rom);
    iconv_close(id);
    return 0;
}
