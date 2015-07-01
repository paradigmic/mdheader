#include <stdio.h>
#include <string.h>

#define HEADER_SIZE 0x100
#define HEADER_OFFSET 0x100

struct mdheader {
    char line1[16];
    char copyright[8];
    char date[8];
    char title_dom[48];
    char title_over[48];
    char type[2];
    char product[14];
    char controls[16];
    char rom[8];
    char ram[8];
    char external[12];
    char modem[12];
    char memo[40];
    char regions[16];
};

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

    if (argc != 2)
        return 1;

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

    printf("Common title:\n%s\n", term(mdh->line1, str, 16));
    printf("Copyright:\n%s\n", term(mdh->copyright, str, 8));
    printf("Date:\n%s\n", term(mdh->date, str, 8));
    printf("Game name (domestic):\n%s\n", term(mdh->title_dom, str, 48));
    printf("Game name (overseas):\n%s\n", term(mdh->title_over, str, 48));
    printf("Type:\n%s\n", term(mdh->type, str, 2));
    printf("Product:\n%s\n", term(mdh->product, str, 14));
    printf("Controls:\n%s\n", term(mdh->controls, str, 16));
    printf("Modem:\n%s\n", term(mdh->modem, str, 12));
    printf("Memo:\n%s\n", term(mdh->memo, str, 40));
    printf("Regions:\n%s\n", term(mdh->regions, str, 16));

    fclose(rom);
    return 0;
}
