#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>

#define TIMERNUM	6

typedef struct
{
	char id;
	struct itimerval value;
	char timer_name[10];
}timer;

timer hapi_timer[TIMERNUM];

enum
{
	IOCTL_TIMERINIT = 5,
	IOCTL_TIMERSET, //set value
	IOCTL_STATEGATE,//get value
	IOCTL_TIMEDEL,
};

typedef struct
{
	int id;//TIMER��
	int data;//TIMER��ʼֵ
	int timer_flag;
} sep_timer;

#define STEP_TIMER   2
#define HEAD_TIMER  3
#define PFSTEP_TIMER  1
#define CLOCK_TIMER   4

/**
 * �������ܣ�����һ��Ӧ�ö�ʱ����ʱ��ƬΪ10ms�ı�����
 * ������ָ���˶�ʱ����һ�ζ�ʱʱ����Ժ�ÿ�ζ�ʱ��ʱ�䣬
 * ����ڴ���Ӧ�ö�ʱ�����������������У�����Ҫ�Ժ����
 * ����Ӧ�ö�ʱ�����������ʱ��������
 * pcName:������8�ֽ��ַ���,Ӧ�ö�ʱ��������
 * ExpirationRoutine:��ʱ��ʱ�䵽ʱ�Ĵ�����
 * ulID:��ʱ���������õ��Ĳ�������������ͬһ��������ͬ�¼�
 * ulInitialTime:ָ����ʱ����ʼ��ʱ��Ƭ
 * ulRescheduleTime:ָ���ڵ�һ��ʱ��Ƭ����ÿ�ζ�ʱ��ʱ��Ƭ��0��ʾ��ʱ����ִ��һ��
 * uiEnable:4-->��ֹӦ�ö�ʱ���ڴ���������У��Ժ���Ҫ��ST_ControlTimer���ƶ�ʱ������
 * :5-->����Ӧ�ö�ʱ���ڴ����������
 * phOSHandle( ����):	 ������Ӧ�ö�ʱ�����
 * ����ֵ:0x00------->�����ɹ�
 */
int timer_item;
unsigned int ST_CreateAppTimer(const char *pcName, void (*ExpirationRoutine)(int), unsigned long ulInitialTime, unsigned long ulRescheduleTime, int uiEnable, int *phOSHandle)
{
	if (uiEnable < 4 || uiEnable > 5)
		return 0xFFFFFFFB;	// ��ֹ�������������

	if (ulInitialTime == 0)
		return 0xFFFFFFF3;	// ��ʼ��ʱ��Ƭ����Ϊ0

	for (timer_item = 0; timer_item < TIMERNUM; timer_item++)
	{
		if (hapi_timer[timer_item].id == 0)
			break;
	}

	memcpy(&hapi_timer[timer_item].timer_name, pcName, strlen(pcName));
	memset(&hapi_timer[timer_item].value, 0, sizeof(hapi_timer[timer_item].value));

	hapi_timer[timer_item].value.it_value.tv_sec = 0;
	hapi_timer[timer_item].value.it_value.tv_usec = 1000 * ulInitialTime;
	hapi_timer[timer_item].value.it_interval.tv_sec = 0;
	hapi_timer[timer_item].value.it_interval.tv_usec = 1000 * ulRescheduleTime;

	if (uiEnable == 4)
	{
		setitimer(ITIMER_VIRTUAL, &hapi_timer[timer_item].value, NULL);
		signal(SIGVTALRM, ExpirationRoutine);
	}
	else
		signal(SIGVTALRM, ExpirationRoutine);

	hapi_timer[timer_item].id = timer_item + 1;
	*phOSHandle = timer_item + 1;

	return 0;
}

/**
 * �������ܣ�ɾ��ָ����Ӧ�ö�ʱ��
 * phOSHandle( ���):��ʱ��������ɴ�����ʱ���������
 * phOSHandle( ����):ɾ���ɹ������ó�EM_INVALID_HANDLE��ʧ���򲻸ı�ԭ��ֵ
 *  ����ֵ:0x00------->�����ɹ�
 */
unsigned int ST_DeleteAppTimer(int *phOSHandle)
{
	int timer_item;

	for (timer_item = 0; timer_item < TIMERNUM; timer_item++)
	{
		if (hapi_timer[timer_item].id == *phOSHandle)
			break;
	}

	if (timer_item >= TIMERNUM)
		return 0xFFFFFFDD;

	memset(&hapi_timer[timer_item], 0, sizeof(&hapi_timer[timer_item]));
	*phOSHandle = 0xFFFFFFE6;

	return 0;
}

/**
 * �������ܣ���ֹ������Ӧ�ö�ʱ������,��ʱ������ǰ�Ѿ�������
 * hOSHandle:ָ��Ӧ�ö�ʱ���ľ��
 * uiEnable:---ȡֵ��Χ,EM_rtos_DISABLE_TIMER;ʵ��ֵ4,��ֹӦ�ö�ʱ������
 * ---ȡֵ��Χ,EM_rtos_ENABLE_TIMER;ʵ��ֵ5,����Ӧ�ö�ʱ������
 * ����ֵ:	0x00-->�����ɹ�
 */
unsigned int ST_ControlAppTimer(int hOSHandle, unsigned int uiEnable)
{
	int timer_item;

	for (timer_item = 0; timer_item < TIMERNUM; timer_item++)
	{
		if (hapi_timer[timer_item].id == hOSHandle)
			break;
	}

	if (timer_item >= TIMERNUM)
		return 0xFFFFFFE6;

	if (uiEnable < 4 || uiEnable > 5)
		return 0xFFFFFFFB;

	if (uiEnable == 4)
	{
		setitimer(ITIMER_VIRTUAL, &hapi_timer[timer_item].value, NULL);
	}

	return 0;
}

/**
 * �������ܣ��������ö�ʱ����������ʱ����������״κ��Ժ�ʱ�䣬�Ƿ�������������
 * hOSHandle:��ʱ��������ɴ�����ʱ���������
 * ExpirationRoutine:��ʱ��ʱ�䵽ʱ�Ĵ�����
 * ulInitialTime:ָ����ʱ����ʼ��ʱ��Ƭ
 * ulRescheduleTime:ָ���ڵ�һ��ʱ��Ƭ����ÿ�ζ�ʱ��ʱ��Ƭ��0��ʾ��ʱ����ִ��һ��
 *  uiEnable: 4-->��ֹӦ�ö�ʱ������������������У��Ժ���Ҫ��ST_ControlTimer���ƶ�ʱ������
 *  : 5-->����Ӧ�ö�ʱ�������������������
 *  ����ֵ:0x00------->�����ɹ�
 */
unsigned int ST_ResetAppTimer(int hOSHandle, void (*ExpirationRoutine)(int), unsigned long ulInitialTime, unsigned long ulRescheduleTime, unsigned int uiEnable)
{
	int timer_item;

	if (uiEnable < 4 || uiEnable > 5)
		return 0xFFFFFFFB;
	if (ulInitialTime == 0)
		return 0xFFFFFFF3;

	for (timer_item = 0; timer_item < TIMERNUM; timer_item++)
	{
		if (hapi_timer[timer_item].id == hOSHandle)
			break;
	}

	memset(&hapi_timer[timer_item].value, 0, sizeof(&hapi_timer[timer_item].value));
	hapi_timer[timer_item].value.it_value.tv_sec = 0;
	hapi_timer[timer_item].value.it_value.tv_usec = 10000 * ulInitialTime;
	hapi_timer[timer_item].value.it_interval.tv_sec = 0;
	hapi_timer[timer_item].value.it_interval.tv_usec = ulRescheduleTime;

	if (uiEnable == 4)
	{
		setitimer(ITIMER_REAL, &hapi_timer[timer_item].value, 0);
		signal(SIGVTALRM, ExpirationRoutine);
	}
	else
		signal(SIGVTALRM, ExpirationRoutine);

	return 0;

}

