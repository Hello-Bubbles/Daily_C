#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

int ascii_int(int num)
{
    if (num >= '0' && num <= '9') return num - '0';
    if (num >= 'A' && num <= 'F') return num - 'A' + 10;
    if (num >= 'a' && num <= 'f') return num - 'a' + 10;   
    return -1;                                              
}

//  skip whitespace safely
int next_hex_char(FILE *fp)
{
    int c;
    do {
        c = fgetc(fp);
        if (c == EOF) return EOF;
    } while (isspace((unsigned char)c));
    return c;
}

int whole_num(int first_char, FILE *fp)
{
    //  handle EOF safely
    if (first_char == EOF) return EOF;

    int ch = next_hex_char(fp);
    if (ch == EOF) return EOF;

    int hi = ascii_int(first_char);
    int lo = ascii_int(ch);

    //  stop on invalid hex digit
    if (hi < 0 || lo < 0) return EOF;

    return (hi << 4) | lo;
}

int main()
{
    FILE *fp1;
    int len, bytes;
    int up_nib, lo_nib;
    fp1 = fopen("encoded.txt", "r");    
    if (fp1 == NULL) {
        printf("files doesn't exist");
        return -1;
    }

    // read first code byte
    len = whole_num(next_hex_char(fp1), fp1);

    while (len != EOF)                      
    {
        // ignore stray delimiters if present
        if (len == 0x00) {
            len = whole_num(next_hex_char(fp1), fp1);
            continue;
        }

        //  handle 0xFF blocks too (copy 254, no implied 00)
        if (len == 0xFF) {
            for (int i = 0; i < 254; i++) {
                bytes = whole_num(next_hex_char(fp1), fp1);
                if (bytes == EOF || bytes == 0x00) { len = EOF; break; } // malformed/EOF
                up_nib = (bytes >> 4) & 0x0F;
                lo_nib = bytes & 0x0F;
                printf("%x%x", up_nib, lo_nib);
            }

            // read next (delimiter or next code)
            bytes = whole_num(next_hex_char(fp1), fp1);
            if (bytes == EOF) break;
            if (bytes == 0x00) {
                len = whole_num(next_hex_char(fp1), fp1);
            } else {
                len = bytes; // continue with next code (no "00" inserted for FF)
            }
            continue;
        }

        // normal block: copy (len-1) data bytes
        for (int i = 0; i < (len - 1); i++) {
            bytes = whole_num(next_hex_char(fp1), fp1);
            if (bytes == EOF || bytes == 0x00) { len = EOF; break; } 
            up_nib = (bytes >> 4) & 0x0F;
            lo_nib = bytes & 0x0F;
            printf("%x%x", up_nib, lo_nib);
        }
        if (len == EOF) break;

        // now read the next encoded byte: delimiter OR next code
        bytes = whole_num(next_hex_char(fp1), fp1);
        if (bytes == EOF) break;

        if (bytes == 0x00) {
            // end-of-frame: do NOT insert implied 00
            len = whole_num(next_hex_char(fp1), fp1);       //  safe after delimiter
        } else {
            // frame continues: insert implied 00 (because len != 0xFF here)
            printf("00");
            len = bytes;  // next code
        }
    }
}
