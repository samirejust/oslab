#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p2c[2]; // Parent to Child pipe: [0]=read, [1]=write
  int c2p[2]; // Child to Parent pipe: [0]=read, [1]=write
  char byte = 'x';

  if (pipe(p2c) < 0 || pipe(c2p) < 0) {
    fprintf(2, "pingpong: pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    fprintf(2, "pingpong: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // Child process
    close(p2c[1]); // Close unused write end
    close(c2p[0]); // Close unused read end

    if (read(p2c[0], &byte, 1) != 1) {
      fprintf(2, "pingpong: child read failed\n");
      exit(1);
    }
    printf("%d: received ping\n", getpid());

    if (write(c2p[1], &byte, 1) != 1) {
      fprintf(2, "pingpong: child write failed\n");
      exit(1);
    }

    close(p2c[0]);
    close(c2p[1]);
    exit(0);
  } else {
    // Parent process
    close(p2c[0]); // Close unused read end
    close(c2p[1]); // Close unused write end

    if (write(p2c[1], &byte, 1) != 1) {
      fprintf(2, "pingpong: parent write failed\n");
      exit(1);
    }

    if (read(c2p[0], &byte, 1) != 1) {
      fprintf(2, "pingpong: parent read failed\n");
      exit(1);
    }
    printf("%d: received pong\n", getpid());

    close(p2c[1]);
    close(c2p[0]);
    exit(0);
  }
}