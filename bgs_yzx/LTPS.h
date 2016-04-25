#ifndef _LTP_H
#define _LTP_H


#include <math.h>

/* Compare a value pointed to by 'ptr' to the 'center' value and
* increment pointer. Comparison is made by masking the most
* significant bit of an integer (the sign) and shifting it to an
* appropriate position. */
#define compab_mask_inc(ptr,shift) { value |= ((unsigned int)(*center - *ptr - 1) & 0x80000000) >> (31-shift); ptr++; }
/* Compare a value 'val' to the 'center' value. */
#define compab_mask(val,shift) { value |= ((unsigned int)(*center - (val) - 1) & 0x80000000) >> (31-shift); }
/* Predicate 1 for the 3x3 neighborhood */
#define predicate 1
/* The number of bits */
#define bits 8

typedef struct
{
	int x, y;
} integerpoint;

typedef struct
{
	double x, y;
} doublepoint;

void calculate_points(void);
inline int getLTP(int a, int b);
inline double interpolate_at_ptr(int* upperLeft, int i, int columns);
inline double interpolate_at_ptr(uchar* upperLeft, int i, int columns);
int* lbp_histogram(int* img, int rows, int columns, int* result, int interpolated);
void ltps(cv::Mat& source, int rows, int columns, int interpolated, cv::Mat& res);

#endif