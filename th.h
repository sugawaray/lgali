typedef void *TANY;
typedef unsigned long Tsize_t;

int fprintf(TANY str, const char *format, ...);
int memcmp(const void *s1, const void *s2, Tsize_t n);
void *memset(void *s, int c, Tsize_t n);
TANY fopen(const char *filename, const char *mode);
int fclose(TANY fd);
Tsize_t fread(void *ptr, Tsize_t sz, Tsize_t cnt, TANY fd);
int feof(TANY fd);
