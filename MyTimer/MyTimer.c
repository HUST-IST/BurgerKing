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
	int id;//TIMER号
	int data;//TIMER初始值
	int timer_flag;
} sep_timer;

#define STEP_TIMER   2
#define HEAD_TIMER  3
#define PFSTEP_TIMER  1
#define CLOCK_TIMER   4

/**
 * 函数功能：创建一个应用定时器，时间片为10ms的倍数。
 * 本函数指定了定时器第一次定时时间和以后每次定时的时间，
 * 如果在创建应用定时器后不是立即允许运行，则需要以后调用
 * 控制应用定时器函数来激活定时器的运行
 * pcName:不大于8字节字符串,应用定时器的名字
 * ExpirationRoutine:定时器时间到时的处理函数
 * ulID:定时器处理函数用到的参数，用来区分同一处理函数不同事件
 * ulInitialTime:指定定时器初始化时间片
 * ulRescheduleTime:指定在第一次时间片到后每次定时的时间片，0表示定时器仅执行一次
 * uiEnable:4-->禁止应用定时器在创建完后运行，以后需要用ST_ControlTimer控制定时器运行
 * :5-->允许应用定时器在创建完后运行
 * phOSHandle( 出口):	 创建的应用定时器句柄
 * 返回值:0x00------->创建成功
 */
int timer_item;
unsigned int ST_CreateAppTimer(const char *pcName, void (*ExpirationRoutine)(int), unsigned long ulInitialTime, unsigned long ulRescheduleTime, int uiEnable, int *phOSHandle)
{
	if (uiEnable < 4 || uiEnable > 5)
		return 0xFFFFFFFB;	// 禁止和允许参数错误

	if (ulInitialTime == 0)
		return 0xFFFFFFF3;	// 初始化时间片参数为0

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
 * 函数功能：删除指定的应用定时器
 * phOSHandle( 入口):定时器句柄，由创建定时器函数获得
 * phOSHandle( 出口):删除成功后被设置成EM_INVALID_HANDLE，失败则不改变原来值
 *  返回值:0x00------->操作成功
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
 * 函数功能：禁止或允许应用定时器运行,定时器是先前已经创建的
 * hOSHandle:指定应用定时器的句柄
 * uiEnable:---取值范围,EM_rtos_DISABLE_TIMER;实际值4,禁止应用定时器运行
 * ---取值范围,EM_rtos_ENABLE_TIMER;实际值5,允许应用定时器运行
 * 返回值:	0x00-->操作成功
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
 * 函数功能：重新设置定时器，包括定时器处理程序，首次和以后时间，是否允许立即运行
 * hOSHandle:定时器句柄，由创建定时器函数获得
 * ExpirationRoutine:定时器时间到时的处理函数
 * ulInitialTime:指定定时器初始化时间片
 * ulRescheduleTime:指定在第一次时间片到后每次定时的时间片，0表示定时器仅执行一次
 *  uiEnable: 4-->禁止应用定时器在重新设置完后运行，以后需要用ST_ControlTimer控制定时器运行
 *  : 5-->允许应用定时器在重新设置完后运行
 *  返回值:0x00------->创建成功
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

