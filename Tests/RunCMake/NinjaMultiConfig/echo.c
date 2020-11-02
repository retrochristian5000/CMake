#include <stdio.h>

#include <unistd.h>

int main(int argc, char** argv)
{
  int i;
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("'%s'$", cwd);
  }
  for (i = 0; i < argc; ++i) {
    printf(" '%s'", argv[i]);
  }
  printf("\n");
  return 0;
}
