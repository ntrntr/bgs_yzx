#include "stdafx.h"
#include "LTPS.h"
static const int T = 3;
integerpoint points[bits];
doublepoint offsets[bits];
#define M_PI (3.14159265358979323846) 
/*
* Get a bilinearly interpolated value for a pixel.
*/
inline double interpolate_at_ptr(int* upperLeft, int i, int columns)
{
	double dx = 1 - offsets[i].x;
	double dy = 1 - offsets[i].y;
	return
		*upperLeft*dx*dy +
		*(upperLeft + 1)*offsets[i].x*dy +
		*(upperLeft + columns)*dx*offsets[i].y +
		*(upperLeft + columns + 1)*offsets[i].x*offsets[i].y;
}
inline double interpolate_at_ptr(uchar* upperLeft, int i, int columns)
{
	double dx = 1 - offsets[i].x;
	double dy = 1 - offsets[i].y;
	return
		*upperLeft*dx*dy +
		*(upperLeft + 1)*offsets[i].x*dy +
		*(upperLeft + columns)*dx*offsets[i].y +
		*(upperLeft + columns + 1)*offsets[i].x*offsets[i].y;
}
/*
* Calculate the point coordinates for circular sampling of the neighborhood.
*/
void calculate_points(void)
{
	double step = 2 * M_PI / bits, tmpX, tmpY;
	int i;
	for (i = 0; i<bits; i++)
	{
		tmpX = predicate * cos(i * step);
		tmpY = predicate * sin(i * step);
		points[i].x = (int)tmpX;
		points[i].y = (int)tmpY;
		offsets[i].x = tmpX - points[i].x;
		offsets[i].y = tmpY - points[i].y;
		if (offsets[i].x < 1.0e-10 && offsets[i].x > -1.0e-10) /* rounding error */
			offsets[i].x = 0;
		if (offsets[i].y < 1.0e-10 && offsets[i].y > -1.0e-10) /* rounding error */
			offsets[i].y = 0;

		if (tmpX < 0 && offsets[i].x != 0)
		{
			points[i].x -= 1;
			offsets[i].x += 1;
		}
		if (tmpY < 0 && offsets[i].y != 0)
		{
			points[i].y -= 1;
			offsets[i].y += 1;
		}
	}
}

/*
* Calculate the LBP histogram for an integer-valued image. This is an
* optimized version of the basic 8-bit LBP operator. Note that this
* assumes 4-byte integers. In some architectures, one must modify the
* code to reflect a different integer size.
*
* img: the image data, an array of rows*columns integers arranged in
* a horizontal raster-scan order
* rows: the number of rows in the image
* columns: the number of columns in the image
* result: an array of 256 integers. Will hold the 256-bin LBP histogram.
* interpolated: if != 0, a circular sampling of the neighborhood is
* performed. Each pixel value not matching the discrete image grid
* exactly is obtained using a bilinear interpolation. You must call
* calculate_points (only once) prior to using the interpolated version.
* return value: result
*/
int* lbp_histogram(int* img, int rows, int columns, int* result, int interpolated)
{
	int leap = columns*predicate;
	/*Set up a circularly indexed neighborhood using nine pointers.*/
	int
		*p0 = img,
		*p1 = p0 + predicate,
		*p2 = p1 + predicate,
		*p3 = p2 + leap,
		*p4 = p3 + leap,
		*p5 = p4 - predicate,
		*p6 = p5 - predicate,
		*p7 = p6 - leap,
		*center = p7 + predicate;
	//0 1 2
	//7 c 3
	//6 5 4
	unsigned int value;
	int pred2 = predicate << 1;
	int r, c;

	memset(result, 0, sizeof(int) * 256); /* Clear result histogram */

	if (!interpolated)
	{
		for (r = 0; r<rows - pred2; r++)
		{
			for (c = 0; c<columns - pred2; c++)
			{
				value = 0;

				/* Unrolled loop */
				compab_mask_inc(p0, 0);
				compab_mask_inc(p1, 1);
				compab_mask_inc(p2, 2);
				compab_mask_inc(p3, 3);
				compab_mask_inc(p4, 4);
				compab_mask_inc(p5, 5);
				compab_mask_inc(p6, 6);
				compab_mask_inc(p7, 7);
				center++;

				result[value]++; /* Increase histogram bin value */
			}
			p0 += pred2;
			p1 += pred2;
			p2 += pred2;
			p3 += pred2;
			p4 += pred2;
			p5 += pred2;
			p6 += pred2;
			p7 += pred2;
			center += pred2;
		}
	}
	else
	{
		//points[5].y < 0  points[7].y < 0
		p0 = center + points[5].x + points[5].y * columns;
		p2 = center + points[7].x + points[7].y * columns;
		//points[4].y > 0  points[6].y > 0
		p4 = center + points[1].x + points[1].y * columns;
		p6 = center + points[3].x + points[3].y * columns;

		for (r = 0; r<rows - pred2; r++)
		{
			for (c = 0; c<columns - pred2; c++)
			{
				value = 0;

				/* Unrolled loop */
				compab_mask_inc(p1, 1);
				compab_mask_inc(p3, 3);
				compab_mask_inc(p5, 5);
				compab_mask_inc(p7, 7);

				/* Interpolate corner pixels */
				compab_mask((int)(interpolate_at_ptr(p0, 5, columns) + 0.5), 0);
				compab_mask((int)(interpolate_at_ptr(p2, 7, columns) + 0.5), 2);
				compab_mask((int)(interpolate_at_ptr(p4, 1, columns) + 0.5), 4);
				compab_mask((int)(interpolate_at_ptr(p6, 3, columns) + 0.5), 6);
				p0++;
				p2++;
				p4++;
				p6++;
				center++;

				result[value]++;
			}
			p0 += pred2;
			p1 += pred2;
			p2 += pred2;
			p3 += pred2;
			p4 += pred2;
			p5 += pred2;
			p6 += pred2;
			p7 += pred2;
			center += pred2;
		}
	}
	return 0;
}

inline int getLTP(int a, int b)
{
	if (a-b > T)
	{
		return 1;
	}
	else if (a - b < -T)
	{
		return -1;
	}
	else
		return 0;
}
void ltps(cv::Mat& source, int rows, int columns, int interpolated, cv::Mat& res)
{

	if (!source.isContinuous())
	{
		cout << "error in " << __FILE__ << " line = " << __LINE__ << endl;
		return;
	}
	int leap = columns*predicate;
	/*Set up a circularly indexed neighborhood using nine pointers.*/
	uchar
		*p0 = source.data,
		*p1 = p0 + predicate,
		*p2 = p1 + predicate,
		*p3 = p2 + leap,
		*p4 = p3 + leap,
		*p5 = p4 - predicate,
		*p6 = p5 - predicate,
		*p7 = p6 - leap,
		*center = p7 + predicate;
	//0 1 2
	//7 c 3
	//6 5 4
	unsigned int value;
	int pred2 = predicate << 1;
	int r, c;
	int x, y, k;
	float* ptr;
	if (!interpolated)
	{
		//for (r = 0; r < rows - pred2; r++)
		//{
		//	for (c = 0; c < columns - pred2; c++)
		//	{
		//		value = 0;

		//		/* Unrolled loop */
		//		compab_mask_inc(p0, 0);
		//		compab_mask_inc(p1, 1);
		//		compab_mask_inc(p2, 2);
		//		compab_mask_inc(p3, 3);
		//		compab_mask_inc(p4, 4);
		//		compab_mask_inc(p5, 5);
		//		compab_mask_inc(p6, 6);
		//		compab_mask_inc(p7, 7);
		//		center++;

		//		result[value]++; /* Increase histogram bin value */
		//	}
		//	p0 += pred2;
		//	p1 += pred2;
		//	p2 += pred2;
		//	p3 += pred2;
		//	p4 += pred2;
		//	p5 += pred2;
		//	p6 += pred2;
		//	p7 += pred2;
		//	center += pred2;
		//}
	}
	else
	{
		//points[5].y < 0  points[7].y < 0
		p0 = center + points[5].x + points[5].y * columns;
		p2 = center + points[7].x + points[7].y * columns;
		//points[4].y > 0  points[6].y > 0
		p4 = center + points[1].x + points[1].y * columns;
		p6 = center + points[3].x + points[3].y * columns;

		for (r = 0; r < rows - pred2; r++)
		{
			ptr = res.ptr<float>(r + 1);
			ptr++;
			for (c = 0; c < columns - pred2; c++)
			{
				//value = 0;

				///* Unrolled loop */
				//compab_mask_inc(p1, 1);
				//compab_mask_inc(p3, 3);
				//compab_mask_inc(p5, 5);
				//compab_mask_inc(p7, 7);

				///* Interpolate corner pixels */
				//compab_mask((int)(interpolate_at_ptr(p0, 5, columns) + 0.5), 0);
				//compab_mask((int)(interpolate_at_ptr(p2, 7, columns) + 0.5), 2);
				//compab_mask((int)(interpolate_at_ptr(p4, 1, columns) + 0.5), 4);
				//compab_mask((int)(interpolate_at_ptr(p6, 3, columns) + 0.5), 6);
				x = 0;
				y = 0;
				//0 1 2
				//7 c 3
				//6 5 4

				//0
				k = getLTP((int)(*center), (int)(interpolate_at_ptr(p0, 5, columns) + 0.5));
				x += k * -1;
				y += k * 1;
				//2
				k = getLTP((int)(*center), (int)(interpolate_at_ptr(p2, 7, columns) + 0.5));
				x += k * 1;
				y += k * 1;
				//4
				k = getLTP((int)(*center), (int)(interpolate_at_ptr(p4, 1, columns) + 0.5));
				x += k * 1;
				y += k * -1;
				//6
				k = getLTP((int)(*center), (int)(interpolate_at_ptr(p6, 3, columns) + 0.5));
				x += k * -1;
				y += k * -1;
				//1
				k = getLTP((int)(*center), (int)(*p1));
				x += k * 0;
				y += k * 1;
				//3
				k = getLTP((int)(*center), (int)(*p3));
				x += k * 1;
				y += k * 0;
				//5
				k = getLTP((int)(*center), (int)(*p5));
				x += k * 0;
				y += k * -1;
				//7
				k = getLTP((int)(*center), (int)(*p7));
				x += k * -1;
				y += k * 0;
				p0++;
				p1++;
				p2++;
				p3++;
				p4++;
				p5++;
				p6++;
				p7++;
				center++;

				ptr[0] = x;
				ptr[1] = y;
				//result[value]++;
				ptr += 2;
			}
			p0 += pred2;
			p1 += pred2;
			p2 += pred2;
			p3 += pred2;
			p4 += pred2;
			p5 += pred2;
			p6 += pred2;
			p7 += pred2;
			center += pred2;
		}
	}
	GaussianBlur(res, res, Size(3, 3), 0);
}