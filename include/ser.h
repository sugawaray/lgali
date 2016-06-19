int serinit();
int serout(const char *m);
int serrcv(char *b, int sz);
int seroutf(const char *fmt, ...);

u32 serdbgaddr();
