#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/mii.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <linux/types.h>
#include <netinet/in.h>

#define PHY_ID_HIGH_WORD 2
#define PHY_ID_LOW_ORD 3
	
static int readPhyReg(const char* ifname, __u16 regNum, __u16* val)
{
	struct mii_ioctl_data *mii = NULL;
	struct ifreq ifr;
	int ret;
	int sockfd = -1;

	if (ifname == NULL)
	{
		printf("invalid ifname \n");
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);

	sockfd = socket(PF_LOCAL, SOCK_DGRAM, 0);
	if (0 > sockfd)
	{
		printf("create socket fail \n");
		return -1;
	}

	/* get phy address */
	ret = ioctl(sockfd, SIOCGMIIPHY, &ifr);
	if (0 > ret)
	{
		printf("get phy address fail \n");
		close(sockfd);
		return -1;
	}

	mii = (struct mii_ioctl_data*)&ifr.ifr_data;
	mii->reg_num = regNum;

	ret = ioctl(sockfd, SIOCGMIIREG, &ifr);
	if (0 > ret)
	{
		printf("get phy register fail \n");
		close(sockfd);
		return -1;
	}

	*val = mii->val_out;
	close(sockfd);
	return 0;
}

static int writePhyReg(const char* ifname, __u16 regNum, __u16 val)
{
	struct mii_ioctl_data *mii = NULL;
	struct ifreq ifr;
	int ret;
	int sockfd = -1;

	if (ifname == NULL)
	{
		printf("invalid ifname \n");
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);

	sockfd = socket(PF_LOCAL, SOCK_DGRAM, 0);
	if (0 > sockfd)
	{
		printf("create socket fail \n");
		return -1;
	}

	/* get phy address */
	ret = ioctl(sockfd, SIOCGMIIPHY, &ifr);
	if (0 > ret)
	{
		printf("get phy address fail \n");
		close(sockfd);
		return -1;
	}

	mii = (struct mii_ioctl_data*)&ifr.ifr_data;
	mii->reg_num = regNum;
	mii->val_in = val;

	ret = ioctl(sockfd, SIOCSMIIREG, &ifr);
	if (0 > ret)
	{
		printf("get phy register fail \n");
		close(sockfd);
		return -1;
	}
	
	close(sockfd);
	return 0;
}


static int getPhyID(const char* ifname, uint32_t* id)
{
	__u16 reg2;
	__u16 reg3;

	int ret;
	/* 获取phy寄存器2 */
	ret = readPhyReg(ifname, PHY_ID_HIGH_WORD, &reg2);
	if (0 > ret)
	{
		printf("get phy reg2 fail \n");
		return -1;
	}

	/* 获取phy寄存器3 */
	ret = readPhyReg(ifname, PHY_ID_LOW_WORD, &reg3);
	if (0 > ret)
	{
		printf("get phy reg3 fail \n");
		return -1;
	}

	*id = reg2 << 16 | reg3;
	
	printf("reg2 = 0x%x, reg3 = 0x%x \n", reg2, reg3);

	return 0;
}

int main(int argc, char *argv[])
{	
	const char* ifname = "eth0";
	int ret;
	uint32_t id;
	ret = getPhyID(ifname, &id);
	printf("id = 0x%x \n", id);
	return 0;
}