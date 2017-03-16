/*
	This program is for ICM-D-2017
	IMPORTANT:How to use dynamic scheduling or greedy scheduling(people 
	select shortest queue):
		1.Use Ctrl+F find Modify tag
		2.Annotate the one you dont need
		The dynamic scheduling will be significantly better ONLY when 
		regNUM is larger than 15 and normNum is larger than 8. This is
		because otherwise the ID check will be the bottleneck.
	  To simulate our HA and SA, we design a pipeline model with dynamic 
	scheduling. 
	  The pipeline model have 4 main stages to process the passengers' 
	arrival,the ID check, the X-ray check and retrieving things.
	  There are also many flexible parameters we can change such as 
	the number of staff who handle ID check, the number of X-ray 
	machines, the density of passengers and we can change the 
	proportion of VIP resources and regular passengers' resources 
	in each stage.
*/
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cmath>
#include<string>
#include<ctime>
#include<algorithm>
#include "passenger.h"
using namespace std;

int RangeRand(int l, int r)
{
	return l + rand() % (r - l + 1);
}

#define Xray 8	 //how many Xray machines
#define regX 3   //where the regular X-ray machine begin(from regX to 
				 //Xray) before these are X-ray for VIP
#define regNum 7 //how many ID check registers
#define normNum 4 //where the regular register begin(from normNum to 
				  //regNum) before these are register for VIP
#define pasNum 4800 //how many passengers in the duration which can 
					//set in passenger.h
#define clothcap 4//how many people simultaneously change clothes
#define CCAP 8 //how many people can simultaneously retrieve 
				//their personal things

passenger allpas[10000]; //all the passengers
int n;	//not used
int nowTime = 0; //At which time we are now
int queuelen = 0, queuelenVIP = 0, queuelenREG = 0;  
//the length of the queue in front of X-ray machine

struct
{
	int x[10000];
	int front;
	int rear;
	int waitload;
	bool VIP;
} outQueue[50] = { 0 }; //the queue in front of the ID check

struct 
{
	int x[10000];
	int front;
	int rear;
	int waitload;
	bool VIP;
} innerQueue[50] = { 0 }; //the queue in front of the X-ray

struct
{
	int x[10000];
	int front;
	int rear;
	int cap;
} Czone[50] = { 0 }; 
//The space where passengers retrive their personal things. 
//Not actual queue .

int min2(int x, int y) { return (x < y) ? x : y; } //minimum of 2

int findMinOutQueue(int VIP) //find the shortest ID check queue
{
	int left = (VIP) ? 1 : normNum;
	int right = (VIP) ? normNum - 1 : regNum;
	int Min = 1000000;
	int ans = 0;
	for (int i = left; i <= right; ++i)
		if (outQueue[i].rear- outQueue[i].front < Min)
		{
			Min = outQueue[i].rear - outQueue[i].front;
			ans = i;
		}
	return ans;
}

//A passenger arrive and select the shortest ID check queue.
void addOutQueue(int i) 
{
	int x = findMinOutQueue(allpas[i].VIP);
	outQueue[x].rear++;
	outQueue[x].x[outQueue[x].rear] = i;
}

//Select the best queue to have a X-ray check by greedy algorithm
int findMinInqueueGreedy(int vip, int preTime, int loc)
{
	int left = (vip) ? 1 : regX;
	int right = (vip) ? regX - 1 : Xray;
	int Min = 1000000;
	int ans = (vip) ? 1 : Xray;
	for (int i = left; i <= right; ++i)
		if (innerQueue[i].rear - innerQueue[i].front < Min)
		{
			Min = innerQueue[i].rear - innerQueue[i].front;
			ans = i;
		}
	return ans;
}

//Select the most suitable queue to have a X-ray 
//check by dynamic shceduling.
int findMinInqueueDynamicScheduling(int vip,int preTime,int loc)
{
	int left=(vip)?1:regX;
	int right = Xray;
	int Min = 10000000;
	int ans = (vip)?1:Xray;

	int Max = -1;
	for (int i = left; i <= right; ++i)
		if (innerQueue[i].waitload<=preTime && 
			innerQueue[i].waitload>Max)
		{
			Max = innerQueue[i].waitload;
			ans = i;
		}
	if (Max >= 0) return ans;
	ans = (vip) ? 1 : Xray;
	for (int i = left; i <= right; ++i)
		if (innerQueue[i].waitload < Min)
		{
			Min = innerQueue[i].waitload;
			ans = i;
		}
	return ans;
}

//When a pssenger pass the ID check, schedule him/her to next queue
void addInnerQueue(int i)	
{
	int x;
	x = findMinInqueueDynamicScheduling(
		allpas[i].VIP,allpas[i].PclothChangeTime,i); //Modify tag

	//x = findMinInqueueGreedy(
	//	allpas[i].VIP, allpas[i].PclothChangeTime, i);//Modify tag

	innerQueue[x].rear++;
	innerQueue[x].x[innerQueue[x].rear] = i;
}

int num[10] = { 0 };

//Wait to retrieve their personal things
void addRetrieveZone(int loc,int i)
{
	num[i]++;
	Czone[i].rear++;
	Czone[i].x[Czone[i].rear] = loc;
	if (Czone[i].cap >=CCAP)
		allpas[loc].clothwait = 1;
	else 
		Czone[i].cap++;
}

//Simulate the process of passenger's arrivals
void arrive()
{
	for (int i = 0; i < pasNum; ++i)
		if (allpas[i].arrivalTime == nowTime)
		{
			allpas[i].stage = 1;
			addOutQueue(i);
		}
}

//Simulate the process of ID checkers' ID checking work
void regist()
{
	for (int i = 1; i <= regNum; ++i)
		if (outQueue[i].front < outQueue[i].rear)
		{
			int loc = outQueue[i].x[outQueue[i].front + 1];
			allpas[loc].registTime -= 1;
			if (allpas[loc].registTime <= 0)
			{
				allpas[loc].stage = 2;
				addInnerQueue(loc);
				outQueue[i].front++;
			}
		}
}

//Simulate the X-ray check and the wave check
void handlequeue()
{
	int loc;
	int x = 0;
	for (int i = 1; i <= Xray; ++i)
	{
		int ad = innerQueue[i].rear - innerQueue[i].front;
		queuelen += ad;
		if (i < regX) queuelenVIP += ad; else queuelenREG += ad;
		for (int j = innerQueue[i].front + 1; j <= min2(
			innerQueue[i].rear, 
			innerQueue[i].front + clothcap); ++j)
		{
			loc = innerQueue[i].x[j];
			if (allpas[loc].Changing == 0) allpas[loc].Changing = 1;
			allpas[loc].clothusingTime++;
		}
		if (innerQueue[i].rear > innerQueue[i].front)
		{
			loc = innerQueue[i].x[innerQueue[i].front + 1];
			if (allpas[loc].clothusingTime>=
				allpas[loc].clothChangeTime)
				if (allpas[loc].checktime > 0)
				{
					allpas[loc].checktime--;
					allpas[loc].stage = 3;
				}
				else
				{
					allpas[loc].stage = 4;
					addRetrieveZone(loc,i);
					innerQueue[i].front++;
				}
		}
	}
}

void Retrive() //Simulate passenger trying to retrieve their things
{
	int x;
	for (int i = 1; i <= Xray; ++i)
	{
		for (int j = Czone[i].front + 1; j <= Czone[i].rear; ++j)
		{
			x = Czone[i].x[j];
			if (!allpas[x].clothwait)
				if (allpas[x].clothwearing)
				{
					allpas[x].clothwearing--;
					if (allpas[x].clothwearing == 0)
					{
						allpas[x].leavetime = nowTime;
						allpas[x].finished = 1;
						allpas[x].stage = 5;
						Czone[i].cap--;
					}
				}
		}
		x = Czone[i].x[Czone[i].front + 1];
		while (Czone[i].cap && allpas[x].clothwearing == 0)
		{
			Czone[i].front++;
			x = Czone[i].x[Czone[i].front + 1];
		}
		for (int j = Czone[i].front + 1; j <= Czone[i].rear; ++j)
		{
			x = Czone[i].x[j];
			if (allpas[x].clothwait &&Czone[i].cap<CCAP)
			{
				allpas[x].clothwait = 0;
				Czone[i].cap++;
			}
		}
	}
}

void init() //If you want a different passenger's list, use this
{
	//srand((int)time(0));
}

//Important part of dynamic scheduling, calculate 
//the waitTime in each X-ray queue
void waitloadUpdate()
{
	int nowload = 0;
	int loc;
	for (int i = 1; i <= Xray; ++i)
	{
		nowload = 0;
		if (innerQueue[i].front < innerQueue[i].rear)
		{
			loc = innerQueue[i].x[innerQueue[i].front + 1];
			if (allpas[loc].stage == 3) nowload = 3;
			if (allpas[loc].clothChangeTime < 
				allpas[loc].PclothChangeTime)
				nowload += allpas[loc].PclothChangeTime 
				- allpas[loc].clothChangeTime;
		}
		for (int j = innerQueue[i].front + 2; 
		j <= innerQueue[i].rear; ++j)
		{
			loc = innerQueue[i].x[j];
			if (allpas[loc].PclothChangeTime > nowload)
				nowload += (allpas[loc].PclothChangeTime - nowload);
			nowload += 3;
		}
		innerQueue[i].waitload = nowload;
	}
}

//write out the answer including all kinds of waiting time, 
//queue length and so on
void answeroutput()
{
	int numVIP = 0, numREG = 0, num = 0;
	float ansVIP = 0, ansREG = 0, ans = 0;
	float wasteVIP = 0, wasteREG = 0, waste = 0;
	int throughput = 0;
	int thewaste = 0;
	for (int i = 0; i < pasNum; ++i)
	{
	if (allpas[i].leavetime >= 1800 && allpas[i].leavetime <= 5400)
	throughput++;
	if (allpas[i].arrivalTime >= 1800 && 
		allpas[i].arrivalTime <= 5400)
	{
		thewaste = allpas[i].leavetime - allpas[i].arrivalTime+3 
			- allpas[i].conRegistTime - allpas[i].clothChangeTime 
			- allpas[i].conCheckTime - allpas[i].conClothwearing;
		ans += allpas[i].leavetime - allpas[i].arrivalTime;
		waste += thewaste;
		num++;
		if (allpas[i].VIP)
		{
			ansVIP += allpas[i].leavetime - allpas[i].arrivalTime;
			wasteVIP += thewaste;
			numVIP++;
		}
		else
		{
			ansREG += allpas[i].leavetime - allpas[i].arrivalTime;
			wasteREG += thewaste;
			numREG++;
		}
	}
	}
	ansVIP /= numVIP;
	wasteVIP /= numVIP;

	ansREG /= numREG;
	wasteREG /= numREG;

	ans /= num;
	waste /= num;
	cout << "The last one leave at:\t" << nowTime 
		<< " seconds" << endl;
	cout << "Total number is: \t" << num << 
		" Total average using time is: \t" << ans << ' ' <<
		"Total average waste time is: \t" << waste << endl;
	cout << "VIP number is: \t\t" << numVIP
		<< " VIP average using time is: \t" << ansVIP << ' '
		<< "VIP average waste time is: \t" << wasteVIP << endl;
	cout << "Regular number is: \t" << numREG 
		<< " Regular average using time is: \t" << ansREG 
		<< ' ' << "Regular average waste time is: \t" 
		<< wasteREG << endl;
	cout << "Average all queue length:\t" 
		<< (((float)queuelen / nowTime) / Xray) << endl;
	cout << "Average VIP queue length:\t" 
		<< (((float)queuelenVIP / nowTime) / (regX - 1)) << endl;
	cout << "Average Regular queue length:\t" 
	<< (((float)queuelenREG / nowTime) / (Xray - regX + 1)) << endl;
	cout << "Throughput:\t" << throughput << endl;
}

bool check() //check whether all passengers are finished
{
	for (int i = 0; i < pasNum; ++i)
		if (allpas[i].finished == 0)
			return 0;
	return 1;
}

int main()
{
	init();
	for (nowTime = 0;!check(); nowTime++) //simulate all process
	{
		waitloadUpdate();
		arrive(); //ok
		regist();
		handlequeue();
		Retrive();
	}
	answeroutput();
	return 0;
}


