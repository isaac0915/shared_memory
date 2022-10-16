#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/file.h>
struct SHARE {
	int bomb_x;
	int bomb_y;
	int process;
	int win;
	int pid_p;
	int pid_c;
	int b_p;
	int b_c;
};
int main(int argc, char* argv[])
{
	pid_t   pid;
	int 	i;
	/* fork another process */
	int ship_P[4];
	int ship_C[4];
	int done;

	int fd = shm_open("posixsm", O_CREAT | O_RDWR, 0666);
	ftruncate(fd, 0x400000);
	struct SHARE* p = mmap(NULL, 0x400000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	p->process = 1;
	p->win = -1;
	p->b_p = 0;
	p->b_c = 0;
	pid = fork();
	int tmp = 0;

	if (pid < 0) { /* error occurred */
		fprintf(stderr, "Fork Failed");
		exit(-1);
	}
	else if (pid == 0) { /* child process */
		p->pid_c = getpid();
		srand(atoi(argv[2]));
		i = atoi(argv[2]);
		while (p->win == -1) {
			if (p->process == 0) {
				done = 0;
				if (tmp == 0) {
					printf("[%d Child]: Random Seed %d\n ", getpid(), i);
					tmp++;
					p->process = 1;
				}
				else if (tmp == 1) {
					ship_C[0] = rand() % 4;
					ship_C[1] = rand() % 4;
					while (done == 0) {
						ship_C[2] = rand() % 4;
						ship_C[3] = rand() % 4;
						if (abs(abs((ship_C[2] - ship_C[0])) + abs((ship_C[3] - ship_C[1]))) == 1) {
							done = 1;
						}
					}
					printf("[%d Child]: The gunboat: (%d,%d)(%d,%d)\n ", getpid(), ship_C[0], ship_C[1], ship_C[2], ship_C[3]);
					tmp++;
					p->process = 1;
				}
				else if (tmp == 2 && p->process == 0) {
					p->process = 1;
					tmp++;

				}
				if (tmp >= 3 && p->process == 0) {
					if (p->bomb_x == ship_C[0] && p->bomb_y == ship_C[1] || p->bomb_x == ship_C[2] && p->bomb_y == ship_C[3]) {//¦³À»¤¤
						if (p->bomb_x == ship_C[0] && p->bomb_y == ship_C[1]) {
							ship_C[0] = -1;
							ship_C[1] = -1;
						}
						else {
							ship_C[2] = -1;
							ship_C[3] = -1;
						}
						if (ship_C[0] == -1 && ship_C[1] == -1 && ship_C[2] == -1 && ship_C[3] == -1) {
							printf("[%d Child]: hit and sinking\n", getpid());
							p->win = 1;
							break;
						}
						else {
							printf("[%d Child]: hit\n", getpid());
							p->bomb_x = rand() % 4;
							p->bomb_y = rand() % 4;
							printf("[%d Child]: bombing (%d,%d)\n ", getpid(), p->bomb_x, p->bomb_y);
							p->b_c++;
							tmp++;
							p->process = 1;
						}

					}
					else {
						printf("[%d Child]: missed\n", getpid());
						p->bomb_x = rand() % 4;
						p->bomb_y = rand() % 4;
						printf("[%d Child]: bombing (%d,%d)\n ", getpid(), p->bomb_x, p->bomb_y);
						p->b_c++;
						tmp++;
						p->process = 1;
					}
				}
			}

		}
		exit(0);
	}
	else { /* parent process */
		/* parent will wait for the child to complete */
		srand(atoi(argv[1]));
		i = atoi(argv[1]);
		p->pid_p = getpid();
		while (p->win == -1) {
			if (p->process == 1) {
				done = 0;
				if (tmp == 0) {
					printf("[%d Parent]: Random Seed %d\n ", getpid(), i);
					tmp++;
					p->process = 0;
				}
				else if (tmp == 1 && p->process == 1) {
					ship_P[0] = rand() % 4;
					ship_P[1] = rand() % 4;
					while (done == 0) {
						ship_P[2] = rand() % 4;
						ship_P[3] = rand() % 4;
						if (abs(abs(ship_P[2] - ship_P[0]) + abs(ship_P[3] - ship_P[1])) == 1) {
							done = 1;
						}
					}
					printf("[%d Parent]: The gunboat: (%d,%d)(%d,%d)\n ", getpid(), ship_P[0], ship_P[1], ship_P[2], ship_P[3]);
					tmp++;
					p->process = 0;
				}
				else if (tmp == 2 && p->process == 1) {
					p->process = 0;
					tmp++;

				}
				else if (tmp >= 3 && p->process == 1) {
					if (tmp == 3) {
						p->bomb_x = rand() % 4;
						p->bomb_y = rand() % 4;
						printf("[%d Parent]: bombing (%d,%d)\n ", getpid(), p->bomb_x, p->bomb_y);
						p->b_p++;
						tmp++;
						p->process = 0;
					}
				}
				if (tmp >= 4 && p->process == 1) {


					if (p->bomb_x == ship_P[0] && p->bomb_y == ship_P[1] || p->bomb_x == ship_P[2] && p->bomb_y == ship_P[3]) {

						if (p->bomb_x == ship_P[0] && p->bomb_y == ship_P[1]) {
							ship_P[0] = -1;
							ship_P[1] = -1;
						}
						else {
							ship_P[2] = -1;
							ship_P[3] = -1;
						}
						if (ship_P[0] == -1 && ship_P[1] == -1 && ship_P[2] == -1 && ship_P[3] == -1) {
							printf("[%d Parent]: hit and sinking\n", getpid());
							p->win = 0;
							break;
						}
						else {
							printf("[%d Parent]: hit\n", getpid());
							p->bomb_x = rand() % 4;
							p->bomb_y = rand() % 4;
							printf("[%d Parent]: bombing (%d,%d)\n ", getpid(), p->bomb_x, p->bomb_y);
							p->b_p++;
							tmp++;
							p->process = 0;
						}

					}
					else {
						printf("[%d Parent]: missed\n", getpid());
						p->bomb_x = rand() % 4;
						p->bomb_y = rand() % 4;
						printf("[%d Parent]: bombing (%d,%d)\n ", getpid(), p->bomb_x, p->bomb_y);
						p->b_p++;
						tmp++;
						p->process = 0;
					}
				}

			}

		}
		if (p->win == 1) {
			printf("[%d Parent]: %d wins with %d bombs\n", getpid(), p->pid_p, p->b_p);
		}
		else {
			printf("[%d Parent]: %d wins with %d bombs\n", getpid(), p->pid_c, p->b_c);
		}
		exit(0);
	}
	return 0;
}
