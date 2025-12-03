#ifndef FRAMES_H
#define FRAMES_H

#include <linux/videodev2.h>

struct buffer {
    void   *start;
    unsigned long length;
};

struct v4l2_format setup_camera();

struct v4l2_requestbuffers get_buffers();

unsigned long get_free_space_kb(const char *path);

void get_free_storage(unsigned long frame_count);

void write_frame_count(unsigned long frame_count);

unsigned long read_frame_count();

int open_camera();

void map_buffers(int fd, struct buffer* buffers, struct v4l2_buffer *buf);

#endif
