#include <png.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

int main()
{
    int fb = open("/dev/fb0", O_RDWR);
    if(fb >= 0)
    {
        struct fb_var_screeninfo vinfo;
        ioctl(fb, FBIOGET_VSCREENINFO, &vinfo);
        int fb_width = vinfo.xres;
        int fb_height = vinfo.yres;
        int fb_bpp = vinfo.bits_per_pixel;
        int fb_bytes = fb_bpp / 8;
        int fb_data_size = fb_width * fb_height * fb_bytes;
        unsigned char* fbdata = (unsigned char*)mmap(0, fb_data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb, (off_t)0);
        FILE *fp = fopen("/etc/bootsplash.png", "rb");
        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        png_infop info_ptr = png_create_info_struct(png_ptr);
        png_init_io(png_ptr, fp);
        png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
        int width, height, bit_depth, color_type, interlace_type, compression_type, filter_method;
        png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, &compression_type, &filter_method);
        unsigned char** row_pointers = malloc(height * sizeof(png_bytep));
        for(int i = 0; i < height; i++)
        {
            row_pointers[i] = malloc(width);
        }
        png_set_rows(png_ptr, info_ptr, &row_pointers);
        printf("Width: %d Height: %d BPP: %d Color type: %d Interlace type: %d Compression type: %d", width, height, bit_depth, color_type, interlace_type, compression_type);
        int voff = (fb_width * 2) - 512;
        int hoff = (fb_height * 2) - 512;
        for(int i = 0; i < 1024; i += 4)
        {
            for(int j = 0; j < 1024; j += 4)
            {
                unsigned char pp = row_pointers[i / 4][j / 4];
                //printf("%d", pp);
                /*fbdata[(i + voff) * fb_width + (j + hoff)] = pp;
                fbdata[(i + voff) * fb_width + (j + hoff) + 1] = pp;
                fbdata[(i + voff) * fb_width + (j + hoff) + 2] = pp;
                fbdata[(i + voff) * fb_width + (j + hoff) + 3] = pp;*/
                fbdata[(i + voff) * fb_width + (j + hoff)] = pp;
                fbdata[(i + voff) * fb_width + (j + hoff) + 1] = pp;
                fbdata[(i + voff) * fb_width + (j + hoff) + 2] = 255;
                fbdata[(i + voff) * fb_width + (j + hoff) + 3] = 255;
            }
        }
        for(int i = 0; i < height; i++)
        {
            free(row_pointers[i]);
        }
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        free(row_pointers);
        munmap(fbdata, fb_data_size);
        close(fb);
    }
    return 0;
}
