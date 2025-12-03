#include "frame.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/statvfs.h>
#include <linux/videodev2.h>

#define CAMERA "/dev/video0"
#define FRAME_FOLDER "/home/rasp-soe/frames/"
#define FRAME_FILE "/home/rasp-soe/frames/frame_counter.txt"

struct v4l2_format setup_camera()
{
	struct v4l2_format fmt;
	memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    return fmt;
}

struct v4l2_requestbuffers get_buffers()
{
	struct v4l2_requestbuffers req;
	memset(&req, 0, sizeof(req));
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;	
    return req;
}

unsigned long get_free_space_kb(const char *path) 
{
    struct statvfs stat;
    if (statvfs(path, &stat) != 0) {
        perror("statvfs");
        return 0;
    }
    return (stat.f_bavail * stat.f_frsize) / 1024;
}

void get_free_storage(unsigned long frame_count)
{
	char logMessage[64];
	
	if (frame_count % 100 == 0) {  // verifica a cada 100 frames
    	unsigned long livre_kb = get_free_space_kb(FRAME_FOLDER);
    	if (livre_kb < 4000000) { // menos de ~4 GB livres, finaliza a execução
        	printf("%s", logMessage);
   	    	exit(2);
    	}
	}
}

void write_frame_count(unsigned long frame_count)
{
	char sysMessage[64];
	int fd = open(FRAME_FILE, O_WRONLY, 0644);
	close(fd);
	
	if(fd != -1) // Se o arquivo existe, joga o argumento lá dentro
		sprintf(sysMessage, "echo %lu > %s", frame_count, FRAME_FILE);	
	else // Se não, cria o arquivo e joga zero lá dentro 
		sprintf(sysMessage, "echo %d > %s", 0, FRAME_FILE);
	
	system(sysMessage);
}

unsigned long read_frame_count()
{
	unsigned long counter = 0;
	char counter_array[32];
	char *endptr;
	
	int fd = open(FRAME_FILE, O_RDONLY);

	// Se o arquivo não existe, cria com write_frame_count e abre novamente
	if(fd == -1) {
		write_frame_count(counter);
		fd = open(FRAME_FILE, O_RDONLY);
	}
	// Se o arquivo existe, extrai o número, se não, retorna zero
	if(fd != -1){
		read(fd, counter_array, sizeof(counter_array));
		counter = strtoul(counter_array, &endptr, 10);
	}
	
	close(fd);
	return counter;
}

int open_camera()
{ 
    return open(CAMERA, O_RDWR);
}

void map_buffers(int fd, struct buffer* buffers, struct v4l2_buffer *buf) 
{
    for (int i = 0; i < 4; i++) {
        memset(buf, 0, sizeof(*buf));
        buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf->memory = V4L2_MEMORY_MMAP;
        buf->index = i;

        ioctl(fd, VIDIOC_QUERYBUF, buf);
        buffers[i].length = buf->length;
        buffers[i].start = mmap(NULL, buf->length,
            PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf->m.offset);

        ioctl(fd, VIDIOC_QBUF, buf);
    }
}
