/*  Copyright 2011 AIT Austrian Institute of Technology
*
*   This file is part of OpenTLD.
*
*   OpenTLD is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   OpenTLD is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with OpenTLD.  If not, see <http://www.gnu.org/licenses/>.
*
*/

/*
 * VarianceFilter.cpp
 *
 *  Created on: Nov 16, 2011
 *      Author: Georg Nebehay
 */
#include <iostream>
#include "VarianceFilter.h"

#include "IntegralImage.h"
#include "DetectorCascade.h"

using namespace cv;
using std::cout;
using std::endl;
namespace tld
{

VarianceFilter::VarianceFilter()
{
    enabled = true;
    minVar = 0;
//    integralImg = NULL;
 //   integralImg_squared = NULL;
 	isFirstTime = true;
	this->ii1 = NULL;
	this->ii2 = NULL;
}

VarianceFilter::~VarianceFilter()
{
    release();
}

void VarianceFilter::release()
{
/*    if(integralImg != NULL) delete integralImg;

    integralImg = NULL;

    if(integralImg_squared != NULL) delete integralImg_squared;

    integralImg_squared = NULL;
*/

	if (this->ii1 != NULL)
		delete this->ii1;
	this->ii1 = NULL;

	if (this->ii2 != NULL)
		delete this->ii2;
	this->ii2 = NULL;
}

float VarianceFilter::calcVariance(int *off)
{

   // int *ii1 = integralImg->data;
   // long long *ii2 = integralImg_squared->data;

    float mX  = (ii1[off[3]] - ii1[off[2]] - ii1[off[1]] + ii1[off[0]]) / (float) off[5]; //Sum of Area divided by area
    float mX2 = (ii2[off[3]] - ii2[off[2]] - ii2[off[1]] + ii2[off[0]]) / (float) off[5];
    return mX2 - mX * mX;
}

void VarianceFilter::nextIteration(const Mat &img)
{
    if(!enabled) return;

   // release();

   // integralImg = new IntegralImage<int>(img.size());
   // integralImg->calcIntImg(img);

    //integralImg_squared = new IntegralImage<long long>(img.size());
    //integralImg_squared->calcIntImg(img, true);

	if (isFirstTime){
		//第一次调用时 分配空间 最后释放空间 而不是每次都新分配和释放空间
		this->ii1 = new unsigned[img.rows * img.cols];
		this->ii2 = new unsigned long long[img.rows * img.cols];

		isFirstTime = false;
	}
    calcIntegralImg(img);
}

bool VarianceFilter::filter(int i)
{
    if(!enabled) return true;

    float bboxvar = calcVariance(windowOffsets + TLD_WINDOW_OFFSET_SIZE * i);

    detectionResult->variances[i] = bboxvar;

    if(bboxvar < minVar)
    {
        return false;
    }

    return true;
}

void VarianceFilter::calcIntegralImg(const Mat &img){
	if (img.type() != CV_8UC1){
		cout << "img type is not CV_8UC1." << endl;
		return;
	}
	
	unsigned * preLine1 = ii1;
	unsigned long long *preLine2 = ii2;
	const unsigned char * imgRow = img.ptr<unsigned char>(0);
	int width = img.cols;
	int height = img.rows;	
	int value = 0;
	int sum1 = 0;
	int sum2 = 0;
	unsigned* p1 = ii1;
	unsigned long long* p2 = ii2;

	//计算第0行的第0个元素
	value = imgRow[0];
	*p1 = value;
	*p2 = value * value;
	p1++;
	p2++;

	//计算第0行的其他元素
	for (int j = 1; j < width; j++){
		value = imgRow[j];
		*p1 = value + *(p1 -1);
		*p2 = value * value + *(p2 - 1);
		
		p1++;
		p2++;
	}
	
	//计算其他行
	for (int i = 1; i < height; i++){
		sum1 = 0;
		sum2 = 0;
		imgRow = img.ptr<unsigned char>(i);
		for (int j = 0; j < width; j++){
			value = imgRow[j];
			sum1 += value;
			*p1 = sum1 + *preLine1;
			p1++;
			preLine1++;

			sum2 += value * value;
 			*p2 = sum2 + *preLine2;
			p2++;
			preLine2++;
		}
	}
 	
}
} /* namespace tld */








