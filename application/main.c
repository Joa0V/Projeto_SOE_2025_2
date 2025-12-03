#include "gps.h"
#include "frame.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/statvfs.h>
#include <linux/videodev2.h>

#define GPS "/dev/serial0"
#define CAMERA "/dev/video0"
#define FRAME_FOLDER "/home/rasp-soe/frames/"
#define FRAME_FILE "/home/rasp-soe/frames/frame_counter.txt"

int main() {

	int fd;
	int gps_fd = -1;
	GpsCoords coordinates;
	char filename[256];
	char genericString[512];
	unsigned long frame_counter;

	struct v4l2_format fmt;
	struct v4l2_requestbuffers req;
	struct buffer buffers[4];
	struct v4l2_buffer buf;
	enum v4l2_buf_type type;

    do
		fd = open_camera(); 
	while(fd == -1);
	
	gps_fd = gps_init(GPS);
	
	if(gps_fd == -1)
		puts("Erro ao inicializar GPS");
	
	puts("camera aberta");		
	
	frame_counter = read_frame_count();
	
    // 1. Configuração da captura
    puts("configurando camera");
    
    fmt = setup_camera(fd);

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
        perror("Erro ao configurar formato");
        return 1;
    }
	puts("solicitando buffers");
    req = get_buffers();

    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
        perror("Erro em VIDIOC_REQBUFS"); 
        return 1;
    }
    
	map_buffers(fd, buffers, &buf);
	
	puts("Iniciando captura");
    
    // 2. Inicia captura
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMON, &type);

	while (1) {
		
		get_free_storage(frame_counter); 

    	memset(&buf, 0, sizeof(buf));
    	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    	buf.memory = V4L2_MEMORY_MMAP;
    	ioctl(fd, VIDIOC_DQBUF, &buf);
    	
    	if (buf.bytesused == 0) {
		    fprintf(stderr, "Aviso: frame vazio — possível desconexão da câmera.\n");
		    close(fd);
		    sleep(2);
		    
		    puts("Tentando abrir camera ");	
		    
		    do
		    	fd = open_camera(); 
		    while(fd == -1);
		    
		    puts("camera aberta");	
		    
		    fmt = setup_camera(fd);
		
		    if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
			    perror("Erro ao configurar formato");
			    return 1;
    		}
			
			req = get_buffers();
			
			if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
    			perror("Erro em VIDIOC_REQBUFS"); 
			    return 1;
			}
			
			map_buffers(fd, buffers, &buf);
			
			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			ioctl(fd, VIDIOC_STREAMON, &type);
		
			continue;	    
		}
		
		coordinates = gps_read(gps_fd);
		if(coordinates.valid)
			snprintf(filename, sizeof(filename), "%sframe_%lu_lat:%.6f_lon:%.6f.jpg", FRAME_FOLDER, frame_counter++, coordinates.latitude, coordinates.longitude);
		else
	    	sprintf(filename, "%sframe_%lu.jpg", FRAME_FOLDER, frame_counter++);
    	
    	FILE *f = fopen(filename, "wb");
    	fwrite(buffers[buf.index].start, buf.bytesused, 1, f);
    	fclose(f);
    	write_frame_count(frame_counter);
    	
    	// Envia a foto salva para a rede
    	
    	snprintf(genericString, sizeof(genericString), "~/Projeto_SOE_2025_2/cpp_detection/build/detection_test %s", filename);
    	
    	system(genericString);

		// Apaga a foto
		sprintf(genericString, "rm %sframe_%lu.jpg", FRAME_FOLDER, frame_counter);
		
	    ioctl(fd, VIDIOC_QBUF, &buf);
	}
		
    return 0;
}

