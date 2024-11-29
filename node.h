#pragma once
#pragma once
#include"basic.h"
#include"samples.h"
struct InputNode
{
	long double value;
};
struct HidenNode
{
	long double value;
	long double ihweight[MaxInputNum];
	long double hbias;
	long double c_ihweight[MaxInputNum];
	long double c_hbias;
};
struct OutputNode
{
	long double value;
	//long double expvalue;
	long double RightValue;
	long double howeight[MaxHidenNum];
	long double obias;
	long double c_howeight[MaxHidenNum];
	long double c_obias;
};
class NeuralNetwork
{
public:
	NeuralNetwork();
	~NeuralNetwork();
	void ForwardSpread();
	void BackwardSpread();
	void AdjustingParameters();
	void TrainingNeuralNetwork();
	void TestingNeuralNetwork();
	void LoadingSamplesToNeuralNetwork(unsigned int k);
	void PreprocessingData(SampleNode* ptr);
	long double GetDeviation() const;
	void LoadParameterToTxt() const;
	void ReadParameterFromTxt() const;
	int GetFinalAns() const;
private:
	InputNode* inputnode[MaxInputNum];
	HidenNode* hidennode[MaxHidenNum];
	OutputNode* outputnode[MaxOutputNum];
	SampleNode* SampleHeadPtr;
	Sample sample;
};

 