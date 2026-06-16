/*
 * monitor_ipc.c
 * PART D -- IPC & Multi-Processing (CLO-4)
 *
 * Compile: gcc -Wall -o monitor_ipc monitor_ipc.c
 * Run:     ./monitor_ipc <directory>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define LOG_FILE "logs/report.txt"
#define MSG_SIZE 256

/* SIGINT handler - both parent and child use this */
void handle_sigint(int sig) {
    printf("\n[PID %d] Caught SIGINT. Shutting down.\n", getpid());
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    /* Register SIGINT handler */
    signal(SIGINT, handle_sigint);

    /* Create pipe: pipefd[0] = read end, pipefd[1] = write end */
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe failed");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    /* ---- PARENT: Scan directory and send file info through pipe ---- */
    if (pid > 0) {
        close(pipefd[0]); /* parent only writes */

        DIR *dir = opendir(argv[1]);
        if (!dir) {
            perror("opendir failed");
            close(pipefd[1]);
            return 1;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            char *name = entry->d_name;

            /* Skip if not a .txt file */
            int len = strlen(name);
            if (len < 5 || strcmp(name + len - 4, ".txt") != 0)
                continue;

            /* Get file size */
            char filepath[MSG_SIZE];
            snprintf(filepath, sizeof(filepath), "%s/%s", argv[1], name);

            struct stat st;
            stat(filepath, &st);

            /* Send "filename size" to child via pipe */
            char msg[MSG_SIZE];
            snprintf(msg, sizeof(msg), "%s %ld\n", name, (long)st.st_size);

            write(pipefd[1], msg, strlen(msg));
            printf("[Parent PID %d] Sent: %s", getpid(), msg);
        }

        closedir(dir);
        close(pipefd[1]); /* closing tells child we're done */

        wait(NULL); /* wait for child to finish */
        printf("[Parent] Done.\n");

    /* ---- CHILD: Read from pipe and log to file ---- */
    } else {
        close(pipefd[1]); /* child only reads */

        /* Make sure logs/ directory exists */
        mkdir("logs", 0755);

        FILE *log = fopen(LOG_FILE, "a");
        if (!log) {
            perror("fopen failed");
            exit(1);
        }

        fprintf(log, "\n--- New Scan ---\n");

        char msg[MSG_SIZE];
        char buf[MSG_SIZE * 2] = {0};
        int buf_len = 0;

        /* Read from pipe until parent closes the write end */
        int n;
        while ((n = read(pipefd[0], buf + buf_len, sizeof(buf) - buf_len - 1)) > 0) {
            buf_len += n;
            buf[buf_len] = '\0';

            /* Process complete lines */
            char *line;
            while ((line = strchr(buf, '\n')) != NULL) {
                *line = '\0';

                fprintf(log, "File: %s\n", buf);
                printf("[Child  PID %d] Logged: %s\n", getpid(), buf);
                fflush(log);

                /* Shift remaining data */
                int remaining = buf_len - (line - buf + 1);
                memmove(buf, line + 1, remaining);
                buf_len = remaining;
                buf[buf_len] = '\0';
            }
        }

        fprintf(log, "--- End of Scan ---\n");
        fclose(log);
        close(pipefd[0]);

        printf("[Child] Logging complete. See %s\n", LOG_FILE);
        exit(0);
    }

    return 0;
}
