#include <unistd.h>
#include <stdio.h>
#include <linux/limits.h>

int main(void)
{
   char cwd[PATH_MAX];
   if (getcwd(cwd, sizeof(cwd)) != NULL) {
       printf("%s", cwd);
   } else {
       perror("getcwd() error");
       return 1;
   }
   return 0;
}
