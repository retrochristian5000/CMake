#include <stdio.h>

int main(int argc, char* argv[])
{
  if (argc < 2) {
    return 1;
  }

  FILE* const out = fopen(argv[1], "w");
  if (!out) {
    return 1;
  }

  fprintf(out, "int gen(void)\n{\n  return 42;\n}\n");
  fclose(out);

  return 0;
}
