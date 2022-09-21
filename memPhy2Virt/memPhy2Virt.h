#ifndef _MEMPHY2VIRT_H_
#define _MEMPHY2VIRT_H_

#define PAGE_SIZE 0x1000
#define PAGE_SIZE_MASK 0xfffffffffffff000

int showRegValue(unsigned long phyAddr, unsigned int len);
int writeReg(unsigned long phyAddr, unsigned int value);
int readReg(unsigned long phyAddr, unsigned int *value);

#endif
