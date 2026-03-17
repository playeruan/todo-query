#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

const char * restrict usagefmt = "usage: %s <file | dir> [-s<separator>]\n";
const char *separator = "|";

void tqfile(FILE *f, const char *filename) {
  char c;
  uint64_t line = 1;
  bool in_string = false;
  while ((c = getc(f)) != EOF) {
    if (c == 'T' &&
        getc(f) == 'O' &&
        getc(f) == 'D' &&
        getc(f) == 'O' ) {
      printf("%s:%zu %s ", filename, line, separator);
      if ((c = getc(f)) == ':') c = getc(f);
      while (isspace(c)) c = getc(f);
      do {
        putchar(c); 
        if (in_string && c == '\n') {
          putchar('\t');
        }
      } while (
          (c = getc(f)) != EOF &&
          (in_string || c != '\n') &&
          (!in_string || c != '"')
      );
      putchar('\n');
    }
    if (c == '"') in_string = !in_string;
    if (c == '\n') line++;
  }
}

void tqdir(DIR *dr, const char *dirpath) {
  FILE *f = NULL;
  struct dirent *de;
  while ((de = readdir(dr)) != NULL) {
    if (strcmp(de->d_name, ".") == 0 ||
        strcmp(de->d_name, "..") == 0 ||
        de->d_name[0] == '.') {
      continue;
    }

    char fullname[(1<<9)];
    sprintf(fullname, "%s/%s", dirpath, de->d_name);
    if (de->d_type == DT_DIR) {
      DIR *subdr = opendir(fullname);
      if (NULL == subdr) {
        fprintf(stderr, "Error opening sub-directory \"%s\": %d\n",
            fullname,
            errno
        );
      } else {
        tqdir(subdr, fullname);
      }
    } else if (de->d_type == DT_REG) {
      f = fopen(fullname, "r");
      if (NULL == f) {
        fprintf(stderr, "Error opening file \"%s\": %d\n",
            fullname,
            errno
        );
      } else {
        tqfile(f, fullname);
        fclose(f);
      }
    }
  }
  closedir(dr);
}

int main(int argc, char *argv[]) {
  
  //TODO: Hello, TODO!
  
  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    printf(usagefmt, argv[0]);
    return 1;
  }

  for (int i = 2; i < argc; i++) {
    // check for flags
    if (strncmp(argv[i], "-s", 1) == 0) {
      separator = argv[i] + 2;
    }
  }

  struct stat s;

  if( stat(argv[1], &s) == 0 )
  {
    if( s.st_mode & S_IFDIR )
    {
      DIR *dr = opendir(argv[1]);
      if (NULL == dr) {
        fprintf(stderr, "Error opening directory \"%s\"\n", argv[1]);
        return 1;
      }
      char dirpath[1<<8];
      snprintf(dirpath, sizeof(dirpath), "%s", argv[1]);
      tqdir(dr, dirpath);
    }
    else if( s.st_mode & S_IFREG )
    {
      FILE *f = fopen(argv[1], "r");
      if (NULL == f) {
        fprintf(stderr, "Error opening file \"%s\"\n", argv[1]);
        return 1;
      }
      tqfile(f, argv[1]);
      fclose(f);
    }
    else
    {
      fprintf(stderr, "Only files and directories are supported!\n");
      return 1;
    }
  }
  else
  {
    fprintf(stderr, "Error opening \"%s\"\n", argv[1]);
    return 1;
  }

  return 0;

}
