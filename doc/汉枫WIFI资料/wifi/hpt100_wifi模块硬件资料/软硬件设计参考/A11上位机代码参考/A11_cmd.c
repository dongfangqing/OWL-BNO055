#include "A11_cmd.h"

int connect_to_module();
int module_echo();

void connect_A11(HANDLE fd)
{
		if (connect_to_module(fd)== 1)
		{
			A11_connected= 1;
			if (module_echo(fd)== 1)
				module_send_cmd_set(fd, "AT+E");
		}
		else
			A11_connected= 0;
}

// 往模块下命令，并等待模块回+ok 确认
int module_send_cmd_set(HANDLE fd, char *cmd)
{
	int n;
	char ctmp[100];
	int i;

	sprintf(ctmp, "%s\n", cmd);
	//printf("send cmd: %s", ctmp);
	if (CommWrite(fd, ctmp, strlen(ctmp))==-1)
		return FALSE;
	i= 0;
	while (i< 40)
	{
		n= CommRead(fd, ctmp, 10);
		if ((ctmp[0]== 'A')&&(ctmp[1]== 'T')&&(ctmp[2]== '+')&&(ctmp[3]== 'E'))
		{
			n= CommRead(fd, ctmp, 10);
			return TRUE;
		}
		if ((ctmp[0]== '+')&&(ctmp[1]== 'o')&&(ctmp[2]== 'k'))
			return TRUE;
		i++;
		Sleep(200);
	}
	return FALSE;
}
// 往模块下查询命令，并返回查询结果在ret中
int module_send_cmd_get(HANDLE fd, char *cmd, char *ret)
{
	int n;
	char ctmp[100];
	int i;

	sprintf(ctmp, "%s\n", cmd);
	//printf("send cmd: %s", ctmp);
	if (CommWrite(strPCCom.fd, ctmp, strlen(ctmp))==-1)
		return FALSE;
	i= 0;
	while (i< 40)
	{
		n= CommRead(strPCCom.fd, ctmp, 100);
		ctmp[n-4]= '\0';
		//printf("module_send_cmd_get: %s\n", ctmp);
		if ((ctmp[0]== '+')&&(ctmp[1]== 'o')&&(ctmp[2]== 'k'))
		{
			sprintf(ret, "%s", &(ctmp[4]));
			return TRUE;
		}
		i++;
		Sleep(200);
	}
	return FALSE;
}

///////////////////////////////////////////
bool uart_read2clean(HANDLE fd, int sec)
{
	char c[50];
	int num;

	while (1)
	{
		num= CommRead(fd, c, 50);
		//printf("num= %d\n", num);
		if (num== -1)
			return FALSE;
		if (num!= 0)
			Sleep(sec*1000);
		else
			break;
	}
	return TRUE;
}

bool uart_write_wait(HANDLE fd, char *buf, int len, int sec)
{
	if (CommWrite(fd, buf, len)==-1)
		return FALSE;
	if (sec!= 0)
		Sleep(sec*1000);
	return TRUE;
}

bool trying_to_onnect(HANDLE fd)
{
	char c[20];
	int num;
	int trying= 0;

	// 串口刚设置完后，可能有些乱码，先读清掉
	if (uart_read2clean(fd, 1)== FALSE)
		return FALSE;
	
	while (1)
	{
		if(uart_write_wait(fd, "+++", 3, 0)== FALSE)
			return FALSE;
		num= CommRead(fd, c, 5);
		if (num== -1)
			return FALSE;
		if (num== 0)
		{
			Sleep(1000);
			trying++;
			if (trying>= 3)
				return FALSE;
			continue;
		}
		if (c[0]== 'a')
		{
			if(uart_write_wait(fd, "a", 1, 0)== FALSE)
				return FALSE;
			num= CommRead(fd, c, 10);
			if (num== -1)
				return FALSE;
			if ((num== 7)&&(c[0]=='+')&&(c[1]=='o')&&(c[2]=='k'))
			{
				//printf("OK!\n");
				return TRUE;
			}
			else
				return FALSE;
		}
		else
			return FALSE;
	}
	return FALSE;
}

int connect_to_module(HANDLE fd)
{
	gboolean ret;
	char tmp[20];
	int num, i;

	i= 0;
	printf("Connect to Module...");
	while (i< 2)
	{
		printf("...");
		CommWrite(fd, "AT+\n", 4);
		num= CommRead(fd, tmp, 20);
		tmp[num]= '\0';
		//printf("connect_to_module...%s\n", tmp);
		// 如果显示AT+ 或+ok : 则模块已进入命令模式
		if (((tmp[0]== 'A')&&(tmp[1]== 'T')&&(tmp[2]== '+'))||((tmp[0]== '+')&&(tmp[1]== 'o')&&(tmp[2]== 'k')))
		{
			printf("OK!\n");
			return 1;
		}
		i++;
	}
	// 否则发"+++" 进入命令模式
	ret= trying_to_onnect(fd);
	if (ret== TRUE)
	{
		printf("OK!\n");
		return 1;
	}
	printf("Fialed!\n");
	return 0;
}

// 测试模块是否回显命令: AT+E
int module_echo(HANDLE fd)
{
	int n;
	char ctmp[25];

	if (CommWrite(fd, "AT+\n", 4)==-1)
		return FALSE;
	n= CommRead(fd, ctmp, 25);
	if (ctmp[0]== 'A')
		return 1;
	else
		return 0;
}

