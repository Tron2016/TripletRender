#define CPU_ONLY 0
#define USE_OPENCV 1

#include <set>
#include <queue>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/filesystem.hpp>

#include <caffe/proto/caffe.pb.h>
#include <caffe/util/db.hpp>
#include <caffe/util/format.hpp>
#include <caffe/util/io.hpp>
#include <caffe/util/rng.hpp>
#include <caffe/util/math_functions.hpp>

#include "labeled_pose.h"

namespace fs = boost::filesystem;

#define BATCH_SIZE 300

DEFINE_string(backend, "lmdb", "The backend {lmdb, leveldb} for storing the result");

inline std::string getDirName(const std::string &dir)
{
	return dir.substr(dir.find_last_of("/") + 1);
}

LabeledPose* getPositive(const std::vector<LabeledPose*> &poses, LabeledPose *pose, int similarityThreshold)
{
	std::vector<LabeledPose*> shuffledPoses(poses.begin(), poses.end());
	std::random_shuffle(shuffledPoses.begin(), shuffledPoses.end());
	for (std::vector<LabeledPose*>::iterator it = shuffledPoses.begin(); it != shuffledPoses.end(); ++it)
	{
		if ((*it)->getObjectName() == pose->getObjectName())
		{
			double dist = pose->getDistance(*it);
			if (dist > 0 && dist < similarityThreshold)
			{
				return *it;
			}
		}
	}
}

std::vector<LabeledPose*> getNegatives(const std::vector<LabeledPose*> &poses, LabeledPose *pose, int numNegatives, int similarityThreshold)
{
	std::vector<LabeledPose*> shuffledPoses(poses.begin(), poses.end());
	std::random_shuffle(shuffledPoses.begin(), shuffledPoses.end());
	std::vector<LabeledPose*> negatives;
	for (std::vector<LabeledPose*>::iterator it = shuffledPoses.begin(); it != shuffledPoses.end(); ++it)
	{
		if ((*it)->getClassName() != pose->getClassName())
		{
			negatives.push_back(*it);
			if (negatives.size() >= numNegatives) break;
		}
	}
	return negatives;
}

void printPose(LabeledPose *pose)
{
	std::cout << pose->getObjectName() << "[" << pose->getElevation() << "," << pose->getAzimuth() << "] ";
}

struct Triplet
{
	LabeledPose *ref;
	LabeledPose *pos;
	std::vector<LabeledPose*> neg;
};

void printTriplet(Triplet *triplet)
{
	printPose(triplet->ref);
	printPose(triplet->pos);
	for (std::vector<LabeledPose*>::iterator it = triplet->neg.begin(); it != triplet->neg.end(); ++it)
	{
		printPose(*it);
	}
}

std::string getEncodedDatum(LabeledPose *pose)
{
	std::string value;
	cv::Mat img = cv::imread(pose->getFilename(), -1);
	if (!img.data)
	{
		throw ("Could not read image " + pose->getFilename()).c_str();
	}
	caffe::Datum datum;
	caffe::CVMatToDatum(img, &datum);
	datum.SerializeToString(&value);
	return value;
}

int countTriplets = 0;

void encodePose(LabeledPose *pose, boost::scoped_ptr<caffe::db::Transaction> &tnx)
{
	std::string key = caffe::format_int(countTriplets++, 8);
	std::string value = getEncodedDatum(pose);
	tnx->Put(key, value);
}

void saveTriplet(Triplet *triplet, boost::scoped_ptr<caffe::db::Transaction> &tnx)
{
	encodePose(triplet->ref, tnx);
	encodePose(triplet->pos, tnx);
	for (std::vector<LabeledPose*>::iterator it = triplet->neg.begin(); it != triplet->neg.end(); ++it)
	{
		encodePose(*it, tnx);
	}
}

int main(int argc, char **argv)
{
	if (argc < 3) 
	{
		std::cerr << "Usage: generator <INPUT_DIR> <OUTPUT_DIR> [NUM_NEGATIVES] [SIMILARITY_THRESHOLD]" << std::endl;
		exit(-1);
	}
	std::string inputDir = argv[1];
	std::string outputDir = argv[2];
	int numNegatives = argc > 3 ? atoi(argv[3]) : 1;
	int similarityThreshold = argc > 4 ? atoi(argv[4]) : 20;

	srand(time(0));
	
	std::cout << "Reading poses..." << std::endl;
	std::vector<LabeledPose*> poses;
	for (fs::directory_iterator class_itr(inputDir); class_itr != fs::directory_iterator(); ++class_itr)
	{
		for (fs::directory_iterator obj_itr(class_itr->path()); obj_itr != fs::directory_iterator(); ++obj_itr)
		{
			for (fs::directory_iterator view_itr(obj_itr->path()); view_itr != fs::directory_iterator(); ++view_itr)
			{
				std::string filename = view_itr->path().string();
				std::string objname = getDirName(obj_itr->path().string());
				std::string classname = getDirName(class_itr->path().string()); 
				LabeledPose *pose = LabeledPose::loadFromFilename(filename, objname, classname);
				poses.push_back(pose);
			}
		}
	}
	
	std::cout << "Building triplets..." << std::endl;
	std::vector<Triplet*> triplets;
	for (std::vector<LabeledPose*>::iterator it = poses.begin(); it != poses.end(); ++it)
	{
		Triplet *triplet = new Triplet;
		triplet->ref = *it;
		triplet->pos = getPositive(poses, *it, similarityThreshold);
		triplet->neg = getNegatives(poses, *it, numNegatives, similarityThreshold);
		std::cout << "New triplet: ";
		printTriplet(triplet);
		std::cout << std::endl;
		triplets.push_back(triplet);
	}
	
	std::random_shuffle(triplets.begin(), triplets.end());
	
	std::cout << "Saving triplets..." << std::endl;
	// Create a new DB
	boost::scoped_ptr<caffe::db::DB> db(caffe::db::GetDB(FLAGS_backend));
	db->Open(outputDir + "/db", caffe::db::NEW);
	boost::scoped_ptr<caffe::db::Transaction> tnx(db->NewTransaction());
	for (size_t i = 0; i < triplets.size(); ++i)
	{
		std::cout << "Saving triplet: ";
		printTriplet(triplets[i]);
		std::cout << std::endl;
		saveTriplet(triplets[i], tnx);
		if (i % BATCH_SIZE == 0)
		{
			std::cout << "Comitting..." << std::endl;
			tnx->Commit();
			tnx.reset(db->NewTransaction());
		}
	}
	std::cout << "Comitting..." << std::endl;
	tnx->Commit();
	std::cout << "#Triplets: " << triplets.size() << std::endl;
	
	return 0;
}
