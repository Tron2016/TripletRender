#pragma once
#ifndef RENDERERENGINE_H
#define RENDERERENGINE_H
#include <windows.h>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/Depth>
#include <osgViewer/Renderer>
#include<osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include "labeled_pose.h"
class RendererEngine
{
public:
	RendererEngine(LabeledPose *pose, bool lighting = false);

	void render(int width, int height, cv::Mat &depthMat, cv::Mat &colorMat, osg::Vec4 bgColor = osg::Vec4(0, 1, 0, 1), bool sphereOrientation = true, double *A = NULL, double *R = NULL,
		double *T = NULL, double *unprojectOutput = NULL, bool writeFiles = false, bool offScreen = true, double *AOutput = NULL, double *ROutput = NULL,
		double *TOutput = NULL, double distanceInc = 1, std::string deg_order = "zxy", bool getUnproject = false);

	cv::Mat ptrDoubleToMat(double *ptr, int width, int height);

	cv::Mat ptrUcharToMat(uchar* ptr, int width, int height, int numChannels);

private:
	osg::ref_ptr<osgViewer::CompositeViewer> viewer;
	osg::ref_ptr<osgViewer::View> view;
	osg::ref_ptr<osg::Camera> camera;
	osg::Group* sceneRoot;
	LabeledPose *pose;
	bool lighting;


};
#endif 