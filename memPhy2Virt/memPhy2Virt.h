#ifndef _MEMPHY2VIRT_H_
#define _MEMPHY2VIRT_H_

#define PAGE_SIZE 0x1000
#define PAGE_SIZE_MASK 0xfffffffffffff000

int showRegValue(unsigned long phyAddr, unsigned long len);
int writeReg(unsigned long phyAddr, unsigned long value);
int readReg(unsigned long phyAddr, unsigned long *value);

#endif
