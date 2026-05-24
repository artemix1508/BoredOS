#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/kd.h>

pid_t server_pid = -1;
pid_t client_pid = -1;

void cleanup(int sig) {
    (void)sig;
    ioctl(0, KDSETMODE, (void*)KD_TEXT);
    if (client_pid > 0) {
        kill(client_pid, SIGTERM);
        waitpid(client_pid, NULL, 0);
    }
    if (server_pid > 0) {
        kill(server_pid, SIGTERM);
        waitpid(server_pid, NULL, 0);
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    // 1. Switch TTY console to graphics mode to disable kernel console blitter
    ioctl(0, KDSETMODE, (void*)KD_GRAPHICS);

    // 2. Setup signal handlers for graceful shutdown
    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);

    printf("[startx] Starting X server (Xfbdev)...\n");

    // 3. Fork and run Xfbdev
    server_pid = fork();
    if (server_pid < 0) {
        perror("[startx] fork server failed");
        ioctl(0, KDSETMODE, (void*)KD_TEXT);
        return 1;
    }

    if (server_pid == 0) {
        // Child: run Xfbdev
        // We pass -ac to disable access control by default, so X11 clients can connect.
        char *x_args[] = { "/bin/Xfbdev.elf", "-ac", NULL };
        execv(x_args[0], x_args);
        perror("[startx] execv Xfbdev failed");
        exit(1);
    }

    // 4. Wait for Xfbdev to initialize
    printf("[startx] Waiting for Xfbdev to initialize...\n");
    
    // VFS does not store AF_UNIX socket nodes on disk in BoredOS.
    // Instead, we use a robust fallback sleep of 2.5 seconds to ensure
    // that Xfbdev is fully initialized, listening, and ready to accept connections.
    printf("[startx] Using fallback sleep to allow Xfbdev to initialize...\n");
    usleep(2500 * 1000);

    // Check if X server crashed immediately
    int status = 0;
    if (waitpid(server_pid, &status, WNOHANG) > 0) {
        fprintf(stderr, "[startx] Error: Xfbdev crashed immediately during startup.\n");
        ioctl(0, KDSETMODE, (void*)KD_TEXT);
        return 1;
    }

    printf("[startx] X server is ready!\n");

    // 5. Set DISPLAY environment variable
    setenv("DISPLAY", ":0", 1);

    printf("[startx] Spawning window manager (dwm)...\n");
    client_pid = fork();
    if (client_pid < 0) {
        perror("[startx] fork client failed");
        kill(server_pid, SIGTERM);
        waitpid(server_pid, NULL, 0);
        ioctl(0, KDSETMODE, (void*)KD_TEXT);
        return 1;
    }

    if (client_pid == 0) {
        // Child: run dwm
        char *dwm_args[] = { "/bin/dwm.elf", NULL };
        execv(dwm_args[0], dwm_args);
        perror("[startx] execv dwm failed");
        exit(1);
    }

    // 6. Monitor both server and window manager; exit if either dies
    printf("[startx] Running X session...\n");
    int ret;
    while (1) {
        // Check if X server exited
        ret = waitpid(server_pid, &status, WNOHANG);
        if (ret == server_pid) {
            if (WIFEXITED(status)) {
                printf("[startx] X server exited with status %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("[startx] X server terminated by signal %d\n", WTERMSIG(status));
            } else {
                printf("[startx] X server exited with unknown status %d\n", status);
            }
            if (client_pid > 0) {
                kill(client_pid, SIGTERM);
                waitpid(client_pid, NULL, 0);
            }
            break;
        } else if (ret < 0 && errno != EINTR) {
            printf("[startx] error waiting for X server: %s\n", strerror(errno));
            break;
        }

        // Check if window manager exited
        ret = waitpid(client_pid, &status, WNOHANG);
        if (ret == client_pid) {
            if (WIFEXITED(status)) {
                printf("[startx] Window manager (dwm) exited with status %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("[startx] Window manager (dwm) terminated by signal %d\n", WTERMSIG(status));
            } else {
                printf("[startx] Window manager (dwm) exited with unknown status %d\n", status);
            }
            if (server_pid > 0) {
                kill(server_pid, SIGTERM);
                waitpid(server_pid, NULL, 0);
            }
            break;
        } else if (ret < 0 && errno != EINTR) {
            printf("[startx] error waiting for window manager: %s\n", strerror(errno));
            break;
        }

        usleep(50 * 1000); // 50ms sleep
    }

    printf("[startx] Terminating startx...\n");
    ioctl(0, KDSETMODE, (void*)KD_TEXT);

    printf("[startx] Done!\n");
    return 0;
}
