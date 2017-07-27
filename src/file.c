/* 
 * File:   file.c
 * Author: root
 *
 * Created on 21 сентября 2016 г., 16:20
 */

#include <X11/Xlib.h>

#include "file.h"

double
memory_free(const char *dir) {

    int mbyte = MBYTE;
    struct statvfs fs_pnki;
    size_t size_free;
    size_t size_all;

    if (statvfs(dir, &fs_pnki) >= 0) {
        size_free = (geteuid() == 0) ? fs_pnki.f_frsize * fs_pnki.f_bfree : fs_pnki.f_frsize * fs_pnki.f_bavail;

        return (size_free / mbyte);
    } else
        return 0;

}

double
memory_all(const char *dir) {

    int mbyte = MBYTE;
    struct statvfs fs_pnki;
    size_t size_free;
    size_t size_all;

    if (statvfs(dir, &fs_pnki) >= 0) {
        size_all = fs_pnki.f_frsize * fs_pnki.f_blocks;

        return (size_all / mbyte);
    } else
        return 0;

}

int
file_copy(const char *from, const char *to) {
    int fromfd = -1, tofd = -1;
    ssize_t nread;
    char buf[BUFSIZE];

    chmod(from, 0755);

    if ((fromfd = open(from, O_RDONLY)) < 0) return -1;
    //if((tofd   = open(to, O_WRONLY | O_CREAT | O_TRUNC | S_IRUSR | S_IWUSR), PERM_FILE) < 0) return -1;
    if ((tofd = creat(to, PERM_FILE)) < 0) return -1;

    while ((nread = read(fromfd, buf, sizeof (buf))) > 0)
        if (write(tofd, buf, nread) != nread) {
            chmod(to, 0755);
            return -1;
        }

    chmod(to, 0755);

    close(fromfd);
    close(tofd);

    return 0;
}

int
xserver_is_start() {
    Display *dsp = NULL;
    do {
        dsp = XOpenDisplay(NULL);
        if (!dsp) {
            break;
        }
        
        free(dsp);
        
        return 0;
    } while (0);

    return -1;
}