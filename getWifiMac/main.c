#include <stdio.h>    //printf
#include <string.h>   //strncpy
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>   //ifreq
#include <unistd.h>   //close
 
int main(void)
{
    unsigned char ssid[32];
    get_wifi_ssid(ssid);
    printf("dhs ssid:%s \n", ssid);

    return 0;
}

int get_wifi_ssid(unsigned char *pSsid)
{
    int fd;
    struct ifreq ifr;
    char *iface = "wlan0";
    unsigned char *mac, sTmpSsid[32]={0};
     
    fd = socket(AF_INET, SOCK_DGRAM, 0);
 
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
 
    ioctl(fd, SIOCGIFHWADDR, &ifr);
 
    close(fd);
     
    mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
    
    snprintf(sTmpSsid, sizeof(sTmpSsid) - 1, "DVR_%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    toUpperCase(sTmpSsid);
    //printf("dhs ssid:%s \n", sTmpSsid);
    strcpy(pSsid, sTmpSsid);
    return 0;
}
