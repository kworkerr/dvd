#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

static volatile sig_atomic_t interrupted = 0;

static void handle_sigint(int sig)
{
    (void)sig;
	interrupted = 1;
}

void draw_logo(int x, int y, int color)
{
    printf("\033[%dm", color);
    printf("\033[%d;%dH  _____", y, x);
    printf("\033[%d;%dH | DVD |", y + 1, x);
    printf("\033[%d;%dH  -----", y + 2, x);
    printf("\033[0m");
}

static void print_help(void)
{
    printf("dvd - A cool DVD bounce in C.\n\n");
    printf("--speed N   animation speed (default: 1)\n");
    printf("--color N   logo color, from 1 to 8\n");
}

static void sleep_us(long delay)
{
	struct timespec time = {
		.tv_sec = delay / 1000000L,
		.tv_nsec = (delay % 1000000L) * 1000L
	};

	nanosleep(&time, NULL);
}

int main(int argc, char **argv)
{
    int x = 1, y = 1;
    int dx = 1, dy = 1;
    int color = 35;
    int speed = 1;
    char *end;
    struct winsize ws;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--speed") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "dvd: '--speed' requires a value\n");
                return 1;
            }

            speed = strtol(argv[++i], &end, 10);

            if (*end != '\0' || speed < 1)
            {
                fprintf(stderr, "dvd: '--speed' only accepts positive integers\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "--color") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "dvd: '--color' requires a value\n");
                return 1;
            }

            long n = strtol(argv[++i], &end, 10);

            if (*end != '\0' || n < 1 || n > 8)
            {
                fprintf(stderr, "dvd: '--color' accepts integers from 1 to 8\n");
                return 1;
            }

            color = 29 + n;
        }
        else
        {
            print_help();
            return 1;
        }
    }

    signal(SIGINT, handle_sigint);

	printf("\033[?25l");

    while (!interrupted)
    {
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
        {
            perror("dvd: ioctl");
            return 1;
        }
		
        printf("\033[2J");
        draw_logo(x, y, color);
        fflush(stdout);

		long delay = 100000L / speed;

		if (delay < 1)
			delay = 1;

		sleep_us(delay);

        x += dx;
        y += dy;

        if (x <= 1 || x + 8 >= ws.ws_col)
            dx = -dx;

        if (y <= 1 || y + 2 >= ws.ws_row)
            dy = -dy;
    }

	printf("\033[?25h");
	return 0;
}
