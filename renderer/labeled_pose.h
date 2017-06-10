#ifndef LABELEDPOSE_H
#define LABELEDPOSE_H

#include "pose.h"
#include <vector>
#include <sstream>
#include <cstdlib>

/**
 * Represent an annoted partial view from a 3D object.
 * @author
 * 		Abner M. C.
 * @date
 * 		13/7/16
 * @version
 * 		1.0
 */
class LabeledPose : public Pose
{
public:

	/**
	 * Create a new labeled pose.
	 * @param elevation Camera elevation (angle in yÔz plane)
	 * @param azimuth Camera Azimuth (angle in xÔy plane)
	 * @param yaw Camera yaw (angle in xÔz plane)
	 * @param camDistance Camera distance from object
	 * @param filename Image from which pose was extracted
	 * @param label Label associated to the pose
	 */
	LabeledPose(float elevation, float azimuth, float yaw, float camDistance, 
		const std::string &filename, const std::string &objectName, const std::string &className, 
		int label);
		
	LabeledPose();
	
	std::string getFilename() const
	{
		return this->filename;
	}
	
	void setFilename(const std::string &filename)
	{
		this->filename = filename;
	}

	std::string getObjectName() const
	{
		return this->objectName;
	}

	void setObjectName(const std::string &objectName)
	{
		this->objectName = objectName;
	}

	std::string getClassName() const
	{
		return this->className;
	}
	
	void setClassName(const std::string &className)
	{
		this->className = className;
	}
	
	int getLabel() const
	{
		return this->label;
	}
	
	void setLabel(int label)
	{
		this->label = label;
	}

	static LabeledPose* loadFromFilename(const std::string &filename, const std::string &objName, const std::string &className);
	
private:
	std::string filename;
	std::string objectName;
	std::string className;
	int label;
	
	static std::vector<std::string> tokenize(const std::string& str, char delimiter);
	
};

#endif // LABELEDPOSE_H
