#include "stb_image.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

const char *script = "set terminal pdf\n"
                     "set output '%s'\n"
                     "set nokey\n"
                     "set xrange [0:%d]\n"
                     "set yrange [-%d:0]\n"
                     "plot '/proc/self/fd/%d' with points\n";

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr,
		    "usage: %s <infile> <outfile>\n"
		    "    infile:  image to render\n"
		    "    outfile: PDF file to save graph to\n",
		    argv[0]);
		return 1;
	}

	char *infile = argv[1], *outfile = argv[2];
	int width, height;
	// load image as grayscale (1 channel)
	unsigned char *data = stbi_load(infile, &width, &height, NULL, 1);
	if (!data) {
		fprintf(stderr, "%s: failed to decode input '%s': %s\n", argv[0], infile,
		    stbi_failure_reason());
		return 1;
	}

	// set up 2 pipes, one for the gnuplot script and one for the data
	int pipe_script[2], pipe_data[2];
	if (pipe(pipe_script) != 0) {
		perror("error with pipe: ");
		return 1;
	}
	if (pipe(pipe_data) != 0) {
		perror("error with pipe: ");
		return 1;
	}

	if (fcntl(pipe_script[0], F_SETFL, O_NONBLOCK) < 0) {
		perror("error with fcntl: ");
		return 1;
	}
	if (fcntl(pipe_data[0], F_SETFL, O_NONBLOCK) < 0) {
		perror("error with fcntl: ");
		return 1;
	}

	pid_t pid = fork();
	if (pid == 0) {
		// child
		// send script that we're gonna receive to stdin (which will be gnuplot's stdin)
		if (dup2(pipe_script[0], STDIN_FILENO) < 0) {
			perror("error with dup2: ");
			return 1;
		}
		// close our write ends (parent still has them open)
		close(pipe_script[1]);
		close(pipe_data[1]);
		// run gnuplot
		execlp("gnuplot", "gnuplot", (char *) NULL);
	} else {
		// parent
		// close our read ends (child still has them open)
		close(pipe_script[0]);
		close(pipe_data[0]);
		// send the script to the child process
		dprintf(pipe_script[1], script, outfile, width, height, pipe_data[0]);
		close(pipe_script[1]);
		// iterate through pixels
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (data[y * width + x] < 128) {
					// send it to the data stream
					// y is negated because gnuplot's positive y extends upwards
					dprintf(pipe_data[1], "%d %d\n", x, -y);
				}
			}
		}
		// we're done
		close(pipe_data[1]);
		// wait for gnuplot to exit
		waitpid(pid, NULL, 0);
	}

	return 0;
}
