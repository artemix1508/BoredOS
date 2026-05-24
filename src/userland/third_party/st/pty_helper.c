#define _GNU_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>

int openpty(int *mfd, int *sfd, char *name, const struct termios *ios, const struct winsize *win) {
    int ptmx_fd = open("/dev/ptmx", O_RDWR | O_NOCTTY);
    if (ptmx_fd < 0) {
        return -1;
    }

    char spath[32];
    char *target_name = name ? name : spath;
    if (ptsname_r(ptmx_fd, target_name, 32) != 0) {
        close(ptmx_fd);
        return -1;
    }

    if (unlockpt(ptmx_fd) != 0) {
        close(ptmx_fd);
        return -1;
    }

    int pts_fd = open(target_name, O_RDWR | O_NOCTTY);
    if (pts_fd < 0) {
        close(ptmx_fd);
        return -1;
    }

    if (ios) {
        tcsetattr(pts_fd, TCSAFLUSH, ios);
    }

    if (win) {
        ioctl(pts_fd, TIOCSWINSZ, win);
    }

    *mfd = ptmx_fd;
    *sfd = pts_fd;
    return 0;
}
