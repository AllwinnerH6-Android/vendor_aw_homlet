
#ifndef DISPLAYD_UTILS_H_
#define DISPLAYD_UTILS_H_


#undef  NELEM
#define NELEM(name) (sizeof(name) / sizeof(name[0]))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int getConnectStateFromFile(const char *path);
int readIntFromFile(const char *path);
int writeIntToFile(const char *path, int t);

#include "sunxi_typedef.h"
const char *toString(output_layer_mode_t mode);

#endif
