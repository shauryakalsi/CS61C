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
  //set all to zero initially instead of checking at each loop iteration if zero set is needed
  for (int j = 0; j < imageHeight; ++j){
    for(int i = 0; i < imageWidth; ++i){
      depth[j*imageWidth + i] = 0;
    }
  }
  //if maximumDisplacement = 0, simply return. All zero.
  if(maximumDisplacement == 0){
    return;
  }
  
  //changed bounds from 0, imageHeight and removed conditional since irrelevant pixels are already all zero
  for (int y = featureHeight; y < imageHeight - featureHeight; ++y)
  {
    for (int x = featureWidth; x < imageWidth - featureWidth; ++x)
    {
      //pulled variable definitions out of inner loops 
      float minimumSquaredDifference = -1;
      int minimumDy = 0;
      int minimumDx = 0;
      int lowerLeftY = 0;
      int upperLeftY = 0;
      int lowerRightY = 0;
      int upperRightY = 0;
      int leftX = 0;
      int rightX = 0;

      //could possibly change this to max(-maximumDisplacement, featureHeight - y), min(imageHeight - featureHeight - y - 1) and remove conditional
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
          float result[4];
          float tempResult[4];
          float tempSum;
          float tail_sum = 0;
          __m128 leftLowerVector;
          __m128 rightLowerVector;
          __m128 leftUpperVector;
          __m128 rightUpperVector;
          __m128 difference;
          float diff = 0;
          //halve loop iterations by going from 0, featureHeight instead of -featureHeight, featureHeight
          for (int boxY = 0; boxY <= featureHeight; ++boxY)
          {
            lowerLeftY = y + boxY;
            upperLeftY = y - boxY;
            lowerRightY = y + dy + boxY;
            upperRightY = y + dy - boxY;
            //loop unrolled and SIMD, dounle calculation per iteration since half boxY loop
            for (int boxX = -featureWidth; boxX < -featureWidth + (featureWidth*2)/4*4; boxX = boxX + 4)
            {
              leftX = x + boxX;
              rightX = x + dx + boxX;
              
              leftLowerVector = _mm_loadu_ps(left + lowerLeftY * imageWidth + leftX);//creating for lower side
              rightLowerVector = _mm_loadu_ps(right + lowerRightY * imageWidth + rightX);//reating for lower side
              difference = _mm_sub_ps(leftLowerVector, rightLowerVector);
              main_sum = _mm_add_ps(main_sum, _mm_mul_ps(difference, difference)); //added this line here
              //to prevent double counting of 0 have conditional
              if (boxY != 0) {
                leftUpperVector = _mm_loadu_ps(left + upperLeftY * imageWidth + leftX);
                rightUpperVector = _mm_loadu_ps(right + upperRightY * imageWidth + rightX);
                difference = _mm_sub_ps(leftUpperVector, rightUpperVector);
                main_sum = _mm_add_ps(main_sum, _mm_mul_ps(difference, difference));
              }
              
            }
            
          }
          //check to see if squaredDistance is already greater than minimumSquaredDifference,
          //if so, no need to calculate tail case since this will not be the minimum distance
          _mm_storeu_ps(tempResult, main_sum);
          tempSum = tempResult[0] + tempResult[1] + tempResult[2] + tempResult[3];
          if (tempSum > minimumSquaredDifference && minimumSquaredDifference != -1) {
          	
          	continue;
          }
          //tail case with 1 extra, simply add it onto sum
          if ((2*featureWidth+1) % 4 == 1){
            for(int boxY = 0; boxY <= featureHeight; ++boxY){
              lowerLeftY = y + boxY;
              upperLeftY = y - boxY;
              lowerRightY = y + dy + boxY;
              upperRightY = y + dy - boxY;
              leftX = x - featureWidth + (featureWidth*2)/4*4 ;
              rightX = x + dx - featureWidth + (featureWidth*2)/4*4;
              diff = left[lowerLeftY * imageWidth + leftX] - right[lowerRightY * imageWidth + rightX];
              tail_sum += diff*diff;
              if (boxY != 0) {
                diff = left[upperLeftY * imageWidth + leftX] - right[upperRightY * imageWidth + rightX];
                tail_sum += diff*diff;
              }

            }
          }
          //tail case with 3 extra, vectorize with additional zero and add into sum
          else if (((2*featureWidth+1) % 4 == 3)){
            float tailLeftUpperArray[4];
            float tailRightUpperArray[4];
            float tailLeftLowerArray[4];
            float tailRightLowerArray[4];
            for(int boxY = 0; boxY <= featureHeight; ++boxY){
              lowerLeftY = y + boxY;
              upperLeftY = y - boxY;
              lowerRightY = y + dy + boxY;
              upperRightY = y + dy - boxY;
              leftX = x - featureWidth + (featureWidth*2)/4*4 ;
              rightX = x + dx - featureWidth + (featureWidth*2)/4*4;
             
              tailLeftLowerArray[0] = left[lowerLeftY * imageWidth + leftX];
              tailLeftLowerArray[1] = left[lowerLeftY * imageWidth + leftX + 1];
              tailLeftLowerArray[2] = left[lowerLeftY * imageWidth + leftX + 2];
              tailLeftLowerArray[3] = 0;
             
              tailRightLowerArray[0] = right[lowerRightY * imageWidth + rightX];
              tailRightLowerArray[1] = right[lowerRightY * imageWidth + rightX + 1];
              tailRightLowerArray[2] = right[lowerRightY * imageWidth + rightX + 2];
              tailRightLowerArray[3] = 0;
              
              tailLeftUpperArray[0] = left[upperLeftY * imageWidth + leftX];
              tailLeftUpperArray[1] = left[upperLeftY * imageWidth + leftX + 1];
              tailLeftUpperArray[2] = left[upperLeftY * imageWidth + leftX + 2];
              tailLeftUpperArray[3] = 0;
              
              tailRightUpperArray[0] = right[upperRightY * imageWidth + rightX];
              tailRightUpperArray[1] = right[upperRightY * imageWidth + rightX + 1];
              tailRightUpperArray[2] = right[upperRightY * imageWidth + rightX + 2];
              tailRightUpperArray[3] = 0;


              leftLowerVector = _mm_loadu_ps(tailLeftLowerArray);//creating for lower side
              rightLowerVector = _mm_loadu_ps(tailRightLowerArray);//reating for lower side
              difference = _mm_sub_ps(leftLowerVector, rightLowerVector);
              main_sum = _mm_add_ps(main_sum, _mm_mul_ps(difference, difference)); //added this line here

              if(boxY != 0){
                leftUpperVector = _mm_loadu_ps(tailLeftUpperArray);
                rightUpperVector = _mm_loadu_ps(tailRightUpperArray);
                difference = _mm_sub_ps(leftUpperVector, rightUpperVector);
                main_sum = _mm_add_ps(main_sum, _mm_mul_ps(difference, difference));
              }

            }

          }
          //computes final squaredDifference
          _mm_storeu_ps(result, main_sum);
          
          squaredDifference += result[0] + result[1] + result[2] + result[3] + tail_sum;
          //removed call to displacement naive and shifted calculation of dx*dx + dy*dy and minimumDx * minimumDx + minimumDy * minimumDy to the end so short circuit faster
          if ((minimumSquaredDifference == -1) || ((minimumSquaredDifference > squaredDifference) && minimumSquaredDifference != squaredDifference) || (minimumSquaredDifference == squaredDifference && (dx*dx + dy*dy) < minimumDx*minimumDx + minimumDy * minimumDy))
          {
            minimumSquaredDifference = squaredDifference;
            minimumDx = dx;
            minimumDy = dy;
          }
        }
      }
      //sets value
      if (minimumSquaredDifference != -1)
      {
          
          depth[y * imageWidth + x] = sqrt(minimumDx*minimumDx + minimumDy*minimumDy);
        
      }
      
    }
  }
  
}


