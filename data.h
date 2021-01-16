#include "main.h"

unsigned char program_enable[4] = {0xAC,0x53, 0x00, 0x00};
unsigned char chip_erase[4] = {0xAC, 0x80, 0x00, 0x00};
unsigned char write_page[4] = {0x4C, 0x00, 0x00, 0x00};

unsigned char read_signature1 [4] = {0x30,0x00,0x00,0x00}; //the 4th byte is read and write (use SPI_Read_Write(read_signature, 4,4)) func
unsigned char read_signature2 [4] = {0x30,0x00,0x01,0x00};
unsigned char read_signature3 [4] = {0x30,0x00,0x02,0x00};

unsigned char extended_addr [4] = {0x30,0x00,0x02,0x00};


unsigned char *main_program[11] = {
"1000000019C034C033C032C031C030C02FC02EC080",
"100010002DC02CC02BC02AC029C028C027C026C094", 
"1000200025C024C023C022C021C020C01FC01EC0C4", 
"100030001DC01CC011241FBECFEFD4E0DEBFCDBF5A",
"0800400003D000C0F894FFCFCB",
"1000480080E48AB980E48BB987EA91E60197F1F7F1",
"1000580000C000001BB887EA91E60197F1F700C0DD",	
"040068000000F0CFD5","02006C00C9CFFA","00000001FF"
};


