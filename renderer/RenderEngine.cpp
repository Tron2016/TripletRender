#include "RenderEngine.h"
#include <OpenThreads/Mutex>
#include <osgViewer/ViewerEventHandlers>
#include <osg/MatrixTransform>
#include <osgGA/MultiTouchTrackballManipulator>
#include <osgGA/StateSetManipulator> 


RendererEngine::RendererEngine(LabeledPose *pose, bool lighting) {
	using namespace osg;
	this->pose = pose;
	this->lighting = lighting;

	viewer = new osgViewer::CompositeViewer;
	viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
	viewer->setKeyEventSetsDone(0);

	view = new osgViewer::View;
	viewer->addView(view);

	camera = view->getCamera();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x = 0;
	traits->y = 0;
	traits->width = 300;
	traits->height = 300;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	//it decide whether off screen rendering
	traits->pbuffer = true;

	osg::GraphicsContext* _gc = osg::GraphicsContext::createGraphicsContext(traits.get());
	camera->setGraphicsContext(_gc);

	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setRenderOrder(osg::Camera::POST_RENDER);
	//camera->setRenderTargetImplementation(Camera::FRAME_BUFFER_OBJECT);

	camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera->setClearDepth(0.0);

	sceneRoot = new Group;
	StateSet* sceneSS = sceneRoot->getOrCreateStateSet();
	osg::Depth* depth = new osg::Depth(osg::Depth::GREATER, 1.0, 0.0);
	sceneSS->setAttributeAndModes(depth, (StateAttribute::ON || StateAttribute::OVERRIDE));

	//view->addEventHandler(new osgGA::StateSetManipulator(view->getCamera()->getOrCreateStateSet()));
	//view->addEventHandler(new osgViewer::StatsHandler);
	////viewer->setCameraManipulator(new osgGA::MultiTouchTrackballManipulator);
	//view->addEventHandler(new osgViewer::WindowSizeHandler);
	//it's very important to generate image
	//view->setCameraManipulator(new osgGA::MultiTouchTrackballManipulator);
	//view->setCameraManipulator(NULL);
	sceneRoot->addChild(osgDB::readNodeFile(this->pose->getFilename(), new osgDB::Options(std::string("noTriStripPolygons"))));
	osg::ref_ptr<MatrixTransform> transform = new MatrixTransform;
	osg::Vec3f objCenter = sceneRoot->getBound().center();
	float objRaidus=sceneRoot->getBound().radius();
	//osg::Matrix origin = sceneRoot->getWorldMatrices();
	transform->setMatrix(Matrix::translate(-objCenter)*Matrix::scale(1.0/objRaidus,1.0/objRaidus,1.0/objRaidus));
	transform->addChild(sceneRoot);
	
	//tranform->setMatrix(Matrix::rotate(1,1,1)*)
	//view->setSceneData(sceneRoot);
	view->setSceneData(transform);
}

void RendererEngine::render(int width, int height, cv::Mat &depthMat, cv::Mat &colorMat, osg::Vec4 bgColor, bool sphereOrientation, double *A, double *R ,
	double *T, double *unprojectOutput , bool writeFiles , bool offScreen , double *AOutput , double *ROutput ,
	double *TOutput , double distanceInc , std::string deg_order , bool getUnproject) {
	using namespace osg;
	using namespace cv;
	//center is the position of reference point usually is the center of object , eye is the position of camera,up is the direction of camera
	osg::Vec3f eye, center, up;
	float m_distance = 3;
	float eyex=
	eye = osg::Vec3f(0, -3,0);
	center = osg::Vec3f(0, 0, 0);
	up = osg::Vec3f(0, 0, 1);
	camera->setViewMatrixAsLookAt(eye, center, up);
	//camera->setViewMatrix(osg::Matrix::identity());
	camera->setViewport(new Viewport(0, 0, width, height));
	camera->setClearColor(osg::Vec4(0,0, 0, 1));
	camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(width) / static_cast<double>(height), 1.0f, 10000.0f);

	osg::ref_ptr<osg::Image> colorImage = new osg::Image;
	osg::ref_ptr<osg::Image> depthImage = new osg::Image;

	colorImage->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
	depthImage->allocateImage(width, height, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE);

	camera->attach(osg::Camera::COLOR_BUFFER, colorImage.get());
	camera->attach(osg::Camera::DEPTH_BUFFER, depthImage.get());
	uchar *ptr1 = new uchar[width*height*3]();
	uchar *ptr2 = new uchar[width*height]();
	//camera->setFinalDrawCallback(new ImageCaptureCallback(colorImage, depthImage,ptr1,ptr2, "b"));
	
	
	viewer->frame();
	//osg::Image* cc =colorImage.get();
	//ptr1 = cc->data();
	//osgDB::writeImageFile(*colorImage.get(), "color.bmp");
	//osgDB::writeImageFile(*depthImage.get(), "depth.bmp");
	//Mat crMat(height, width, CV_8UC3);
	int b=colorImage->isDataContiguous();
	//getRenderedImage(colorImage->data(), width, height,crMat.data);
	Mat crMat(height, width, CV_8UC3, colorImage->data());
	Mat dpMat(height, width, CV_8UC1, depthImage->data());
	cvtColor(crMat, crMat, CV_RGB2BGR);
	colorMat = crMat;
	depthMat = dpMat;
	imshow("a", crMat);
	cv::waitKey(10000);
	//imwrite("dp.jpg", dpMat);
	//viewer->realize();
	//viewer->renderingTraversals();
	
	//cv::flip(crMat, colorMat, 0);
	//cv::flip(dpMat, depthMat, 0);
	
	//viewer->run();

}

cv::Mat RendererEngine::ptrDoubleToMat(double *ptr, int width, int height) {
	cv::Mat result;
	return result;
}

cv::Mat RendererEngine::ptrUcharToMat(uchar* ptr, int width, int height, int numChannels) {
	cv::Mat result;
	return result;
}