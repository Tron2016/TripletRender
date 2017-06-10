#pragma once
#ifndef RENDERERENGINE_H
#define RENDERERENGINE_H
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/Depth>
#include <osgViewer/Renderer>

#include <opencv2/core/core.hpp>
#include "labeled_pose.h"
#include "RenderEngine.h"
class RendererEngine
{
public:
	RendererEngine(LabeledPose *pose, bool lighting = false);

	void render(int width, int height, cv::Mat &depthMat, cv::Mat &colorMat, cv::Vec4b bgColor = cv::Vec4b(0, 0, 0, 1), bool sphereOrientation = true, double *A = NULL, double *R = NULL,
		double *T = NULL, double *unprojectOutput = NULL, bool writeFiles = false, bool offScreen = true, double *AOutput = NULL, double *ROutput = NULL,
		double *TOutput = NULL, double distanceInc = 1, std::string deg_order = "zxy", bool getUnproject = false);

	cv::Mat ptrDoubleToMat(double *ptr, int width, int height);

	cv::Mat ptrUcharToMat(uchar* ptr, int width, int height, int numChannels);

private:
	LabeledPose *pose;
	bool lighting;


};
#endif 