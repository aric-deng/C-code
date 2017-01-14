#include <stdio.h>
#include <linux/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <pthread.h>
#include "dvr_type.h"
#include "cg_uart.h"

#define     UART_ERR_OPEN_FAIL      -1
#define     UART_ERR_CONFIG_FAIL    -2

#define MAX_COM_NUM 3
#define GENERAL 1
#define USBTTY 0
#define COM_TYPE 1

#define MAX_PKG_SIZE    64
#define MIN_PKG_SIZE    7
#define PKG_BASIC_LEN   6   //not include data length
#define MAX_WRITE_SIZE_PER_TIME     64
#define	UART_BUFFER_MAX_SIZE	512

#define Printf(string...)	\
do{\
	printf("%s(%d)[%s]: ", __FILE__, __LINE__, __FUNCTION__);\
	printf(string);\
} while(0)

#define UART_DEBUG
#ifdef UART_DEBUG
    //#define UART_DBG(fmt, ...) 
#else
#endif

typedef enum{
    UART_0  =   0,
    UART_1,
    UART_2,
    UART_NUM
}UART_ID;

typedef struct{
    int UartID;
    int BaundRate;
    int DataBite;
    int Parity;
    int StopBite;
}UART_CONFIG;

int     uart_fd = 0;
UINT8	uart_read_buf[UART_BUFFER_MAX_SIZE];
UINT8	uart_write_buf[UART_BUFFER_MAX_SIZE * 2];
int     uart_read_buf_idx = 0;
int     uart_write_buf_idx = 0;

const char rx_cmd_hdr[] = {0xAA, 0x44, 0x4D};
const char tx_cmd_hdr[] = {0xAA, 0x4D, 0x44};

int open_port(int com_port)
{
    int fd;
#if (COM_TYPE == GENERAL)
    char *dev[] = {"/dev/ttySGK0", "/dev/ttySGK1", "/dev/ttySGK2"};
#else
    char *dev[] = {"/dev/ttyUSB0", "/dev/ttyUSB1", "/dev/ttyUSB2"};
#endif

    if((com_port < 0) || (com_port > (MAX_COM_NUM - 1)))
    {
        return -1;
    }

    fd = open(dev[com_port], O_RDWR|O_NOCTTY|O_NDELAY);
    printf("Open the %s fd is %d\n", dev[com_port], fd);
    if(fd < 0)
    {
        perror("Open serial port");
        return -1;
    }

    //Set  the  file  status flags to blocking
    if(fcntl(fd, F_SETFL, 0) < 0)
    {
        perror("fcntl F_SETFL\n");
    }

    //test whether a file descriptor refers to a terminal
    if(isatty(fd) == 0)
    {
        perror("This is not a termial device");
    }
    return fd;
}

int set_com_config(int fd, int baud_rate, int data_bits, char parity, int stop_bits)
{
    printf("The fd is %d The baud_rate is %d data_bits is %d, parity is %c, stop_bits is %d\n",\
           fd, baud_rate, data_bits, parity, stop_bits);
           
    struct termios new_cfg, old_cfg;
    int speed = B115200;

    //gets the parameters associated with the object referred by fd
    if(tcgetattr(fd, &old_cfg) != 0)
    {
        perror("tcgetattr");
        return -1;
    }
    
    new_cfg = old_cfg;
    //sets the terminal to something like the "raw" mode of the old Version 7 terminal driver
    cfmakeraw(&new_cfg);
    new_cfg.c_cflag &= ~CSIZE;

    switch(baud_rate)
    {
        case 2400:
        {
            printf("baud_rate 2400 %d\n", speed);
            speed = B2400;
        }
        break;

        case 4800:
        {
            speed = B4800;
            printf("baud_rate 4800 %d\n", speed);
        }
        break;
        
        case 9600:
        {
            speed = B9600;
            printf("baud_rate 9600 %d\n", speed);
        }
        break;
        
        case 19200:
        {
            speed = B19200;
            printf("baud_rate 19200 %d\n", speed);
        }
        break;
        
        case 38400:
        {
            speed = B38400;
            printf("baud_rate 38400 %d\n", speed);
        }
        break;
        
        case 115200:
        {
            speed = B115200;
            printf("baud_rate 115200 %d\n", speed);
        }
        break;
        
        default:
            speed = B115200;
            break;
    }

    //sets  the  input  baud rate
    cfsetispeed(&new_cfg, speed);
    //sets the output baud rate
    cfsetospeed(&new_cfg, speed);
    printf("speed = %d\n", speed);

    switch(data_bits)
    {
        case 7:
        {
            new_cfg.c_cflag |= CS7;
        }
        break;
        
        case  8:
        {
            new_cfg.c_cflag |= CS8;
            printf("data_bits 8\n");
        }
        break;

        default:
            new_cfg.c_cflag |= CS8;
            break;
    }

    switch(parity)
    {
        case 'n':
        case 'N':
        {
            printf("parity: No\n");
            new_cfg.c_cflag &= ~PARENB;
            new_cfg.c_iflag &= ~INPCK;
        }
        break;
        
        case 'o':
        case 'O':
        {
            new_cfg.c_cflag |= (PARODD | PARENB);
            new_cfg.c_iflag |= INPCK;
        }
        break;

        case 'e':
        case 'E':
        {
            new_cfg.c_cflag |= PARENB;
            new_cfg.c_cflag &= ~PARODD;
            new_cfg.c_iflag |= INPCK;
        }
        break;
        
        case 's':
        case 'S':
        {
            new_cfg.c_cflag &= ~PARENB;
            new_cfg.c_cflag &= ~CSTOPB;
        }
        break;
        
        default:
            new_cfg.c_cflag &= ~PARENB;
            new_cfg.c_iflag &= ~INPCK;
            break;
    }
    
    switch(stop_bits)
    {
        case 1:
        {
            printf("stop_bits 1\n");
            new_cfg.c_cflag &= ~CSTOPB;
        }
        break;
        
        case 2:
        {
            new_cfg.c_cflag |= CSTOPB;
        }
        break;
        
        default:
            new_cfg.c_cflag &= ~CSTOPB;
            break;
    }

    new_cfg.c_cc[VTIME] = 0;
    new_cfg.c_cc[VMIN] = 1;
    //discards data written to the object referred to by fd but not transmitted
    tcflush(fd , TCIFLUSH);
    //the change occurs immediately.
    if((tcsetattr(fd, TCSANOW, &new_cfg)) != 0)
    {
        perror("tcsetattr");
        return -1;
    }
    return 0;
}

int gk_open_com_port(UART_CONFIG uart_config)
{
    signed int fd = 0;

    fd = open_port(uart_config.UartID);
    if(fd == -1)
    {
        return UART_ERR_OPEN_FAIL;
    }

    if(set_com_config(fd,uart_config.BaundRate,uart_config.DataBite,uart_config.Parity,uart_config.StopBite) == -1)
    {
        return UART_ERR_CONFIG_FAIL;
    }

    return fd;
}

int gk_read_com_port(signed int fd,char *read_buf,int read_size)
{
    fd_set fds, rd_fds;
    signed int ret = 0;
    struct timeval tv;
    signed int read_real_size = 0;

    FD_ZERO(&fds);
    //add a given file descriptor to a set
    FD_SET(fd,&fds);

    rd_fds = fds;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    //monitor if one or more of fds become 'ready'
    ret = select(fd+1,&rd_fds,NULL,NULL,&tv);

    if(ret == 0) 
        return 0;
        
    else if(ret < 0)
    {
        perror("select err\n");
        return -1;
    }
    else
    {
        if(FD_ISSET(fd,&rd_fds))
        {
            read_real_size = read(fd,read_buf,read_size);
            if(read_real_size < 0)
            {
                return -1;
            }
            return read_real_size;
        }
        else
        {
            return 0;
        }
    }
}

int SetPthreadStackSize(pthread_attr_t * attr, int stacksize)
{
    int pret = -1;

    pret = pthread_attr_init(attr);
    if (pret != 0)
    {
        return -1;
    }

    pret = pthread_attr_setstacksize(attr, stacksize);
    if (pret != 0)
    {
        return -1;
    }

    return 0;
}

int gk_write_com_port(signed int fd,char* write_buf,int write_size)
{
    fd_set fds,rd_fds,wr_fds;
    signed int ret = 0;
    struct timeval tv;
    signed int write_real_size = 0;

    FD_ZERO(&fds);
    FD_SET(fd,&fds);

    rd_fds = fds;
    wr_fds = fds;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    ret = select(fd+1,NULL,&wr_fds,NULL,&tv);

    if(ret == 0) return 0;
    else if(ret < 0)
    {
        perror("select err\n");
        return -1;
    }
    else
    {
        if(FD_ISSET(fd,&wr_fds))
        {
            write_real_size = write(fd,write_buf,write_size);
            if(write_real_size < 0)
            {
                return -1;
            }
            return write_real_size;
        }
        else
        {
            return 0;
        }
    }

}

typedef struct ST_CG_UART_PKG{
    UINT8 header;
    UINT8 dest_addr;
    UINT8 src_addr;
    UINT8 pkg_len;
    UINT8 cmd;
    UINT8 *pData;
    UINT8 chksum;

} CG_UART_PKG;

static int cg_uart_check_sum(CG_UART_PKG *uart_pkg)
{
    UINT8 sum = 0;
    int i = 0, ret = -1;

    sum = uart_pkg->header + uart_pkg->dest_addr + uart_pkg->src_addr + \
            uart_pkg->pkg_len + uart_pkg->cmd;

    for(; i < uart_pkg->pkg_len - 6; i++){
        sum += uart_pkg->pData[i];
    }

    if (sum == uart_pkg->chksum){
        ret = 0;
    } 

    printf("%s, ret:%d \n", __func__, ret);
    return ret;
}

//add below mutex to avoid multiphread UI trash
pthread_mutex_t write_uart_mutex;

int cg_fill_pkg_data_to_uart_buf(CG_UART_PKG *tx_pkg)
{
    int i;
    UINT8 sum = 0;
    UINT8* pBuf;

    //fill tx_pkg data
    tx_pkg->header = tx_cmd_hdr[0];
    tx_pkg->dest_addr = tx_cmd_hdr[1];
    tx_pkg->src_addr = tx_cmd_hdr[2];
    sum = tx_pkg->header + tx_pkg->dest_addr + tx_pkg->src_addr \
                   + tx_pkg->pkg_len + tx_pkg->cmd;
                   
    for (i = 0; i < (tx_pkg->pkg_len - PKG_BASIC_LEN); i++) {
        sum += tx_pkg->pData[i];
    }
    tx_pkg->chksum = sum;

    //add tx_pkg data to uart write buffer
    pthread_mutex_lock(&write_uart_mutex);
    
    pBuf = uart_write_buf + uart_write_buf_idx;
    *(pBuf++) = tx_pkg->header;
    *(pBuf++) = tx_pkg->dest_addr;
    *(pBuf++) = tx_pkg->src_addr;
    *(pBuf++) = tx_pkg->pkg_len;
    *(pBuf++) = tx_pkg->cmd;
    for (i = 0; i < (tx_pkg->pkg_len - PKG_BASIC_LEN); i++) {
        *(pBuf++) = tx_pkg->pData[i];
    }
    *(pBuf++) = tx_pkg->chksum;

    uart_write_buf_idx += tx_pkg->pkg_len;
    
    pthread_mutex_unlock(&write_uart_mutex);
}

int cg_send_sys_info_to_uart(void)
{
    CG_UART_PKG tx_pkg;
    UINT8 data[8] = {0};

    tx_pkg.pkg_len = PKG_BASIC_LEN + sizeof(data);
    tx_pkg.cmd = CMD_SYS_INFO;
    tx_pkg.pData = data;
    //fill buf data according to system status
    data[0] = 0x9;
    data[1] = 0x0;
    data[2] = 0x0;
    data[3] = 0x1;
    data[4] = 0x2;
    data[5] = 0x0;
    data[6] = 0x0;
    data[7] = 0x0;


    cg_fill_pkg_data_to_uart_buf(&tx_pkg);
}

static int cg_uart_parse_cmd(CG_UART_PKG *uart_pkg)
{
    UINT8 cmd = uart_pkg->cmd;
    UINT8 *pData = uart_pkg->pData;
    //UINT8 data_len = uart_pkg->pkg_len - PKG_BASIC_LEN; //cut off the non data part len

    int ret = 0;

    printf("%s in, cmd:%x \n", __func__, cmd);
    
#if 1
    switch (cmd)
    {
        case CMD_ERR:
            break;
            
        case CMD_SYS_INFO:
            break;
            
        case CMD_PB_STATUS:
            break;
            
        case CMD_SYS_STATUS:
            cg_send_sys_info_to_uart();
            
            break;
            
        case CMD_KEY:
            break;
            
        case CMD_TIME:
            break;
            
        case CMD_REC_CYCLE_TIME:
            break;
            
        case CMD_REC_AUDIO:
            break;
            
        case CMD_LANGUAGE:
            break;
            
        case CMD_GSENSOR_STATUS:
            break;
            
        case CMD_LIGHT_FREQ:
            break;
            
        case CMD_DATE_FORMAT:
            break;
            
        case CMD_SYS_DATE:
            break;

        case CMD_SYS_TIME:
            break;
            
        case CMD_GET_VERSION:
            break;
            
        case CMD_FORMAT:
            break;
            
        case CMD_REQUEST_SYS_PARAM:
            if (pData[0] == 0xFF) {
                //request system params
                
            } else if (pData[0] == 0){
                //factory reset
                
            } else {
                printf("invalid data:%x !\n", pData[0]);
            }
            break;
            
        case CMD_SYS_DATE_TIME:
            break;
            
        case CMD_SET_SYS_PARAM:
            break;
        case CMD_SEND_FILENAME:
            break;
        case CMD_CUR_PAGE_FILENAMES:
            break;
        case CMD_PLAY_FILE:
            break;
        case CMD_POWEROFF:
            break;
        case CMD_GET_REC_TIME:
            break;
        case CMD_GET_ELAPSED_TIME:
            break;

        case CMD_GET_PB_DURATION:
            break;

        default:
            printf("Unknown commad:%x, return !\n", cmd);
            ret = -1;
            break;

    }
#endif    
    return ret;
}

int shift_left_uart_read_buf_data(int shift_data_len)
{
    int left_data_len;
    left_data_len = uart_read_buf_idx - shift_data_len;
    memmove(uart_read_buf, uart_read_buf + shift_data_len, left_data_len);
    memset(uart_read_buf + left_data_len, 0, shift_data_len);
    uart_read_buf_idx = left_data_len;
}

void* UartCap_ReadPthread(void* arg)
{
    int ret = 0;
    char datastr[32];
    UINT8 * ppkg_hdr = NULL;
    CG_UART_PKG rx_pkg;
    UINT8 *buf = NULL;
    int left_data_len = 0;

    rx_pkg.header = rx_cmd_hdr[0];
    rx_pkg.dest_addr = rx_cmd_hdr[1];
    rx_pkg.src_addr = rx_cmd_hdr[2];

    while(1)
    {
        int real_read_size = 0;
        int real_write_size = 0;
        
        real_read_size = gk_read_com_port(uart_fd, uart_read_buf + uart_read_buf_idx, UART_BUFFER_MAX_SIZE);
        if(real_read_size < 0)
        {
            printf("Read Uart Port Fail \n");
            continue;
        } else if (real_read_size == 0) {
            continue;
        }

        printf("======== read size:%d\n", real_read_size);
#if 1        
        int i=0;            
        for(i=0; i<real_read_size; i++)
        {
            //printf("%x \n", uart_read_buf[uart_read_buf_idx+i]);
            printf("0x%02x ", uart_read_buf[uart_read_buf_idx+i]);
        }
        printf("=======\n");
#endif        
        uart_read_buf_idx += real_read_size;

        while(uart_read_buf_idx >= MIN_PKG_SIZE){
            //search command header
            ppkg_hdr = memmem(uart_read_buf, uart_read_buf_idx, rx_cmd_hdr, sizeof(rx_cmd_hdr));
            if (ppkg_hdr){
                printf("match header !\n");
                rx_pkg.pkg_len = *(ppkg_hdr + 3);
                
                //check if pkg_len valid
                if (rx_pkg.pkg_len < MIN_PKG_SIZE || rx_pkg.pkg_len > MAX_PKG_SIZE){
                
                    printf("pkg_len:%d is invalid, shift data to array head! \n", rx_pkg.pkg_len);
                    shift_left_uart_read_buf_data(ppkg_hdr + 3 - uart_read_buf);
                    continue;
                    
                } else if (uart_read_buf_idx < rx_pkg.pkg_len){
                    //check if read_buf have the whole package data
                    printf("uart_read_buf_idx:%d < rx_pkg.pkg_len:%d\n", uart_read_buf_idx, rx_pkg.pkg_len);
                    if(memmem(ppkg_hdr + 3, uart_read_buf + uart_read_buf_idx - (ppkg_hdr + 3), rx_cmd_hdr, sizeof(rx_cmd_hdr))){
                    
                        printf("continue parse next cmd \n");
                        shift_left_uart_read_buf_data(ppkg_hdr + 3 - uart_read_buf);
                        continue;
                        
                    } else {
                        printf("read buf doesn't contain the whole package data, break \n");
                    }
                    break;
                }
                
                rx_pkg.cmd = *(ppkg_hdr + 4);
                buf = malloc(rx_pkg.pkg_len - 6); //data len
                if (buf == NULL){
                    printf("malloc fail! \n");
                    continue;
                }
                
                //copy package data to buf
                strncpy(buf, ppkg_hdr + 5, rx_pkg.pkg_len - 6);
                rx_pkg.pData = buf;
                rx_pkg.chksum = *(ppkg_hdr + rx_pkg.pkg_len - 1);
                
                //checksum and parse cmd
                if(cg_uart_check_sum(&rx_pkg) == 0){
                    ret = cg_uart_parse_cmd(&rx_pkg);
                    
                    if (ret == 0) {
                        printf("parse_cmd ok, shift data to array head! \n");
                        shift_left_uart_read_buf_data(ppkg_hdr + rx_pkg.pkg_len - uart_read_buf);
                    } else if(ret == -1) {
                        printf("parse_cmd fail, shift data to array head! \n");
                        shift_left_uart_read_buf_data(ppkg_hdr + 3 - uart_read_buf);
                    }
                    
                } else {
                    printf("check sum fail, shift data to array head! \n");
                    shift_left_uart_read_buf_data(ppkg_hdr + 3 - uart_read_buf);
                }
                
                free(buf);
            }else {
                printf("not found match header in array, drop data! \n");
                //only move the last 2 bytes to array head, and clear other data
                shift_left_uart_read_buf_data(uart_read_buf_idx - 2);
                break;
            }
        }

    }

    gk_close_com_port(uart_fd);
}

int shift_left_uart_write_buf_data(int shift_data_len)
{
    int left_data_len;
    left_data_len = uart_write_buf_idx - shift_data_len;
    memmove(uart_write_buf, uart_write_buf + shift_data_len, left_data_len);
    memset(uart_write_buf + left_data_len, 0, shift_data_len);
    uart_write_buf_idx = left_data_len;
}

void* UartCap_WritePthread(void* arg)
{
    int real_write_size = 0, write_len;
    char datastr[32];

    pthread_mutex_init(&write_uart_mutex, NULL);
    
    while(1)
    {
        while(uart_write_buf_idx > 0) {
            if (pthread_mutex_trylock(&write_uart_mutex))
            {
                printf("%s, write_uart_mutex is already locked. \n", __func__);
                usleep(200 * 1000);
                continue;
            }

            if (uart_write_buf_idx > MAX_WRITE_SIZE_PER_TIME){
                write_len = MAX_WRITE_SIZE_PER_TIME;
            } else {
                write_len = uart_write_buf_idx;
            }
            
            real_write_size = gk_write_com_port(uart_fd, uart_write_buf, write_len);
            if (real_write_size > 0){
                printf("write %d bytes success \n", real_write_size);
                shift_left_uart_write_buf_data(real_write_size);
            } else {
                printf("gk_write_com_port() fail, real_write_size:%d \n", real_write_size);
            }
            
            pthread_mutex_unlock(&write_uart_mutex);
        }
        
        usleep(200 * 1000);
    }
}

int main(void)
{
    pthread_t pid, pidwr;
    pthread_attr_t attr;
    int ret;
    
    UART_CONFIG uart_cfg =
    {
        .UartID = UART_2, //UART_1,
        .BaundRate = 115200,
        .DataBite = 8,
        .Parity = 'N',
        .StopBite = 1
    };

    uart_fd = gk_open_com_port(uart_cfg);
    if(uart_fd > 0)
    {
        Printf("open uart port successful \n");
    }
    else
    {
        Printf("open uart port fail fd = %d \n",uart_fd);
        return -1;
    }

    SetPthreadStackSize(&attr, 500000);
    ret = pthread_create(&pid, &attr, UartCap_ReadPthread, NULL);
    if (ret < 0)
    {
        Printf("UartCap_ServicePthread err!\n");
        return -1;
    }

#if 1

    ret = pthread_create(&pidwr, &attr, UartCap_WritePthread, NULL);
    if (ret < 0)
    {
        Printf("UartCap_RewritePthread err!\n");
        return -1;
    }
#endif

    while(1);
    
    return 0;

}
