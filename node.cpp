#include"node.h"
long double get_rand()
{
	static bool first = true;
	if (first)
	{
		srand((unsigned long)time(NULL));
		first = false;
	}
	long double t = (2.0 * (long double)rand() / RAND_MAX) - 1;
	return t;
}
inline long double ReLU(long double x)
{
	if (x >= 0)
	{
		return x;
	}
	return 0;
}
void NeuralNetwork::PreprocessingData(SampleNode* ptr)
{
	for (int i = 0; i < MaxInputNum; i++)
	{
		inputnode[i]->value = (long double)ptr->sample[i];
	}
	for (int i = 0; i < MaxOutputNum; i++)
	{
		if (i == (int)ptr->target)
		{
			outputnode[i]->RightValue = 1.0;
		}
		else
		{
			outputnode[i]->RightValue = 0.0;
		}
	}
}
int NeuralNetwork::GetFinalAns() const
{
	int rightans = 0;
	for (int i = 1; i < MaxOutputNum; i++)
	{
		if (outputnode[i]->value > outputnode[rightans]->value)
		{
			rightans = i;
		}
	}
	return rightans;
}
NeuralNetwork::NeuralNetwork()
{
	sample.TestingSamples();
	sample.SafeDataToAFile();
	sample.ReadDataFromFile();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
	std::cout << "initing network..." << std::endl;
	for (int i = 0; i < MaxInputNum; i++)
	{
		inputnode[i] = (InputNode*)new InputNode();
		inputnode[i]->value = 0;
	}
	for (int i = 0; i < MaxHidenNum; i++)
	{
		hidennode[i] = (HidenNode*)new HidenNode();
		hidennode[i]->hbias = get_rand();
		hidennode[i]->c_hbias = 0;
		hidennode[i]->value = 0;
		for (int j = 0; j < MaxInputNum; j++)
		{
			hidennode[i]->ihweight[j] = get_rand();
			hidennode[i]->c_ihweight[j] = 0;
		}
	}
	for (int i = 0; i < MaxOutputNum; i++)
	{
		outputnode[i] = (OutputNode*)new OutputNode();
		outputnode[i]->obias = get_rand();
		outputnode[i]->c_obias = 0;
		outputnode[i]->value = 0;
		outputnode[i]->RightValue = 0;
		for (int j = 0; j < MaxHidenNum; j++)
		{
			outputnode[i]->howeight[j] = get_rand();
			outputnode[i]->c_howeight[j] = 0;
		}
	}
	SampleHeadPtr = sample.GetHead();
	//Sleep(SleepDuration);
	std::cout << "initing netwotk done" << std::endl;
	TestingNeuralNetwork();
}
NeuralNetwork::~NeuralNetwork()
{
	for (int i = 0; i < MaxInputNum; i++)
	{
		delete(inputnode[i]);
	}
	for (int i = 0; i < MaxOutputNum; i++)
	{
		delete(outputnode[i]);
	}
	for (int i = 0; i < MaxHidenNum; i++)
	{
		delete(hidennode[i]);
	}
	std::cout << "delete network done..." << std::endl;
	//Sleep(SleepDuration);
}
void ShowProgress(int i, unsigned long SampleNum)
{
	for (int j = 1; j < 100; j++)
	{
		if (i == SampleNum * j / 100)
		{
			std::cout << "loading/training/testing progress: " << j << "%..." << std::endl;
			return;
		}
	}
}
void NeuralNetwork::TrainingNeuralNetwork()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED);
	ReadParameterFromTxt();
	std::cout << "do you want to train the neural network?" << std::endl << "1.yes 2.no" << std::endl;
	int choice;
	std::cin >> choice;
	while (choice != 1 && choice != 2)
	{
		std::cout << "wrong input.input again" << std::endl;
		std::cin >> choice;
	}
	if (choice == 2)
	{
		std::cout << "skip training " << std::endl;
		return;
	}
	std::cout << "training network now" << std::endl;
	SampleNode* ptr = SampleHeadPtr;
	long double BeforeError = 0.0;
	long double BeforeCorrectRate = 0;
	for (unsigned long count = 1; count <= sample.GetNum(); count++)
	{
		if (ptr == nullptr)
		{
			break;
		}
		PreprocessingData(ptr);
		ForwardSpread();
		BeforeError += GetDeviation();
		int rightans = GetFinalAns();
		if (rightans == (int)ptr->target)
		{
			BeforeCorrectRate++;
		}
		ptr = ptr->next;
		ShowProgress(count, sample.GetNum());
	}
	BeforeError /= sample.GetNum();
	BeforeCorrectRate /= sample.GetNum();
	std::cout << "loading done" << std::endl;
	std::cout << "average deviation before training is: " << BeforeError << std::endl;
	std::cout << "correct rate before training is: " << BeforeCorrectRate << std::endl << std::endl;
	//Sleep(100000);
	while (1)
	{
		std::cout << "do you want to train ?" << std::endl << "1.yes 2.no" << std::endl;
		int choicing;
		std::cin >> choicing;
		while (choicing != 1 && choicing != 2)
		{
			std::cout << "wrong input.input again" << std::endl;
			std::cin >> choicing;
		}
		if (choicing == 2)
		{
			break;
		}
		for (int traningtime = 0; traningtime < TrainingTime; traningtime++)
		{
			//long double error = 0.0;
			ptr = SampleHeadPtr;
			std::cout << "the " << traningtime + 1 << " trainig..." << std::endl;
			for (unsigned long count = 1; count <= sample.GetNum(); count++)
			{
				if (ptr == nullptr)
				{
					break;
				}
				PreprocessingData(ptr);
				ForwardSpread();
				BackwardSpread();
				AdjustingParameters();
				//ForwardSpread();
				//error += GetDeviation();
				ptr = ptr->next;
				ShowProgress(count, sample.GetNum());
			}
		}
		std::ifstream TrainTimeInFile("traintime.txt");
		int train = 0;
		if (TrainTimeInFile.is_open())
		{
			TrainTimeInFile >> train;
		}
		train += TrainingTime;
		TrainTimeInFile.close();
		std::ofstream TrainTimeOuFile("traintime.txt");
		if (TrainTimeOuFile.is_open())
		{
			TrainTimeOuFile << train;
		}
		TrainTimeOuFile.close();
		ptr = SampleHeadPtr;
		long double AfterError = 0.0;
		long double AfterCorrectRate = 0;
		for (unsigned long count = 1; count <= sample.GetNum(); count++)
		{
			if (ptr == nullptr)
			{
				break;
			}
			PreprocessingData(ptr);
			ForwardSpread();
			AfterError += GetDeviation();
			int rightans = GetFinalAns();
			if (rightans == (int)ptr->target)
			{
				AfterCorrectRate++;
			}
			ptr = ptr->next;
			ShowProgress(count, sample.GetNum());
		}
		AfterError /= sample.GetNum();
		AfterCorrectRate /= sample.GetNum();
		std::cout << "average deviation after training is: " << AfterError << std::endl;
		std::cout << "correct rate after training is: " << AfterCorrectRate << std::endl << std::endl;
		if (AfterCorrectRate >= BeforeCorrectRate && AfterError <= BeforeError)
		{
			std::cout << " effective training.The growth rate of accuracy is: ";
			std::cout << (AfterCorrectRate - BeforeCorrectRate) / BeforeCorrectRate << std::endl;
			LoadParameterToTxt();
		}
		else
		{
			std::cout << " uneffective training.Change your learning rate." << std::endl;
			std::cout << "input string 'quit' if you want to quit." << std::endl;
			std::string s;
			std::cin >> s;
			if (s == "quit")
			{
				break;
			}
		}
	}
	std::cout << "training network done" << std::endl;
}
void NeuralNetwork::BackwardSpread()
{
	long double ODV[MaxOutputNum];
	for (int i = 0; i < MaxOutputNum; i++)
	{
		ODV[i] = 0;
	}
	for (int i = 0; i < MaxOutputNum; i++)
	{
		//long double final[MaxOutputNum];
		for (int j = 0; j < MaxOutputNum; j++)
		{
			if (j == i)
			{
				ODV[j] -=  outputnode[j]->value * (1 - outputnode[j]->value) * outputnode[j]->RightValue / outputnode[j]->value;
			}
			else
			{
				ODV[i] += outputnode[i]->value * outputnode[j]->value * outputnode[j]->RightValue / outputnode[j]->value;
			}
		}
		/*for (int j = 0; j < MaxOutputNum; j++)
		{
			ODV[i] += final[j];
		}*/
	}
	for (int i = 0; i < MaxOutputNum; i++)
	{
		outputnode[i]->c_obias = -StudyRate * ODV[i];
		for (int j = 0; j < MaxHidenNum; j++)
		{
			outputnode[i]->c_howeight[j] = -StudyRate * ODV[i] * hidennode[j]->value;
		}
	}
	long double HDV[MaxHidenNum];
	for (int j = 0; j < MaxHidenNum; j++)
	{
		HDV[j] = hidennode[j]->value * (1 - hidennode[j]->value);
	}
	/*for (int i = 0; i < MaxOutputNum; i++)
	{
		printf("ODV is: %Lf\n", ODV[i]);
	}
	for (int i = 0; i < MaxHidenNum; i++)
	{
		printf("HDV is: %Lf\n", HDV[i]);
	}
	Sleep(100000);*/
	for (int j = 0; j < MaxHidenNum; j++)
	{
		long double final = 0;
		for (int i = 0; i < MaxOutputNum; i++)
		{
			final += ODV[i] * HDV[j] * outputnode[i]->howeight[j];
		}
		hidennode[j]->c_hbias = -StudyRate * final;
		for (int k = 0; k < MaxInputNum; k++)
		{
			final = 0;
			for (int i = 0; i < MaxOutputNum; i++)
			{
				final += ODV[i] * HDV[j] * outputnode[i]->howeight[j] * inputnode[k]->value;
			}
			hidennode[j]->c_ihweight[k] = -StudyRate * final;
		}
	}
}
void NeuralNetwork::ForwardSpread()
{
	for (int i = 0; i < MaxHidenNum; i++)
	{
		hidennode[i]->value = 0;
		for (int j = 0; j < MaxInputNum; j++)
		{
			hidennode[i]->value += inputnode[j]->value * hidennode[i]->ihweight[j];
		}
		hidennode[i]->value += hidennode[i]->hbias;
		hidennode[i]->value = ReLU(hidennode[i]->value);
	}
	/*std::cout << "hi" << hidennode[1]->value << std::endl;
	long double q[MaxHidenNum];
	for (int i = 0; i < MaxHidenNum; i++)
	{
		q[i] = exp(hidennode[i]->value);
	}
	long double sum = 0;
	for (int i = 0; i < MaxHidenNum; i++)
	{
		sum += q[i];
	}
	std::cout << "sum is: " << sum <<" "<< q[1] << std::endl;
	for (int i = 0; i < MaxHidenNum; i++)
	{
		hidennode[i]->value = q[i] / sum;
	}*/
	for (int i = 0; i < MaxOutputNum; i++)
	{
		//long double orivalue = 0;
		outputnode[i]->value = 0;
		for (int j = 0; j < MaxHidenNum; j++)
		{
			outputnode[i]->value += outputnode[i]->howeight[j] * hidennode[j]->value;
		}
		outputnode[i]->value += outputnode[i]->obias;
		//outputnode[i]->value = sigmoid(outputnode[i]->value);
	}
	long double p[MaxOutputNum];
	for (int i = 0; i < MaxOutputNum; i++)
	{
		p[i] = exp(outputnode[i]->value);
	}
	long double sum = 0;
	for (int i = 0; i < MaxOutputNum; i++)
	{
		sum += p[i];
	}
	for (int i = 0; i < MaxOutputNum; i++)
	{
		outputnode[i]->value = p[i] / sum;
	}
}
void NeuralNetwork::AdjustingParameters()
{
	for (int i = 0; i < MaxHidenNum; i++)
	{
		hidennode[i]->hbias += hidennode[i]->c_hbias;
		for (int j = 0; j < MaxInputNum; j++)
		{
			hidennode[i]->ihweight[j] += hidennode[i]->c_ihweight[j];
		}
	}
	for (int i = 0; i < MaxOutputNum; i++)
	{
		outputnode[i]->obias += outputnode[i]->c_obias;
		for (int j = 0; j < MaxHidenNum; j++)
		{
			outputnode[i]->howeight[j] += outputnode[i]->c_howeight[j];
		}
	}
}
void NeuralNetwork::LoadingSamplesToNeuralNetwork(unsigned int k)
{
	if (k < 1 || k > sample.GetNum())
	{
		std::cout << "unsafe loading sample operation..." << std::endl;
		return;
	}
	SampleNode* ptr = SampleHeadPtr;
	while (--k)
	{
		ptr = ptr->next;
	}
	PreprocessingData(ptr);
}
void NeuralNetwork::TestingNeuralNetwork()
{
	int choice = 0;
	while (1)
	{
		std::cout << "do you want to test the neural network?" << std::endl << "1.yes 2.no" << std::endl;
		std::cin >> choice;
		while (choice != 1 && choice != 2)
		{
			std::cout << "wrong input.input again" << std::endl;
			std::cin >> choice;
		}
		if (choice == 2)
		{
			std::cout << "skip testing " << std::endl;
			break;
		}
		std::cout << "testing network now.input your testing number" << std::endl;
		int k;
		std::cin >> k;
		if (k < 1 || k >(int)sample.GetNum())
		{
			std::cout << "unsafe input.cancel test process..." << std::endl;
			return;
		}
		LoadingSamplesToNeuralNetwork(k);
		ForwardSpread();
		for (int i = 0; i < MaxOutputNum; i++)
		{
			std::cout << "for number: " << i << "  ";
			std::cout << "outputvalue is: " << outputnode[i]->value << " ";
			std::cout << "right value is: " << outputnode[i]->RightValue << std::endl;
		}
		std::cout << "deviation is: " << GetDeviation() << std::endl ;
		/*BackwardSpread();
		AdjustingParameters();
		ForwardSpread();
		std::cout << "deviation after training is: " << GetDeviation() << std::endl;*/
	}
	std::cout << "finish testing" << std::endl << std::endl;
}
long double NeuralNetwork::GetDeviation() const
{
	long double ans = 0.0;
	for (int i = 0; i < MaxOutputNum; i++)
	{
		ans += outputnode[i]->RightValue * std::log(outputnode[i]->value);
	}
	return - ans;
}
void NeuralNetwork::LoadParameterToTxt() const
{
	std::cout << "do you want to save parameters to data?" << std::endl << "1.yes 2.no" << std::endl;
	int choice;
	std::cin >> choice;
	while (choice != 1 && choice != 2)
	{
		std::cout << "wrong input.input again" << std::endl;
		std::cin >> choice;
	}
	if (choice == 2)
	{
		std::cout << "skip saving " << std::endl;
		return;
	}
	char path[4][60] =
	{
		".\\parameter\\ihweight.txt",
		".\\parameter\\hbias.txt",
		".\\parameter\\obias.txt",
		".\\parameter\\howeight.txt"
	};
	for (int i = 0; i < 4; i++)
	{
		std::ofstream filename(path[i]);
		if (filename.is_open())
		{
			switch (i)
			{
			case 0:
				for (int j = 0; j < MaxHidenNum; j++)
				{
					for (int k = 0; k < MaxInputNum; k++)
					{
						filename << hidennode[j]->ihweight[k] << " ";
					}
				}
				break;
			case 1:
				for (int j = 0; j < MaxHidenNum; j++)
				{
					filename << hidennode[j]->hbias << " ";
				}
				break;
			case 2:
				for (int j = 0; j < MaxOutputNum; j++)
				{
					filename << outputnode[j]->obias << " ";
				}
				break;
			case 3:
				for (int j = 0; j < MaxOutputNum; j++)
				{
					for (int k = 0; k < MaxHidenNum; k++)
					{
						filename << outputnode[j]->howeight[k] << " ";
					}
				}
				break;
			default:
				break;
			}
		}
		filename.close();
	}
}
void NeuralNetwork::ReadParameterFromTxt() const
{
	std::cout << "do you want to load parameters from data?" << std::endl << "1.yes 2.no" << std::endl;
	int choice;
	std::cin >> choice;
	while (choice != 1 && choice != 2)
	{
		std::cout << "wrong input.input again" << std::endl;
		std::cin >> choice;
	}
	if (choice == 2)
	{
		std::cout << "skip loading " << std::endl;
		return;
	}
	char path[4][60] =
	{
		".\\parameter\\ihweight.txt",
		".\\parameter\\hbias.txt",
		".\\parameter\\obias.txt",
		".\\parameter\\howeight.txt"
	};
	for (int i = 0; i < 4; i++)
	{
		std::ifstream filename(path[i]);
		if (filename.is_open())
		{
			switch (i)
			{
			case 0:
				for (int j = 0; j < MaxHidenNum; j++)
				{
					for (int k = 0; k < MaxInputNum; k++)
					{
						filename >> hidennode[j]->ihweight[k];
					}
				}
				break;
			case 1:
				for (int j = 0; j < MaxHidenNum; j++)
				{
					filename >> hidennode[j]->hbias;
				}
				break;
			case 2:
				for (int j = 0; j < MaxOutputNum; j++)
				{
					filename >> outputnode[j]->obias;
				}
				break;
			case 3:
				for (int j = 0; j < MaxOutputNum; j++)
				{
					for (int k = 0; k < MaxHidenNum; k++)
					{
						filename >> outputnode[j]->howeight[k];
					}
				}
				break;
			default:
				break;
			}
		}
		filename.close();
	}
}