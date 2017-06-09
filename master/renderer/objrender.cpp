#include "renderer.h"
#include <sstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/filesystem.hpp>
#include "util.h"
#include <time.h>
#include "RenderEngine.h"
std::vector<std::pair<double, double> > getSpherePoints(int numPoints)
{
	std::vector<std::pair<double, double> > points;
	int rnd = 1;
	double offset = 2.0 / numPoints;
	//why not use 2
	double increment = osg::PI * (3 - std::sqrt(3));
	for (int i = 0; i < numPoints; i++)
	{
		double y = ((i * offset) - 1) + (offset / 2);
		double r = std::sqrt(1 - std::pow(y , 2));
		double phi = ((i + rnd) % numPoints) * increment;
		double x = std::cos(phi) * r;
		double z = std::sin(phi) * r;
		double azimuth = osg::RadiansToDegrees(std::atan2(y, x));
		double elevation = osg::RadiansToDegrees(std::asin(z));
		points.push_back(std::make_pair(azimuth, elevation));
	}
	return points;
}

std::vector<std::pair<double, double> > getRandomPoints(int numPoints)
{
	std::vector<std::pair<double, double> > points;
	double azimuth, elevation;
	srand((int)time(NULL));
	//generate numPoints of distinguish Points
	for(int i=0; i<numPoints; i++)
    	{
	    	bool hasAngle = false;
	    	azimuth = rand()%360;
	    	elevation = rand()%360;
	    	for(int j = 0; j < points.size(); ++j)
	    	{
	    		double angleA = points[j].first;
	    		double angleE = points[j].second;

	    		if(angleA == azimuth && elevation == angleE)
	    		{
	    			hasAngle = true;
	    			break;
	    		}
	    	} 

	    	if(hasAngle == true)
	    	{
	    		i--;
	    		continue;
	    	}
	    	else
	    	{
	    		points.push_back(std::make_pair(azimuth, elevation));
	    	}
    	}
		
	return points;
}

cv::Mat AddGuassianNoise(cv::Mat src)   
{
	int w = src.cols;
	int h = src.rows;
	int maxDepth = 0;
	cv::Mat dst(h, w, CV_8U);
	cv::Mat noise(h, w, CV_8U);
	
	for (int i = 0; i < src.cols; ++i)
	{
		for (int j = 0; j < src.rows; ++j)
		{
			int value = src.at<char>(i, j);
			if (maxDepth < value)
			{
				maxDepth = value;
			}
		}
	}

	CvRNG rng = cvRNG(-1);
	cv::randn(noise, 0, maxDepth*0.05);
	cv::add(src, noise, dst);
	return dst;
}

int main(int argc, char **argv)
{
	if (argc < 4) 
	{
		std::cerr << "Usage: renderer <MODE: 0 = DEPTH, 1 = RGB, 2 = RGBD, 3 = GRAYSCALE, 4 = gaussian noise> <INPUT_FILENAME> <OUTPUT_DIR> [NUMBER_OF_VIEWS][Random or not] [VIEW_SIZE]" << std::endl;
		exit(-1);
	}
	int mode = atoi(argv[1]);
	std::string filename = argv[2];
	std::string outputDir = argv[3];
	int numViews = argc > 4 ? atoi(argv[4]) : 360;
	bool bRandom = argc > 5 ? atoi(argv[5]) : false;
	int viewSize = argc > 6 ? atoi(argv[6]) : 64;
	
	#ifdef _WIN32
		string filenameName = filename.substr(filename.find_last_of('\\') + 1);
	#else
		string filenameName = filename.substr(filename.find_last_of('/') + 1);
	#endif
	boost::filesystem::create_directories(outputDir + "/" + filenameName);
	
	std::vector<std::pair<std::string, cv::Mat> > images;
	 std::vector<std::pair<double, double> > points;
	if(bRandom)
	{
		points = getRandomPoints(numViews);
	}
	else
	{
		points = getSpherePoints(numViews);
	}
	LabeledPose *pose = new LabeledPose;
	pose->setFilename(filename);
	//important
	RendererEngine *renderer = new RendererEngine(pose);
	for (std::vector<std::pair<double, double> >::iterator it = points.begin(); it != points.end(); ++it)
	{
		
		double azimuth = it->first;
		double elevation = it->second;
		pose->setAzimuth(azimuth);
		pose->setElevation(elevation);
		cout << "Reading " << filename << "; elevation=" << elevation << "; azimuth=" << azimuth << "..." << endl;
		try 
		{
			cv::Mat depth, color;
			renderer->render(viewSize, viewSize, depth, color, cv::Vec4b(0, 255, 0, 1));
			std::stringstream ss;
			ss << outputDir << "/" << filenameName << "/" << pose->getCameraDistance() << "_" << pose->getElevation() << "_" << pose->getAzimuth() << "_" << pose->getYaw() << ".png";
			std::vector<cv::Mat> channels;
			cv::split(color, channels);
			cv::Mat mergedImage;
			switch (mode)
			{
				case 0:
					mergedImage = depth;
					break;
				case 1:
					cv::merge(channels, mergedImage);
					break;
				case 2:
					channels.push_back(depth);
					cv::merge(channels, mergedImage);
					break;
				case 3:
					cv::merge(channels, mergedImage);
					cv::cvtColor(mergedImage, mergedImage, CV_BGR2GRAY);
					break;
				case 4:
					depth = AddGuassianNoise(depth);
					channels.push_back(depth);
					cv::merge(channels, mergedImage);
					break;
				default:
					std::cerr << "Unrecognized option." << std::endl;
					exit(-1);
			}
                	images.push_back(std::make_pair(ss.str(), mergedImage));
                }
                catch (char const* s)
                {
                        std::cerr << s << std::endl;
                        exit(-1);
                }
        }
        delete pose;
        delete renderer;

        for (std::vector<std::pair<std::string, cv::Mat> >::iterator it = images.begin(); it != images.end(); ++it)
        {
                std::string fileName = it->first;
                cv::Mat mtImg= it->second;
                std::cout << "Saving to " << it->first << "..." << std::endl;
                cv::imwrite(fileName, mtImg);
        }

        return 0;
}

