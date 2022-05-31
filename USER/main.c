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

	float pitch,roll,yaw; 		//ŷ����
	short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
	short gyrox,gyroy,gyroz;	//������ԭʼ����
//	short temp;					//�¶�	
	 
	const char *topics[] = {"/mysmarthome/sub"}; 
	unsigned short timeCount = 0;	//���ͼ������
	unsigned char *dataPtr = NULL;
	
	char PUB_BUF[256];//�ϴ����ݵ�buf
	const char *devSubTopic[] = {"/mysmarthome/sub"};
	const char devPubTopic[] = "/mysmarthome/pub";
  u8 ESP8266_INIT_OK = 0;//esp8266��ʼ����ɱ�־
	
	
 int main(void)
 {	 
	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	Usart1_Init(115200);	 	//���Դ���
	Usart2_Init(115200);        //stm32 8266 ͨѶ����

	delay_init();	//��ʱ��ʼ�� 
 	MPU_Init();					//��ʼ��MPU6050


 // while(ESP8266_INIT_OK == 0) {ESP8266_Init();}  
	ESP8266_Init();
	 
	while(OneNet_DevLink())			//����OneNET
		delay_ms(500);	
	
	OneNet_Subscribe(topics, 1);
	
	
	while(mpu_dmp_init()!=0) {}  
 	while(1)
	{

		while(mpu_dmp_get_data(&pitch,&roll,&yaw)!=0) {}
	//------------------------------������----------------------------------------------
			
			
//			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
//			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
//	//		printf("roll:%d pitch:%d yaw:%d\r\n",(int)(roll*100),(int)(pitch*100),(int)(yaw*10));delay_ms(400);
//			UsartPrintf(USART1, "roll:%d pitch:%d yaw:%d\r\n",(int)(roll*100),(int)(pitch*100),(int)(yaw*10));
			//delay_ms(400);
  //-----------------------------------------------------------------------------------		
			
			/* �ϴ����ݵ���λ��
			*/
			//mpu6050_send_data(aacx,aacy,aacz,gyrox,gyroy,gyroz);//���Զ���֡���ͼ��ٶȺ�������ԭʼ����
			//usart1_report_imu(aacx,aacy,aacz,gyrox,gyroy,gyroz,(int)(roll*100),(int)(pitch*100),(int)(yaw*10));
			
			
		if(timeCount % 40 == 0 )									//1000/25 =40 һ��һ��
		{
			while(mpu_dmp_get_data(&pitch,&roll,&yaw)!=0) {}
		//	temp=MPU_Get_Temperature();	//�õ��¶�ֵ
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
			UsartPrintf(USART1, "roll:%d pitch:%d yaw:%d\r\n",(int)(roll*100),(int)(pitch*100),(int)(yaw*10));
		}
			
		if(++timeCount >= 200)									// 5000/25=200
		{
			UsartPrintf(USART_DEBUG, "OneNet_Publish\r\n");
			
			OneNet_Publish("pcTopic", "MQTT Publish Test");
			
			timeCount = 0;
			ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(3);  //���ݵȴ�����ʱ�����
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		
		delay_ms(10);
	}
	
	
}
 


