#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

int rcvcount=0;
void pwrdn_sighandler(int signo)
{
printf("\n sighandler called \n");
if (signo==SIGIO) {
rcvcount++;
}
return;
}

int main(int argc, char **argv)
{
int pwrdn_fd;
int count;
struct sigaction action;
char input;
memset(&action, 0, sizeof(action));
action.sa_handler = pwrdn_sighandler;
action.sa_flags = 0;

sigaction(SIGIO, &action, NULL);

pwrdn_fd = open("/dev/gpio_int", O_RDWR);

if (pwrdn_fd < 0) {
printf("Failed to open device\n");
return 1;
}

fcntl(pwrdn_fd, F_SETOWN, getpid());
fcntl(pwrdn_fd, F_SETFL, fcntl(pwrdn_fd, F_GETFL) | FASYNC);

while(1) {
/* this only returns if a signal arrives */
sleep(86400); /* one day */
if (!rcvcount)
continue;
system("clear");
printf("\t**************** GPIO interrupt raised! *********************\n");
exit(0);
rcvcount=0;
}
}
