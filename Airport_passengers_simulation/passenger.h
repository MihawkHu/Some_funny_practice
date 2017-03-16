/*
	This including file description all the passenger's information and
we can generator all kinds of passengers with the generation function,
and there are many parameters that we can change easily such as the
VIP ratio, the duration all the passengers arrive and so on.
	All the variables and functions are fully annotated so they are
easy for you to understand.
	With this and mian.cpp, you can easily run our model!
*/
#pragma once
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cmath>
#include<string>
#include<ctime>
#include<algorithm>
using namespace std;

#define VIPratio 50 //the VIP ratio, this means 50%
#define during 7200 //How long we observe
#define stallratio 0 //The ratio of regular passenger who spend 
					//3mins to pass the check
class passenger
{
public:
	//general information
	bool VIP; //VIP 1 otherwise 0
	bool sex; //male 1 female 0
	int age; //the age
	
	char* name; //passenger's name
	char* nation; //passenger's nation
	char* religion; //passenger's religion
	int lasttime; //last time his clothchanging time

	//security information
	float security;
	
	//stage information
	int stage; //0 uncome, 1 in the outqueue, 
	//2 in the inner queue 3 checking 4 retriving things 5 dzone
	
	//outqueue information
	int arrivalTime;
	int registTime; //differ between VIP and reguler 
	int conRegistTime;

	//innerqueue information
	int clothChangeTime;  //true cloth changing time
	int PclothChangeTime; //predicted cloth changing time
	int clothusingTime; //have used how much time
	bool Changing; //whether begin the cloth changing

	//checking information
	int checktime;
	int conCheckTime;

	//wearing clothes
	int clothwearing; //time for wearing clothes
	bool clothwait;  //too crowdy and waits for wearing 
					//clothes,0 for dont wait 1 for wait
	int conClothwearing;

	//finish time
	int leavetime;
	bool finished;

	passenger()
	{
		static bool flag = 1;
		if (flag) 
		{
			//srand(time(0)); 
			flag = 0;
		}
		//general information
		VIP = (RangeRand(1,10)<=(VIPratio/10))?1:0;
		sex = 1;
		age = 18;
		name = "LI";
		nation = "China";
		religion = "none";
		lasttime = (VIP) ? 400 : 600;
		
		//security information
		security = 1;
		
		//stage information
		stage = 0;
		
		//outqueue information
		arrivalTime = rand() % during;
		registTime = (VIP)?RangeRand(7,11):RangeRand(11,15);
		conRegistTime = registTime;

		//innerqueue information
		clothChangeTime=(VIP)? RangeRand(2, 7):RangeRand(20,50);
		PclothChangeTime = clothChangeTime + 
			RangeRand(-clothChangeTime / 10, clothChangeTime / 10);
		Changing = 0;
		clothusingTime = 0;

		//checking information
		checktime = 2;
		if (RangeRand(1, 10) <= 2) checktime = 8;
		if (VIP == 0 && 
			(arrivalTime % 100) < stallratio) checktime = 180;
		conCheckTime = checktime;
		
		
		//clothes wearing
		clothwearing = int(GRandom(24, 14));
		clothwait = 0;
		conClothwearing = clothwearing;
		//finish time
		leavetime = 0;
		finished = 0;
	}
	~passenger()
	{}
	void print1(ostream &os)
	{
		os << arrivalTime << ' ' << VIP<<' '
			<<registTime<<' '<<checktime<< endl;
	}

//Gaussian distribution Date generate E is the expection V is variation
double GRandom(float E, float V) 
{
		static double V1, V2, S;
		static int phase = 0;
		double X;

		if (phase == 0) {
			do {
				double U1 = (double)rand() / RAND_MAX;
				double U2 = (double)rand() / RAND_MAX;
				
				V1 = 2 * U1 - 1;
				V2 = 2 * U2 - 1;
				S = V1 * V1 + V2 * V2;
			} while (S >= 1 || S == 0);

			X = V1 * sqrt(-2 * log(S) / S);
		}
		else
			X = V2 * sqrt(-2 * log(S) / S);

		phase = 1 - phase;
		X = X*V + E;
		if (X < 1) X = 1;
		return X;
}

//Uniform Distribution generate date from l to r(including l and r) evenly
int RangeRand(int l, int r) 
{
	return l + rand() % (r - l + 1);
}
};
