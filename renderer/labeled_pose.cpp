#include "labeled_pose.h"
LabeledPose::LabeledPose(float elevation, float azimuth, float yaw, float camDistance,
	const std::string &filename, const std::string &objectName, const std::string &className,
	int label):Pose(elevation, azimuth, yaw, camDistance){

}
LabeledPose::LabeledPose() {

}

LabeledPose* loadFromFilename(const std::string &filename, const std::string &objName, const std::string &className) {
	LabeledPose* ptr = new LabeledPose();
	return ptr;
}
std::vector<std::string> tokenize(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	return tokens;
}