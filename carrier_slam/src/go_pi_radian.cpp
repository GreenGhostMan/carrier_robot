#include <ros/ros.h>
#include <std_msgs/String.h>
#include <actionlib_msgs/GoalID.h>
#include <actionlib_msgs/GoalStatus.h>
#include <actionlib_msgs/GoalStatusArray.h>
#include <actionlib/client/simple_action_client.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Quaternion.h>
#include <geometry_msgs/Twist.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <move_base_msgs/MoveBaseGoal.h>
#include <visualization_msgs/Marker.h>
#include <math.h>
#include <stdio.h>

#define pi 3.1417

using namespace std;

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> move_base;

static geometry_msgs::Quaternion createQuaternionFromRPY(double roll, double pitch, double yaw);

int main(int argc,char** argv)
{
	ros::init(argc,argv,"nav_test");
	ros::NodeHandle n;
	double x= 0;
	double y= 0;
if (argc != 3){
	ROS_INFO("You Can add x,y arguments!");

}else{
	double x= atoll(argv[1]);
	double y= atoll(argv[2]);
}



	geometry_msgs::Quaternion quaternion;
	double euler_angle=pi;

	
		quaternion=createQuaternionFromRPY(0,0,euler_angle);


	geometry_msgs::Pose waypoints;

	waypoints.position.x=x;
	waypoints.position.y=y;
	waypoints.position.z=0.0;
	waypoints.orientation.x=quaternion.x;
	waypoints.orientation.y=quaternion.y;
	waypoints.orientation.z=quaternion.z;
	waypoints.orientation.w=quaternion.w;



	move_base move_base("/carrier_robot/move_base",true);

	ROS_INFO("Waiting for move_base action server...");

	move_base.waitForServer(ros::Duration(60));

	ROS_INFO("Connected to move base server");
	ROS_INFO("Starting navigation test");

	
		//marker_pub.publish(markers);
		move_base_msgs::MoveBaseGoal goal;
		goal.target_pose.header.frame_id="map";
		goal.target_pose.header.stamp=ros::Time::now();
		goal.target_pose.pose=waypoints;

		move_base.sendGoal(goal);
		bool finished_within_time=move_base.waitForResult(ros::Duration(60));

		if(!finished_within_time)
		{
			move_base.cancelGoal();
			ROS_INFO("Time out achieving goal");
		}
		else
		{
			if(move_base.getState()==actionlib::SimpleClientGoalState::SUCCEEDED)
			{
				ROS_INFO("Goal succeeded!");
			}
		}


	ros::shutdown();
}

static geometry_msgs::Quaternion createQuaternionFromRPY(double roll, double pitch, double yaw)
{
    geometry_msgs::Quaternion q;
    double t0 = cos(yaw * 0.5);
    double t1 = sin(yaw * 0.5);
    double t2 = cos(roll * 0.5);
    double t3 = sin(roll * 0.5);
    double t4 = cos(pitch * 0.5);
    double t5 = sin(pitch * 0.5);
    q.w = t0 * t2 * t4 + t1 * t3 * t5;
    q.x = t0 * t3 * t4 - t1 * t2 * t5;
    q.y = t0 * t2 * t5 + t1 * t3 * t4;
    q.z = t1 * t2 * t4 - t0 * t3 * t5;
    return q;
}
