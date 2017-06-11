#include <sstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/filesystem.hpp>
#include <time.h>
#include "RenderEngine.h"
std::vector<std::vector<double> > getSpherePoints(int numPoints)
{
	std::vector<std::vector<double> > points;
	double direction[3];
	int n = sqrt((numPoints - 2) / 4);
	direction[0] = 0;
	direction[3] = 0;
	double azimuthal= 0.5*osg::PI/n;
	for (int i = -n; i < n + 1; i++)
	{
		direction[1] = 0;
		double b = (n - std::abs(i)) * 4;
		double s = b == 0 ? 1 : b;
		double  elevation = 2 * osg::PI / s;
		for (int j = 0; j < s; j++)
		{
			points.push_back(std::vector<double>(direction, direction + 3));
			direction[1] += osg::RadiansToDegrees(elevation);
		}
		direction[0] += osg::RadiansToDegrees(azimuthal);

	}

	//int rnd = 1;
	//double offset = 2.0 / numPoints;
	////why not use 2
 //   //because y cannot be zero,y change from -1 to 1
	//double increment = osg::PI * (3 - std::sqrt(3));
	//for (int i = 0; i < numPoints; i++)
	//{
	//	double y = ((i * offset) - 1) + (offset / 2);
	//	double r = std::sqrt(1 - std::pow(y , 2));
	//	double phi = ((i + rnd) % numPoints) * increment;
	//	double x = std::cos(phi) * r;
	//	double z = std::sin(phi) * r;
	//	direction[0] = osg::RadiansToDegrees(std::atan2(y, x));
	//	direction[1] = osg::RadiansToDegrees(std::asin(z));
	//	double j = 0;
	//	while (j < 360)
	//	{
	//		direction[2] = j;
	//		points.push_back(std::vector<double>(direction,direction+3));
	//		j = j + 30;
	//	}
	//}
	//return points;
}

std::vector<std::vector<double> > getRandomPoints(int numPoints)
{
	std::vector<std::vector<double> > points;
	double direction[3];
	srand((int)time(NULL));
	//generate numPoints of distinguish Points
	for(int i=0; i<numPoints; i++)
    	{
	    	bool hasAngle = false;
	    	direction[0] = rand()%360;
	    	direction[1] = rand()%360;
			direction[3] = rand() % 360;
	    	for(int j = 0; j < points.size(); ++j)
	    	{
	    		double angleA = points[j][0];
	    		double angleE = points[j][1];
				double angleY = points[j][2];
	    		if(angleA == direction[0] && angleE == direction[1] &&angleY== direction[2])
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
	    		points.push_back(std::vector<double>(direction, direction+3));
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
int main()
{
	using namespace std;
	LabeledPose *pose = new LabeledPose;
	string filename = "frog//frog.obj";
	pose->setFilename(filename);
	pose->setAzimuth(0);
	pose->setElevation(0);
	//important
	RendererEngine *renderer = new RendererEngine(pose);
	cv::Mat depth, color;
	int viewSize = 300;
	renderer->render(viewSize, viewSize, depth, color);
	//imshow("d", depth);
	//imshow("c", color);
	return 0;
}
int debugmain(int argc, char **argv)
{
	using namespace std;
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
		std::string filenameName = filename.substr(filename.find_last_of('\\') + 1);
	#else
		string filenameName = filename.substr(filename.find_last_of('/') + 1);
	#endif
	boost::filesystem::create_directories(outputDir + "/" + filenameName);
	
	std::vector<std::pair<std::string, cv::Mat> > images;
	 std::vector<std::vector<double> > points;
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
	for (std::vector<std::vector<double> >::iterator it = points.begin(); it != points.end(); ++it)
	{
		
		double azimuth =(*it)[0];
		double elevation=(*it)[1];
		double yaw = (*it)[2];
		pose->setAzimuth(azimuth);
		pose->setElevation(elevation);
		cout << "Reading " << filename << "; elevation=" << elevation << "; azimuth=" << azimuth << "..." << endl;
		try 
		{
			cv::Mat depth, color;
			renderer->render(viewSize, viewSize, depth, color);
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

