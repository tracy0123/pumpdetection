//#include "led.h"
#include "delay.h"

#include "sys.h"

#include "usart.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "usart.h"
#include "esp8266.h"
#include "onenet.h"

	float pitch,roll,yaw; 		//欧拉角
	short aacx,aacy,aacz;		//加速度传感器原始数据
	short gyrox,gyroy,gyroz;	//陀螺仪原始数据
//	short temp;					//温度	
	 
	const char *topics[] = {"/mysmarthome/sub"}; 
	unsigned short timeCount = 0;	//发送间隔变量
	unsigned char *dataPtr = NULL;
	
	char PUB_BUF[256];//上传数据的buf
	const char *devSubTopic[] = {"/mysmarthome/sub"};
	const char devPubTopic[] = "/mysmarthome/pub";
  u8 ESP8266_INIT_OK = 0;//esp8266初始化完成标志
	
	
 int main(void)
 {	 
	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	Usart1_Init(115200);	 	//调试串口
	Usart2_Init(115200);        //stm32 8266 通讯串口

	delay_init();	//延时初始化 
 	MPU_Init();					//初始化MPU6050


 // while(ESP8266_INIT_OK == 0) {ESP8266_Init();}  
	ESP8266_Init();
	 
	while(OneNet_DevLink())			//接入OneNET
		delay_ms(500);	
	
	OneNet_Subscribe(topics, 1);
	
	
	while(mpu_dmp_init()!=0) {}  
 	while(1)
	{

		while(mpu_dmp_get_data(&pitch,&roll,&yaw)!=0) {}
	//------------------------------调试用----------------------------------------------
			
			
//			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
//			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
//	//		printf("roll:%d pitch:%d yaw:%d\r\n",(int)(roll*100),(int)(pitch*100),(int)(yaw*10));delay_ms(400);
//			UsartPrintf(USART1, "roll:%d pitch:%d yaw:%d\r\n",(int)(roll*100),(int)(pitch*100),(int)(yaw*10));
			//delay_ms(400);
  //-----------------------------------------------------------------------------------		
			
			/* 上传数据到上位机
			*/
			//mpu6050_send_data(aacx,aacy,aacz,gyrox,gyroy,gyroz);//用自定义帧发送加速度和陀螺仪原始数据
			//usart1_report_imu(aacx,aacy,aacz,gyrox,gyroy,gyroz,(int)(roll*100),(int)(pitch*100),(int)(yaw*10));
			
			
		if(timeCount % 40 == 0 )									//1000/25 =40 一秒一次
		{
			while(mpu_dmp_get_data(&pitch,&roll,&yaw)!=0) {}
		//	temp=MPU_Get_Temperature();	//得到温度值
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
			UsartPrintf(USART1, "roll:%d pitch:%d yaw:%d\r\n",(int)(roll*100),(int)(pitch*100),(int)(yaw*10));
		}
			
		if(++timeCount >= 200)									// 5000/25=200
		{
			UsartPrintf(USART_DEBUG, "OneNet_Publish\r\n");
			
			OneNet_Publish("pcTopic", "MQTT Publish Test");
			
			timeCount = 0;
			ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(3);  //根据等待接收时间调整
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		
		delay_ms(10);
	}
	
	
}
 


