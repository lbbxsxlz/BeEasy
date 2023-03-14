
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

/* refer to glibc sysdeps/posix/system.c: __libc_system/do_system */
int exec_shell(const char *cmd)
{
        int status = 0;
        pid_t pid;
        struct sigaction sa;
        struct sigaction sig_int, sig_quit;
        sigset_t omask;

        if (NULL == cmd) {
                return 0;
        }

        /* ignore signal */
        sa.sa_handler = SIG_IGN;
        sa.sa_flags = 0;
        /* clear signal set */
        sigemptyset(&sa.sa_mask);

        /* save the previous action */
        sigaction(SIGINT, &sa, &sig_int);
        sigaction(SIGQUIT, &sa, &sig_quit);

        /* add SIGCHLD to avoid the No child processes error */
        sigaddset(&sa.sa_mask, SIGCHLD);
        /* block the signal, save the previous action */
        sigprocmask(SIG_BLOCK, &sa.sa_mask, &omask);

        pid = vfork();
        if (pid < 0) {
                status = -1;
        } else if (pid == 0) {
                /* restore the signal action in child process */
                sigaction(SIGINT, &sig_int, (struct sigaction *)NULL);
                sigaction(SIGQUIT, &sig_quit, (struct sigaction *)NULL);
                sigprocmask(SIG_SETMASK, &omask, (sigset_t *)NULL);

                /* Exec the shell command */
                /* The exec() functions return only if an error has occurred */
                (void)execl("/bin/sh", "sh", "-c", cmd, NULL);
                /* return exec error */
                _exit(127);
        } else {
                if (waitpid(pid, &status, 0) != pid) {
                        status = -1;
                }
        }

        /* restore the signal action in parent process */
        sigaction(SIGINT, &sig_int, (struct sigaction *)NULL);
        sigaction(SIGQUIT, &sig_quit, (struct sigaction *)NULL);
        sigprocmask(SIG_SETMASK, &omask, (sigset_t *)NULL);

        return status;
}

int exec_shell_with_result(const char *cmd, char *buf, int size)
{
        FILE *fp;

        fp = popen(cmd, "r");
        if (fp == NULL) {
                printf("Failed to popen: %s\n", strerror(errno));
                return -1;
        }

        if (fgets(buf, size - 1, fp) == NULL) {
                printf("Failed to get the results: %s\n", strerror(errno));
                pclose(fp);
                return -1;
        }

        pclose(fp);
        return 0;
}

int main(int argc, char *argv[])
{
        int ret = exec_shell("cat /etc/passwd > ./passwd");

        char buf[10];

        ret = exec_shell_with_result("cat /etc/passwd | grep root | wc -l", buf, sizeof(buf));
        if (ret < 0) {
                printf("Failed to run_shell_with_resut.\n");
        }

        printf("result:%d\n", atoi(buf));
        return ret;
}
