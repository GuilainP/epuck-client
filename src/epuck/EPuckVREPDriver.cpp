#include "RobotDriver.hpp"

#include "extApiPlatform.h"
#include "extApi.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <thread>
#include <chrono>

EPuckVREPDriver::EPuckVREPDriver(Robot& robot) : RobotDriver(robot), ajouter_(1) {
}
 
bool EPuckVREPDriver::Init() {
    client_id_ = simxStart((simxChar*)"127.0.0.1", 19997, true, true, 2000, 5);
    std::cout << "clientID = " << client_id_ << std::endl;

    if (client_id_ == -1) {
        std::cout << ("Could not connect to V-REP remote API server") << std::endl;
        simxFinish(client_id_);
        return false;
    } else {
        minr_ = 0; minl_ =0;
        maxr_ = 0; maxl_ = 0;
        std::cout << ("Connected to remote API server") << std::endl;

        simxSynchronous(client_id_, true);
        simxStartSimulation(client_id_, simx_opmode_oneshot);

        // Handles
        simxGetObjectHandle(client_id_, "ePuck", &epuck_handle_, simx_opmode_blocking);

        simxGetObjectHandle(client_id_, "ePuck_rightJoint", &right_joint_handle_, simx_opmode_blocking);
        simxGetObjectHandle(client_id_, "ePuck_leftJoint", &left_joint_handle_, simx_opmode_blocking);

        simxGetObjectHandle(client_id_, "ePuck_camera", &vision_handle_, simx_opmode_blocking);

        simxGetObjectHandle(client_id_, "ePuck_proxSensor0", &prox_sensors_handle_[0], simx_opmode_blocking);
        simxGetObjectHandle(client_id_, "ePuck_proxSensor1", &prox_sensors_handle_[1], simx_opmode_blocking);
        simxGetObjectHandle(client_id_, "ePuck_proxSensor2", &prox_sensors_handle_[2], simx_opmode_blocking);
        simxGetObjectHandle(client_id_, "ePuck_proxSensor3", &prox_sensors_handle_[3], simx_opmode_blocking);
        simxGetObjectHandle(client_id_, "ePuck_proxSensor4", &prox_sensors_handle_[4], simx_opmode_blocking);
        simxGetObjectHandle(client_id_, "ePuck_proxSensor5", &prox_sensors_handle_[5], simx_opmode_blocking);
        simxGetObjectHandle(client_id_, "ePuck_proxSensor6", &prox_sensors_handle_[6], simx_opmode_blocking);
        simxGetObjectHandle(client_id_, "ePuck_proxSensor7", &prox_sensors_handle_[7], simx_opmode_blocking);

        simxReadProximitySensor(client_id_, prox_sensors_handle_[0],&detection_state_ir_[0],detected_point_ir_[0], NULL,NULL, simx_opmode_streaming);
        simxReadProximitySensor(client_id_, prox_sensors_handle_[1],&detection_state_ir_[1],detected_point_ir_[1], NULL,NULL, simx_opmode_streaming);
        simxReadProximitySensor(client_id_, prox_sensors_handle_[2],&detection_state_ir_[2],detected_point_ir_[2], NULL,NULL, simx_opmode_streaming);
        simxReadProximitySensor(client_id_, prox_sensors_handle_[3],&detection_state_ir_[3],detected_point_ir_[3], NULL,NULL, simx_opmode_streaming);
        simxReadProximitySensor(client_id_, prox_sensors_handle_[4],&detection_state_ir_[4],detected_point_ir_[4], NULL,NULL, simx_opmode_streaming);
        simxReadProximitySensor(client_id_, prox_sensors_handle_[5],&detection_state_ir_[5],detected_point_ir_[5], NULL,NULL, simx_opmode_streaming);
        simxReadProximitySensor(client_id_, prox_sensors_handle_[6],&detection_state_ir_[6],detected_point_ir_[6], NULL,NULL, simx_opmode_streaming);
        simxReadProximitySensor(client_id_, prox_sensors_handle_[7],&detection_state_ir_[7],detected_point_ir_[7], NULL,NULL, simx_opmode_streaming);

        simxGetObjectPosition(client_id_, epuck_handle_, -1, epuck_position_, simx_opmode_streaming);
        simxGetObjectOrientation(client_id_, epuck_handle_, -1, euler_angles_, simx_opmode_streaming);

        simxGetVisionSensorImage(client_id_, vision_handle_, res_, &sim_image_, 0, simx_opmode_streaming);

        simxGetJointPosition(client_id_, left_joint_handle_, &left_joint_position_, simx_opmode_streaming);
        simxGetJointPosition(client_id_, right_joint_handle_, &right_joint_position_, simx_opmode_streaming);

        simxGetObjectVelocity(client_id_, left_joint_handle_, NULL, left_joint_velocity_, simx_opmode_streaming);
        simxGetObjectVelocity(client_id_, right_joint_handle_, NULL, right_joint_velocity_, simx_opmode_streaming);

       

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        return true;
    }
}


void EPuckVREPDriver::Read() {

    simxSynchronousTrigger(client_id_);

    simxReadProximitySensor(client_id_, prox_sensors_handle_[0],&detection_state_ir_[0],detected_point_ir_[0],NULL, NULL, simx_opmode_buffer);
    simxReadProximitySensor(client_id_, prox_sensors_handle_[1],&detection_state_ir_[1],detected_point_ir_[1],NULL, NULL, simx_opmode_buffer);
    simxReadProximitySensor(client_id_, prox_sensors_handle_[2],&detection_state_ir_[2],detected_point_ir_[2],NULL, NULL, simx_opmode_buffer);
    simxReadProximitySensor(client_id_, prox_sensors_handle_[3],&detection_state_ir_[3],detected_point_ir_[3],NULL, NULL, simx_opmode_buffer);
    simxReadProximitySensor(client_id_, prox_sensors_handle_[4],&detection_state_ir_[4],detected_point_ir_[4],NULL, NULL, simx_opmode_buffer);
    simxReadProximitySensor(client_id_, prox_sensors_handle_[5],&detection_state_ir_[5],detected_point_ir_[5],NULL, NULL, simx_opmode_buffer);
    simxReadProximitySensor(client_id_, prox_sensors_handle_[6],&detection_state_ir_[6],detected_point_ir_[6],NULL, NULL, simx_opmode_buffer);
    simxReadProximitySensor(client_id_, prox_sensors_handle_[7],&detection_state_ir_[7],detected_point_ir_[7],NULL, NULL, simx_opmode_buffer);

    simxGetObjectPosition(client_id_, epuck_handle_, -1, epuck_position_, simx_opmode_buffer);
    simxGetObjectOrientation(client_id_, epuck_handle_, -1, euler_angles_, simx_opmode_buffer);

    simxGetVisionSensorImage(client_id_, vision_handle_, res_, &sim_image_, 0, simx_opmode_buffer);

    simxGetJointPosition(client_id_, left_joint_handle_, &left_joint_position_, simx_opmode_buffer);
    simxGetJointPosition(client_id_, right_joint_handle_, &right_joint_position_, simx_opmode_buffer);

    simxGetObjectVelocity(client_id_, left_joint_handle_, NULL, left_joint_velocity_, simx_opmode_buffer);
    simxGetObjectVelocity(client_id_, right_joint_handle_, NULL, right_joint_velocity_, simx_opmode_buffer);

    


    simxGetPingTime(client_id_, &ping_time_);


    PrintSensors();
}

void EPuckVREPDriver::Send() {
    simxPauseCommunication(client_id_, 1);
    simxSetJointTargetVelocity(client_id_, left_joint_handle_, robot().wheels_command.left_velocity, simx_opmode_oneshot);
    simxSetJointTargetVelocity(client_id_, right_joint_handle_, robot().wheels_command.right_velocity, simx_opmode_oneshot);
    simxPauseCommunication(client_id_, 0);
}

void EPuckVREPDriver::getVisionSensor(Robot& robot) {
	cv::Mat img(res_[0], res_[1], CV_8UC3, (unsigned char*) sim_image_);
	cv::flip(img, robot.vision_sensors, 0);
    cv::cvtColor(robot.vision_sensors, robot.vision_sensors, cv::COLOR_BGR2RGB);
	cv::namedWindow("Camera", cv::WINDOW_AUTOSIZE);
	cv::imshow("Camera", robot.vision_sensors);
	cv::waitKey(1);
    
	cv::imwrite(log().folder_ + "/image/image" +  std::string( 4 - std::to_string(ajouter_).length(), '0').append( std::to_string(ajouter_)) + ".png", robot.vision_sensors);

	++ajouter_;
}

EPuckVREPDriver::~EPuckVREPDriver() {
    simxStopSimulation(client_id_, simx_opmode_blocking);
    simxFinish(client_id_);
    std::cout << "End of the program" << std::endl;
}

void EPuckVREPDriver::PrintSensors() {
    dataToRobot();
    std::cout << COLOR_COUT_BLUE << "Iteration N"<< ajouter_ << "\n" << COLOR_COUT_RESET
              << "ePuck location :  x : " << robot().current_pose.x << ", y : " << robot().current_pose.y << ", th : " << robot().current_pose.th << "\n"
              << "Joint position [rad] :  Left : " << robot().wheels_state.left_position << ", Right : " << robot().wheels_state.right_position << "\n"
              << "Speed [rad/s]   : Left : "  << robot().wheels_state.left_velocity << ", Right : " << robot().wheels_state.right_velocity << std::endl;


    if(detection_state_ir_[0]!=0) {
        std::cout << "IR0 : " << robot().proximity_sensors.ir[0] << "\n";
    }     
    else{
        std::cout << COLOR_COUT_BLACK << "IR0 : NONE" << COLOR_COUT_RESET << "\n";
    }
    
    if(detection_state_ir_[1]!=0) {
        std::cout << "IR1 : " << robot().proximity_sensors.ir[1] << "\n";
    }
    else{ 
        std::cout << COLOR_COUT_BLACK << "IR1 : NONE" << COLOR_COUT_RESET << "\n";
    }
        
    if(detection_state_ir_[2]!=0) {
        std::cout << "IR2 : " << robot().proximity_sensors.ir[2] << "\n";
    }
    else{
        std::cout << COLOR_COUT_BLACK << "IR2 : NONE" << COLOR_COUT_RESET << "\n";
    }

    if(detection_state_ir_[3]!=0) {
        std::cout << "IR3 : " << robot().proximity_sensors.ir[3] << "\n";
    }
    else{
        std::cout << COLOR_COUT_BLACK << "IR3 : NONE" << COLOR_COUT_RESET << "\n";
    }

    if(detection_state_ir_[4]!=0) {
        std::cout << "IR4 : " << robot().proximity_sensors.ir[4] << "\n";
    }
    else{
        std::cout << COLOR_COUT_BLACK << "IR4 : NONE" << COLOR_COUT_RESET << "\n";
    }
        
    if(detection_state_ir_[5]!=0) {
        std::cout << "IR5 : " << robot().proximity_sensors.ir[5] << "\n";
    }
    else{
        std::cout << COLOR_COUT_BLACK << "IR5 : NONE" << COLOR_COUT_RESET << "\n";
    }
        
    if(detection_state_ir_[6]!=0) {
        std::cout << "IR6 : " << robot().proximity_sensors.ir[6] << "\n";
    }
    else{
        std::cout << COLOR_COUT_BLACK << "IR6 : NONE" << COLOR_COUT_RESET << "\n";
    }
        
    if(detection_state_ir_[7]!=0) {
        std::cout << "IR7 : " << robot().proximity_sensors.ir[7] << "\n";
    }
    else{
        std::cout << COLOR_COUT_BLACK << "IR7 : NONE" << COLOR_COUT_RESET << "\n";
    }



    log().addIn(log().file_epuck_pose[0], robot().current_pose.x);
    log().addIn(log().file_epuck_pose[1], robot().current_pose.y);
    log().addIn(log().file_epuck_pose[2], robot().current_pose.th);

    log().addIn(log().file_ir[0], robot().proximity_sensors.ir[0]);
    log().addIn(log().file_ir[1], robot().proximity_sensors.ir[1]);
    log().addIn(log().file_ir[2], robot().proximity_sensors.ir[2]);
    log().addIn(log().file_ir[3], robot().proximity_sensors.ir[3]);
    log().addIn(log().file_ir[4], robot().proximity_sensors.ir[4]);
    log().addIn(log().file_ir[5], robot().proximity_sensors.ir[5]);
    log().addIn(log().file_ir[6], robot().proximity_sensors.ir[6]);
    log().addIn(log().file_ir[7], robot().proximity_sensors.ir[7]);

    log().addIn(log().file_epuck_left_wheel_position, robot().wheels_state.left_position);
    log().addIn(log().file_epuck_right_wheel_position, robot().wheels_state.right_position);

    log().addIn(log().file_epuck_left_wheel_velocity, robot().wheels_state.left_velocity);
    log().addIn(log().file_epuck_right_wheel_velocity, robot().wheels_state.right_velocity);

}

void EPuckVREPDriver::dataToRobot() {
    for (int i = 0; i < 8; i++) {
        if(detection_state_ir_[i]!=0){
            if (std::abs(detected_point_ir_[i][2]) < 0.01 || std::abs(detected_point_ir_[i][2]) > 1) {
                robot().proximity_sensors.ir[i] = 0;
                } else {
                    robot().proximity_sensors.ir[i] = std::abs(detected_point_ir_[i][2]);
                }
        }
        else {
            robot().proximity_sensors.ir[i] = -1;
        }

    }


    robot().wheels_state.left_velocity = left_joint_velocity_[0];
    robot().wheels_state.right_velocity= right_joint_velocity_[0];

    robot().wheels_state.left_position = left_joint_position_;
    robot().wheels_state.right_position = right_joint_position_;
    
    robot().current_pose.x = epuck_position_[0];
    robot().current_pose.y = epuck_position_[1];
    robot().current_pose.th = euler_angles_[1];


    
}
