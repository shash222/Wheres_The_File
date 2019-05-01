#include <stdio.h>
#include <ctype.h>

#define STR_VALUE(val) #val
#define STR(name) STR_VALUE(name)

#define MD5_LEN 32

int CalcFileMD5(char *file_name, char *md5_sum)
{
#define MD5SUM_CMD_FMT "md5sum %." STR(PATH_LEN) "s 2>/dev/null"
  char cmd[256 + sizeof (MD5SUM_CMD_FMT)];
  sprintf(cmd, MD5SUM_CMD_FMT, file_name);
#undef MD5SUM_CMD_FMT

  FILE *p = popen(cmd, "r");
  if (p == NULL) return 0;

  int i, ch;
  for (i = 0; i < MD5_LEN && isxdigit(ch = fgetc(p)); i++) {
    *md5_sum++ = ch;
  }

  *md5_sum = '\0';
  pclose(p);
  return i == MD5_LEN;
}

int main(int argc, char *argv[])
{
  char md5[MD5_LEN + 1];

  if (!CalcFileMD5("todo.txt", md5)) {
    puts("Error occured!");
  } else {
    printf("Success! MD5 sum is: %s\n", md5);
  }
}
