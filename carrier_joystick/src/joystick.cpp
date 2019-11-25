#include "controller.h"
#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <geometry_msgs/Twist.h>



int main(int argc,char** argv)
{
  ros::init(argc,argv,"joystick");
  ros::NodeHandle n;
  
  ControlClass cc(&n);
  ros::spin();
  return 0;
}

ControlClass::ControlClass(ros::NodeHandle *nh) : n(*nh)
{
  initAll();
}

void ControlClass::initAll()
{
  sub = n.subscribe("/joy",50,&ControlClass::callback,this);
  pub = n.advertise<geometry_msgs::Twist>("cmd_vel",50,true);
}

  geometry_msgs::Twist jj;

void::ControlClass::callback(const sensor_msgs::Joy::ConstPtr& joy)
{
  float linearX = joy->axes[1];
  float angularZ = joy->axes[3];
  
  jj.linear.x = 0.2 * linearX;
  jj.angular.z = 0.5* angularZ;
  ROS_INFO_STREAM("axes1 " <<jj.linear.x << "axes2 "<< jj.angular.z);
  pub.publish(jj);
}
