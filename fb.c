#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>

int main(int argc, char* argv[])
{
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    char *fbp = 0;
    int x = 0, y = 0;
    long int location = 0;

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);//打開framebuffer，取得其fd
    if (!fbfd) {
        printf("Error: cannot open framebuffer device.\n");
        return 1;
    }
    printf("The framebuffer device was opened successfully.\n");

    // 取得顯卡的fix屬性
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information.\n");
        return 1;
    }

    // 取得顯卡的var屬性
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error reading variable information.\n");
        return 1;
    }

    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // 算出整個screen所需要的size，以byte為單位，以便稍後將記憶體map到userspace
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // 這樣就map到userspace了
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                       fbfd, 0);
    if (fbp == MAP_FAILED) {
        printf("Error: failed to map framebuffer device to memory.\n");
        return 1;
    }
    printf("The framebuffer device was mapped to memory successfully.\n");

    x = 100; y = 100;       // screen左上角為0,0，在座標為(100,100)的地方開始畫

    // 畫一個200x200個pixel的正方形
    for (y = 100; y < 300; y++)
        for (x = 100; x < 300; x++) {

            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                       (y+vinfo.yoffset) * finfo.line_length;

            if (vinfo.bits_per_pixel == 32) {

               //以下是填這個pixel的屬性，包括RGB的值，以及透明的程度
                *(fbp + location) = 100;        // Some blue
                *(fbp + location + 1) = 15+(x-100)/2;     // A little green
                *(fbp + location + 2) = 200-(y-100)/5;    // A lot of red
                *(fbp + location + 3) = 0;      // No transparency
            }
        }
    munmap(fbp, screensize); //歸還記憶體
    close(fbfd);//記得要關閉framebuffer的fd
    return 0;
}