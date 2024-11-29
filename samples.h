#pragma once
#pragma once
#include"basic.h"
struct SampleNode
{
	unsigned short sample[MaxInputNum];
	unsigned short target;
	SampleNode* next;
};
struct SampleRange
{
	int height;
	int width;
};
class Sample
{
public:
	Sample();
	~Sample();
	void pop(SampleNode s);
	SampleNode push();
	SampleNode* GetHead() const;
	SampleNode* GetLast() const;
	bool IsEmpty(SampleNode s);
	bool IsEmptySample();
	unsigned long GetNum() const;
	void LoadingSamples();
	//SampleNode GetSample(int number, int x, int y);
	void LoadingAllSamples();
	void LoadingSampleNum(int num);
	void TestingSamples();
	SampleNode GetCertainNum(int x, int y, DWORD* ptr, int num);
	void SafeDataToAFile();
	void ReadDataFromFile();
private:
	IMAGE images[10];
	//HDC hdc[10];
	DWORD* ImagePtr[10];
	SampleRange samplerange[10];
	SampleNode* headptr;
	SampleNode* lastptr;
	static unsigned long NumOfSamples;
};

