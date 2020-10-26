#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
  char buffer[500];
  int i, n;
  for (i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-bad") == 0) {
      fprintf(stdout, "stdout from bad command line arg '-bad'\n");
      fprintf(stderr, "stderr from bad command line arg '-bad'\n");
      return 1;
    }
  }
  FILE* source = fopen(argv[argc - 1], "rb");
  if (source == NULL) {
    return 1;
  }
  FILE* target = fopen(argv[argc - 2], "wb");
  if (target != NULL) {
    n = fread(buffer, 1, sizeof(buffer), source);
    fprintf(stderr, "%s\n", buffer);
    fwrite(buffer, 1, n, target);
    fclose(source);
    fclose(target);
    return 0;
  }
  return 1;
}
