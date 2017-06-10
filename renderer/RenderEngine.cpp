#include "RenderEngine.h"
#include "renderer.h"
RendererEngine::RendererEngine(LabeledPose *pose, bool lighting = false) {
}

void RendererEngine::render(int width, int height, cv::Mat &depthMat, cv::Mat &colorMat, cv::Vec4b bgColor = cv::Vec4b(0, 0, 0, 1), bool sphereOrientation = true, double *A = NULL, double *R = NULL,
	double *T = NULL, double *unprojectOutput = NULL, bool writeFiles = false, bool offScreen = true, double *AOutput = NULL, double *ROutput = NULL,
	double *TOutput = NULL, double distanceInc = 1, std::string deg_order = "zxy", bool getUnproject = false) {

}

cv::Mat RendererEngine::ptrDoubleToMat(double *ptr, int width, int height) {

}

cv::Mat RendererEngine::ptrUcharToMat(uchar* ptr, int width, int height, int numChannels) {

}