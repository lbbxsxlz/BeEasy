#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include "memPhy2Virt.h"

void * _memMap(unsigned long phy_addr, unsigned long size, unsigned long *page_diff, unsigned long *size_in_page)
{
	unsigned long phy_addr_in_page;
	void *addr = NULL;
	int fd = -1;

	if (size == 0) {
		perror("_memMap():size can't be zero!\n");
		return NULL;
	}

	fd = open ("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) {
		perror("_memMap():open /dev/mem failed! \n");
		return NULL;
	}

	/* addr align in page_size(4K) */
	phy_addr_in_page = phy_addr & PAGE_SIZE_MASK;
	*page_diff = phy_addr - phy_addr_in_page;

	/* size in page_size */
	*size_in_page =((size + *page_diff - 1) & PAGE_SIZE_MASK) + PAGE_SIZE;

	addr = mmap((void *)0, *size_in_page, PROT_READ|PROT_WRITE, MAP_SHARED, fd, phy_addr_in_page);
	if (addr == MAP_FAILED) {
		printf("_memMap():mmap @ 0x%lx error!\n", phy_addr_in_page);
		close(fd);
		fd = -1;
		return NULL;
	}

	if(fd >= 0) {
		close(fd);
		fd = -1;
	}

	return (void *)(addr + *page_diff);
}

int _memUnmap(void * virtAddr, unsigned long page_diff, unsigned long size_in_page)
{

	unsigned long virtAddrBase = (unsigned long)virtAddr - page_diff;

	if(munmap((void *)virtAddrBase, size_in_page) != 0) {
		perror("_memUnmap(): munmap failed!\n");
		return -1;
	}

	return 0;
}

int showRegValue(unsigned long phyAddr, unsigned int len)
{
	void* virtAddr = NULL;
	void* tmpAddr = NULL;
	int ret = -1;
	unsigned long page_diff = 0;
	unsigned long size_in_page = 0;
	unsigned int i;
	unsigned int value;
	

	virtAddr = _memMap(phyAddr, 1, &page_diff, &size_in_page);
	if (NULL == virtAddr) {
		perror("_memMap fail \n");
		return -1;
	}
	
	tmpAddr = virtAddr;

	for (i = 0; i < len; i++) {
		value = *(unsigned int*)tmpAddr;
		printf("Reg[0x%lx] = 0x%x \n", phyAddr + i * sizeof(unsigned int), value);
		tmpAddr += sizeof(unsigned int);
	}

	ret = _memUnmap(virtAddr, page_diff, size_in_page);

	if (0 > ret) {
		perror("_memUnmap fail \n");
		return -1;
	}
	
	return 0;
}


int readReg(unsigned long phyAddr, unsigned int *value)
{
	void* virtAddr = NULL;
	int ret = -1;
	unsigned long page_diff = 0;
	unsigned long size_in_page = 0;
	

	virtAddr = _memMap(phyAddr, 1, &page_diff, &size_in_page);
	if (NULL == virtAddr) {
		perror("_memMap fail \n");
		return -1;
	}
	
	*value = *(unsigned int*)virtAddr;

	ret = _memUnmap(virtAddr, page_diff, size_in_page);

	if (0 > ret) {
		perror("_memUnmap fail \n");
		return -1;
	}

	return 0;
}

int writeReg(unsigned long phyAddr, unsigned int value)
{
	void* virtAddr = NULL;
	int ret = -1;
	unsigned long page_diff = 0;
	unsigned long size_in_page = 0;
	

	virtAddr = _memMap(phyAddr, 1, &page_diff, &size_in_page);
	if (NULL == virtAddr) {
		perror("_memMap fail \n");
		return -1;
	}
	
	*(unsigned int*)virtAddr = value;

	ret = _memUnmap(virtAddr, page_diff, size_in_page);

	if (0 > ret) {
		perror("_memUnmap fail \n");
		return -1;
	}

	return 0;
}
