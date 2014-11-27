// CS 61C Fall 2014 Project 3

// include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif
#include <stdbool.h>
#include "utils.h"
#include <stdio.h>
#include "calcDepthOptimized.h"
#include "calcDepthNaive.h"

#define ABS(x) (((x) < 0) ? (-(x)) : (x))

void calcDepthOptimized(float *depth, float *left, float *right, int imageWidth, int imageHeight, int featureWidth, int featureHeight, int maximumDisplacement)
{
	//calcDepthNaive(depth, left, right, imageWidth, imageHeight, featureWidth, featureHeight, maximumDisplacement, NULL);
	// if (floatOps != NULL)
	// {
	// 	*floatOps = 0;
	// }

	for (int y = 0; y < imageHeight; y++)
	{
		for (int x = 0; x < imageWidth; x++)
		{
			if ((y < featureHeight) || (y >= imageHeight - featureHeight) || (x < featureWidth) || (x >= imageWidth - featureWidth))
			{
				depth[y * imageWidth + x] = 0;
				continue;
			}

			float minimumSquaredDifference = -1;
			int minimumDy = 0;
			int minimumDx = 0;

			for (int dy = -maximumDisplacement; dy <= maximumDisplacement; dy++)
			{
				for (int dx = -maximumDisplacement; dx <= maximumDisplacement; dx++)
				{
					if (y + dy - featureHeight < 0 || y + dy + featureHeight >= imageHeight || x + dx - featureWidth < 0 || x + dx + featureWidth >= imageWidth)
					{
						continue;
					}

					float squaredDifference = 0;
					__m128 main_sum = _mm_setzero_ps();
					float tail_sum = 0;
					float result[4];
					for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
					{
						//float minFound = 0.0;
						int leftY = y + boxY;
						int rightY = y + dy + boxY;
						int leftX = 0;
						int rightX = 0;
						
						for (int boxX = -featureWidth; boxX < -featureWidth + (featureWidth*2)/4*4; boxX+=4)//less than?
						{
							// int leftX[4] = {x + boxX, x + boxX +1, x + boxX + 2, x + boxX + 3 };
							// __m128i leftXSum = _mm_loadu_si128( __m128i *leftX );

							// int leftY[4] = {y + boxY, y + boxY+1, y + boxY+2, y + boxY+3};
							// __m128i leftYSum = _mm_loadu_si128( __m128i *leftY );

							leftX = x + boxX;
							rightX = x + dx + boxX;
							
							__m128 leftVector = _mm_loadu_ps(left + leftY * imageWidth + leftX);
							__m128 rightVector = _mm_loadu_ps(right + rightY * imageWidth + rightX);
							__m128 difference = _mm_sub_ps (leftVector, rightVector);
							difference = _mm_mul_ps(difference, difference);
							main_sum = _mm_add_ps(main_sum, difference);

							//float result[4];
							//_mm_storeu_ps((__m128 *)result, difference); //not sure
							// minFound = result[0];
							// if (result[1] < minFound) {
							// 	minFound = result[1];
							// }
							// if (result[2] < minFound) {
							// 	minFound = result[2];
							// }
							// if (result[3] < minFound) {
							// 	minFound = result[3];
							// }
							// float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
							// difference = _mm_min_ps(difference, _mm_shuffle_ps(difference, difference, _MM_SHUFFLE(2, 1, 0, 3)));
    			// 			difference = _mm_min_ps(difference, _mm_shuffle_ps(difference, difference, _MM_SHUFFLE(1, 0, 3, 2)));
    			// 			float result = _mm_extract_ps(difference, 0);
							

							// squaredDifference += minFound;

							// if (floatOps != NULL)
							// {
							// 	*floatOps += 3;
							// }
						}
						for(int boxX = -featureWidth + (featureWidth*2)/4*4; boxX <= featureWidth; boxX ++){
							leftX = x + boxX;
							
							rightX = x + dx + boxX;
							

							float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
							difference = difference*difference;
							// if(difference < minFound){
							// 	minFound = difference;
							// }
							tail_sum += difference;
						}
						// _mm_storeu_ps((__m128 *)result, difference);
						// //printf("minfound %f\n",minFound);
						// squaredDifference = minFound;
					}
					
					_mm_storeu_ps(result, main_sum);
					// for (int i = 0; i < 4; i++) {
					// 	printf(" %d %f\n",i,result[i]);
					// }
					//printf("tail %f\n", tail_sum);
					//printf("result_0 %f, result_1 %f, result_2 %f, result_3 %f\n",result[0],result[1],result[2],result[3]);
					squaredDifference += result[0] + result[1] + result[2] + result[3] + tail_sum;

					if ((minimumSquaredDifference == -1) || (minimumSquaredDifference == squaredDifference && (displacementNaive(dx, dy) < displacementNaive(minimumDx, minimumDy))) || ((minimumSquaredDifference > squaredDifference) && minimumSquaredDifference != squaredDifference))
					{
						minimumSquaredDifference = squaredDifference;
						minimumDx = dx;
						minimumDy = dy;
					}
				}
			}

			if (minimumSquaredDifference != -1)
			{
				if (maximumDisplacement == 0)
				{
					depth[y * imageWidth + x] = 0;
				}
				else
				{
					depth[y * imageWidth + x] = displacementNaive(minimumDx, minimumDy);
				}
			}
			else
			{
				depth[y * imageWidth + x] = 0;
			}
		}
	}
}
