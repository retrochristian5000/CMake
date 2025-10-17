#ifndef _CRT_SECURE_NO_WARNINGS
#  define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
  FILE* fin = fopen(argv[argc - 1], "r");
  FILE* fout;
  int i;
  int ch;
  char* outFile = NULL;
  for (i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-o") == 0) {
      outFile = argv[i + 1];
    }
  }
  if (outFile == NULL) {
    printf("No output file.\n");
    return 1;
  }
  fout = fopen(outFile, "w");
  if (fin == NULL || fout == NULL) {
    printf("Error: Could not open file.\n");
    return 1;
  }
  while ((ch = fgetc(fin)) != EOF) {
    fputc(ch, fout);
  }
  fprintf(stdout, "Total Messages: 1\n");
  fclose(fin);
  fclose(fout);
  return 0;
}
