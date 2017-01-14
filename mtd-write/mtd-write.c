#include <mtd/mtd-user.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int updata_process_value = 0;
unsigned int update_crc32_table[256];


int make_crc32_table()
{
    unsigned int c;
    int i = 0;
    int bit = 0;

    for(i = 0; i < 256; i++)
    {
        c  = (unsigned int)i;

        for(bit = 0; bit < 8; bit++)
        {
            if(c&1)
            {
                c = (c >> 1)^(0xEDB88320);
            }
            else
            {
                c =  c >> 1;
            }

        }

        update_crc32_table[i] = c;
    }

    return 0;

}

unsigned int make_crc(unsigned int crc, unsigned char *string, unsigned int size)
{

    while(size--)
        crc = (crc >> 8)^(update_crc32_table[(crc ^ *string++)&0xff]);

    return crc;
}

int crc_check(int w_data_len, char *w_data_buff, int r_data_len, char *r_data_buff)
{

    unsigned int writeCrc = 0xffffffff;
    unsigned int readCrc  = 0xFFFFFFFF;

    writeCrc = make_crc(writeCrc, w_data_buff, w_data_len);

    readCrc  = make_crc(readCrc, r_data_buff, r_data_len);

    if (writeCrc != readCrc)
    {
        printf("update write err \n");
        printf("writeCrc : 0x%x  ReadCrc : 0x%x\n",writeCrc, readCrc);
        return -1;
    }

    return 0;

}


int gk_mtd_part_erase(const char *mtdpart)
{
    int mtd_fd = 0,ret = 0;
    int erase_blk_size = 0,blk_total_size = 0,blk_erase_offset = 0;
    mtd_info_t mtd_blk_info;
    erase_info_t erase_info;

    mtd_fd = open(mtdpart,O_RDWR);
    if(mtd_fd < 0)
    {
        perror("gk_mtd_block_erase,open mtd fail\n");
        return -1;
    }

    ret = ioctl(mtd_fd,MEMGETINFO,&mtd_blk_info);
    printf("mtd erase size [%d] mtd blk size [%d]\n",mtd_blk_info.erasesize,mtd_blk_info.size);
    if(ret != 0)
    {
        perror("gk_mtd_block_erase,get mtd info fail\n");
        close(mtd_fd);
        return -1;
    }

    blk_total_size = mtd_blk_info.size;
    erase_blk_size = mtd_blk_info.erasesize;

    while(blk_erase_offset < blk_total_size)
    {
        erase_info.start = blk_erase_offset;
        if(erase_info.start + erase_blk_size < blk_total_size)
        {
            erase_info.length = erase_blk_size;
        }
        else
        {
            erase_info.length = blk_total_size - erase_info.start;
        }


        ret = ioctl(mtd_fd,MEMERASE,&erase_info);
        if(ret != 0)
        {
            perror("gk_mtd_block_erase erase mtd fail \n");
            close(mtd_fd);
            return -1;
        }

        updata_process_value = (int)((blk_erase_offset*100) / blk_total_size / 2);//1.8	(0 - 55 )
        printf("-----erase_offset = %d upgrade_process_value = %d\n",blk_erase_offset,updata_process_value);

        blk_erase_offset += erase_info.length;
    }

    close(mtd_fd);

    return 0;
}


int gk_mtd_block_write(const char*mtdblk, char *wrbuf,unsigned int wrbufsize)
{
    int mtd_fd = 0,mtd_rd = 0,ret = 0;
    int real_wr_size = 0,wr_blk_size = 0,wr_ptr = 0;
    struct stat mtd_fd_info;
    char *rdbuftmp = NULL;

    mtd_fd = open(mtdblk,O_RDWR);
    if(mtd_fd < 0)
    {
        perror("gk_mtd_block_write,open mtd fail\n");
        return -1;
    }

    mtd_rd = open(mtdblk,O_RDWR);
    if(mtd_rd < 0)
    {
        perror("gk_mtd_block_write,open mtd fail 2\n");
        close(mtd_fd);
        return -1;
    }

    ret = gk_mtd_part_erase(mtdblk);
    if(ret != 0)
    {
        printf("gk_mtd_block_write block erase fail \n");
        close(mtd_fd);
        close(mtd_rd);
        return -1;
    }

    updata_process_value = 50;

    make_crc32_table();

    ret = stat(mtdblk,&mtd_fd_info);
    printf("write mtd size [%d] \n",mtd_fd_info.st_blksize);
    if(ret != 0)
    {
        printf("gk_mtd_block_write get mtd fd info fail \n");
        close(mtd_fd);
        close(mtd_rd);
        return -1;
    }

    rdbuftmp = malloc(mtd_fd_info.st_blksize);
    if(rdbuftmp == NULL)
    {
        printf("gk_mtd_block_write malloc fail \n");
        close(mtd_fd);
        close(mtd_rd);
        return -1;
    }

    while(1)
    {
        if(wr_ptr + mtd_fd_info.st_blksize < wrbufsize)
        {
            wr_blk_size = mtd_fd_info.st_blksize;
        }
        else
        {
            wr_blk_size = wrbufsize - wr_ptr;
        }

        real_wr_size = write(mtd_fd,(const void*)(wrbuf+wr_ptr),wr_blk_size);
        if(real_wr_size != wr_blk_size)
        {
            printf("gk_mtd_block_write : write err \n");
            close(mtd_fd);
            close(mtd_rd);
			free(rdbuftmp);
            return -1;
        }

        real_wr_size = read(mtd_rd,rdbuftmp,wr_blk_size);
        if(real_wr_size != wr_blk_size)
        {
            printf("gk_mtd_block_write : read err \n");
            close(mtd_fd);
            close(mtd_rd);
			free(rdbuftmp);
            return -1;
        }

        if(crc_check(wr_blk_size,wrbuf+wr_ptr,wr_blk_size,rdbuftmp) != 0)
        {
            printf("gk_mtd_block_write : crc err \n");
            close(mtd_fd);
            close(mtd_rd);
			free(rdbuftmp);
            return -1;
        }

        wr_ptr += wr_blk_size;
        updata_process_value = (int)(((wr_ptr*100) / wrbufsize / 2) + 50);
        printf("wr_prt[%d] wrbufsize[%d],Upgrade  %d complete. \n",wr_ptr,wrbufsize,updata_process_value);

        if(wr_ptr >= wrbufsize)
        {
            break;
        }
    }

    close(mtd_fd);
    close(mtd_rd);
	free(rdbuftmp);

	printf("gk_mtd_block_write ok\n");
	
    return 0;
	
}

#define FLASH_UPDATE_FLAG    "flash_update_system"
#define UPDATE_FLAG_SIZE  32

typedef struct st_update_info{
	char update_flag[32];
	unsigned int pkg_in_flash_addr;
	unsigned int pkg_size;	
} ST_UPDATE_INFO;

void write_update_info(unsigned int offset, unsigned int length)
{
	ST_UPDATE_INFO st_update;
	memset(&st_update, 0, sizeof(st_update));
	memcpy(st_update.update_flag, FLASH_UPDATE_FLAG, strlen(FLASH_UPDATE_FLAG) + 1);
	st_update.pkg_in_flash_addr = offset; //0x06000000;
	st_update.pkg_size = length; //0xCE0400;
	printf("update info: flag:%s, offset:0x%x, size:0x%x \n", st_update.update_flag, st_update.pkg_in_flash_addr, st_update.pkg_size);
	gk_mtd_block_write("/dev/mtd2", &st_update, sizeof(ST_UPDATE_INFO));
	
}

int main(int argc, char * argv[])
{
	if (argc != 3){	
		printf("Usage: write_flash <write_file_path> <flash_offset>  \n");
		return -1;
	}
	
	unsigned int offset = strtoul(argv[1], NULL, 0);
	unsigned int length = strtoul(argv[2], NULL, 0);
	write_update_info(offset, length);
	
	return 0;
}