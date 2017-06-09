#ifndef POSE_H
#define POSE_H

#include <string>
#include <cmath>

/**
* Represent a partial view extracted from a 3D object.
* @author 
* 		Abner M. C.
* @date 
* 		13/7/16
* @version
* 		1.0
*/
class Pose
{
public:

	/**
	 * Create a new 3D pose.
	 * @param elevation 
	 * 		Camera elevation (angle in yÔz plane)
	 * @param azimuth 
	 * 		Camera Azimuth (angle in xÔy plane)
	 * @param yaw 
	 * 		Camera yaw (angle in xÔz plane)
	 * @param camDistance 
	 * 		Camera distance from object
	 */
	Pose(double elevation, double azimuth, double yaw, double camDistance);
	Pose();
	/**
	 * Get the distance between current pose and another pose.
	 * The distance is defined as the arc-length between the 
	 * latitude and longitude positions.
	 * @param other Pose to which the distance will be calculated
	 * @return The distance between two poses
	 */
	double getDistance(Pose *other) const;
	
	double getElevation() const
	{
		return this->elevation;
	}
	
	void setElevation(double elevation)
	{
		this->elevation = elevation;
	}
	
	double getAzimuth() const
	{
		return this->azimuth;
	}
	
	void setAzimuth(double azimuth)
	{
		this->azimuth = azimuth;
	}
	
	double getYaw() const
	{
		return this->yaw;
	}
	
	void setYaw(double yaw)
	{
		this->yaw = yaw;
	}
	
	double getCameraDistance() const
	{
		return this->camDistance;
	}
	
	void setCameraDistance(double distance)
	{
		this->camDistance = distance;
	}
	
	bool operator ==(const Pose &p)
	{
		return this->elevation == p.elevation &&
			this->azimuth == p.azimuth &&
			this->yaw == p.yaw &&
			this->camDistance == p.camDistance;
	} 
	
private:
	double elevation;
	double azimuth;
	double yaw;
	double camDistance;
		
	static double degreesToRadians(double degrees);
	
	static double radiansToDegrees(double radians);
	
};

#endif // POSE_H
