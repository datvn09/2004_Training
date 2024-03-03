#include <stdio.h>
#include <stdint.h>

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

uint32_t KEY[4] = {305402420, 305402420, 305402420, 305402420}; 

void encrypt(uint32_t* v) {
    uint32_t v0=v[0], v1=v[1], sum=0, i;             
    uint32_t delta=0x9e3779b9;                      
    for (i=0; i < 32; i++) {                         
        sum += delta;
        v0 += ((v1<<4) + KEY[0]) ^ (v1 + sum) ^ ((v1>>5) + KEY[1]);
        v1 += ((v0<<4) + KEY[2]) ^ (v0 + sum) ^ ((v0>>5) + KEY[3]);
    }                                               
    v[0]=v0; v[1]=v1;
}

void decrypt(uint32_t* v) {
    uint32_t v0=v[0], v1=v[1], sum=0xC6EF3720, i; 
    uint32_t delta=0x9e3779b9;                     
    for (i=0; i<32; i++) {                        
        v1 -= ((v0<<4) + KEY[2]) ^ (v0 + sum) ^ ((v0>>5) + KEY[3]);
        v0 -= ((v1<<4) + KEY[0]) ^ (v1 + sum) ^ ((v1>>5) + KEY[1]);
        sum -= delta;
    }                                              
    v[0]=v0; v[1]=v1;
}

int main() {
    uint32_t v[] = {0x33ae2685, 0x230bcdd5, 0x4f5ac093, 0x3dc3e00a, 0xda19d0a1, 0x32c52ad0, 0xc904ffac, 0x3037b842, 0x9c7bf31e, 0x4b8dfebc, 0x33335ba7, 0x4c4c9188, 0xa555d9a9, 0xaa069852, 0xa177367f, 0x79daa10f, 0x29ca035c, 0x319fbbc8, 0xd51b4a1c, 0x4a1b63b6, 0x99f5d2f1, 0xf35fdd82, 0x7e70314f, 0x42077d00, 0x4f84cb2b, 0x4a73846a, 0xbbb0581e, 0x8c33c34f, 0x4eb73143, 0xac45de0, 0x82592087, 0xc02544fa, 0x56590be4, 0xd2f78e08, 0xb2c9d125, 0x65e106d8, 0x46711844, 0xcf16ec7f, 0xc85dde46, 0x51d873d, 0x50319f0f, 0x8e5370bd, 0x80145a76, 0xbdbe90a6, 0x3a10947e, 0xfaf968c7, 0xac700a03, 0x47e061be, 0xe9e65b90, 0xe3c65a80, 0xd707d969, 0x40e93f77, 0x447cf10e, 0xbc69c7df, 0xd8c669de, 0x36c05ccf, 0x876411ba, 0xb37a6436, 0xcdbeac33, 0x7ba23db9, 0xc18251bd, 0x926d7a16, 0x9ffb0134, 0xc7f9ab96, 0xc635711e, 0x45b69a8, 0x7b0fdd2e, 0xf54849a7, 0x61e5d839, 0x1f12687d, 0xb39a4ba1, 0xd4fa2f5a, 0xc308a7fd, 0xcc0f199b, 0x6b35768, 0xecb39e48, 0xb2c9d125, 0x65e106d8, 0x9e9a0f73, 0xc58bdf39, 0xa9bb76d1, 0xc75ccd7, 0x8473c66, 0x8a4ed0e5, 0xae1dcf9a, 0x214f0ed5, 0xfb6bf695, 0x56e45cc6, 0x47e4e2b9, 0x8e2107d1, 0x5a24b1dc, 0x70599ee2, 0x6cd313ec, 0x4fa221e8, 0x6696e856, 0x62fde305, 0x79958e01, 0x1b99f294, 0x876fd3a, 0x59c1d749, 0x0, 0x0};
    FILE *output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    for (int i = 0; i < ARRAY_LENGTH(v) - 1; i += 2) {
        decrypt(&v[i]);
        // Convert decrypted uint32_t values to ASCII and write to file
        uint8_t bytes[8];
        bytes[0] = (v[i] >> 24) & 0xFF;
        bytes[1] = (v[i] >> 16) & 0xFF;
        bytes[2] = (v[i] >> 8) & 0xFF;
        bytes[3] = v[i] & 0xFF;
        bytes[4] = (v[i + 1] >> 24) & 0xFF;
        bytes[5] = (v[i + 1] >> 16) & 0xFF;
        bytes[6] = (v[i + 1] >> 8) & 0xFF;
        bytes[7] = v[i + 1] & 0xFF;
        fprintf(output_file, "%c%c%c%c%c%c%c%c", bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5], bytes[6], bytes[7]);
    }

    fclose(output_file);
    printf("Decrypted data saved to output.txt\n");

    return 0;
}
