#ifndef _DRIVER_H_
#define _DRIVER_H_

#include <dirent.h>

int filter (const struct dirent *ent);
int rect_mask (int x, int y, int w, int h, int dir);
int ellipse_mask (int x, int y, int w, int h, int dir);
int diamond_mask (int x, int y, int w, int h, int dir);
int thresh(int mean);

#endif //_DRIVER_H_
