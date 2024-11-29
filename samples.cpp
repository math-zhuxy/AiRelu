#include"samples.h"
unsigned long Sample::NumOfSamples = 0;
SampleNode* Sample::GetHead() const
{
	return headptr;
}
SampleNode* Sample::GetLast() const
{
	return lastptr;
}
unsigned long Sample::GetNum() const
{
	return NumOfSamples;
}
void Sample::pop(SampleNode s)
{
	if (IsEmpty(s))
	{
		return;
	}
	SampleNode* node;
	if (NumOfSamples == 1)
	{
		node = headptr;
	}
	else
	{
		node = (SampleNode*)new SampleNode();
		this->lastptr->next = node;
	}
	for (int i = 0; i < MaxInputNum; i++)
	{
		node->sample[i] = s.sample[i];
	}
	node->target = s.target;
	this->lastptr = node;
	node->next = nullptr;
	NumOfSamples++;
}
SampleNode Sample::push()
{
	if (IsEmptySample())
	{
		SampleNode s;
		memset(s.sample, 0, sizeof(unsigned short));
		s.target = -1;
		s.next = nullptr;
		std::cout << "sample is empty now.Unsafe operations " << std::endl;
		return s;
	}
	SampleNode* newhead = headptr->next;
	SampleNode s;
	for (int i = 0; i < MaxInputNum; i++)
	{
		s.sample[i] = headptr->sample[i];
	}
	s.target = headptr->target;
	s.next = nullptr;
	delete(headptr);
	headptr = newhead;
	NumOfSamples--;
	return s;
}
bool Sample::IsEmptySample()
{
	if (NumOfSamples == 0)
	{
		return true;
	}
	return false;
}
Sample::Sample()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	LoadingSamples();
	this->headptr = (SampleNode*)new SampleNode();
	this->lastptr = this->headptr;
	this->headptr->next = nullptr;
	NumOfSamples++;
	std::cout << "create sample done" << std::endl;
	LoadingAllSamples();
}
Sample::~Sample()
{
	SampleNode* cur = headptr;
	SampleNode* nextptr = headptr->next;
	int count = 0;
	while (1)
	{
		delete(cur);
		cur = nextptr;
		if (cur == nullptr)
		{
			break;
		}
		nextptr = nextptr->next;
		count++;
		if (count == NumOfSamples / 2)
		{
			std::cout << "delete sample 50% done" << std::endl;
		}
	}
	std::cout << "delete sample done" << std::endl;
	//Sleep(SleepDuration);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
}
bool Sample::IsEmpty(SampleNode s)
{
	bool test = true;
	int num = 0;
	for (int i = 0; i < MaxInputNum; i++)
	{
		int m = s.sample[i];
		if (m == 0)
		{
			num++;
		}
	}
	if (num >= 28 * 28 - 10)
	{
		return true;
	}
	return false;
}
void Sample::LoadingSamples()
{
	char pathname[60];
	for (int i = 0; i < 10; i++)
	{
		sprintf_s(pathname, ".\\samples\\mnist_train%d.jpg", i);
		loadimage(&images[i], pathname);
		//hdc[i] = GetImageHDC(&images[i]);
		ImagePtr[i] = GetImageBuffer(&images[i]);
		samplerange[i].height = images[i].getheight();
		samplerange[i].width = images[i].getwidth();
	}
}
void Sample::TestingSamples()
{
	std::cout << "do you want to testing samples?" << std::endl << "1.yes 2.no" << std::endl;
	int choice;
	std::cin >> choice;
	while (choice != 1 && choice != 2)
	{
		std::cout << "wrong input.input again" << std::endl;
		std::cin >> choice;
	}
	if (choice == 2)
	{
		std::cout << "skip testing " << std::endl;
		return;
	}
	std::cout << "testing samples now.choosing your number" << std::endl;
	unsigned int k;
	std::cin >> k;
	if (k > NumOfSamples || k < 1)
	{
		std::cout << "error input.cancel testing progress..." << std::endl;
		return;
	}
	SampleNode* ptr = headptr;
	while (--k)
	{
		ptr = ptr->next;
	}
	for (int i = 0; i < 28; i++)
	{
		for (int j = 0; j < 28; j++)
		{
			printf("%3d ", ptr->sample[i * 28 + j]);
		}
		std::cout << std::endl << std::endl;
	}
	std::cout << "target is: " << (int)ptr->target << std::endl << std::endl;
	//Sleep(SleepDuration);
}
SampleNode Sample::GetCertainNum(int x, int y, DWORD* ptr, int num)
{
	DWORD* origin = ptr;
	SampleNode s;
	if (ptr == nullptr)
	{
		memset(s.sample, 0, sizeof(unsigned short));
		s.target = 0;
		s.next = nullptr;
		return s;
	}
	s.target = num;
	s.next = nullptr;
	unsigned int move = y * 28 * samplerange[num].width + x * 28;
	ptr += move;
	int i = 0, j = 0;
	while (1)
	{
		*ptr = BGR(*ptr);
		unsigned short r = GetRValue(*ptr);
		unsigned short g = GetGValue(*ptr);
		unsigned short b = GetBValue(*ptr);
		unsigned short gray = (30 * r + 59 * g + 11 * b) / 100;
		if (gray < GrayscaleThreshold)
		{
			gray = 0;
		}
		s.sample[i + j * 28] = gray;
		ptr++;
		i++;
		if (i == 28)
		{
			ptr += samplerange[num].width - 28;
			i = 0;
			j++;
		}
		if (j == 28)
		{
			break;
		}
	}
	ptr = origin;
	return s;
}
void Sample::LoadingSampleNum(int num)
{
	int Height = samplerange[num].height;
	int Width = samplerange[num].width;
	for (int x = 0; x < Width / 28; x++)
	{
		for (int y = 0; y < Height / 28; y++)
		{
			pop(GetCertainNum(x, y, ImagePtr[num], num));
		}
	}
}
void Sample::LoadingAllSamples()
{
	for (int number = 0; number < 10; number++)
	{
		LoadingSampleNum(number);
	}
	NumOfSamples--;
	std::cout << "loading all samples done" << std::endl;
	//Sleep(SleepDuration);
	std::cout << "total sample amount is " << NumOfSamples << std::endl;
	//Sleep(SleepDuration);
}
void Sample::SafeDataToAFile()
{
	std::cout << "do you want to update sample data?" << std::endl << "1.yes 2.no" << std::endl;
	int choice;
	std::cin >> choice;
	while (choice != 1 && choice != 2)
	{
		std::cout << "wrong input.input again" << std::endl;
		std::cin >> choice;
	}
	if (choice == 2)
	{
		std::cout << "skip updating" << std::endl;
		return;
	}
	std::cout << "saving sample data..." << std::endl;
	SampleNode* ptr = headptr;
	for (int num = 0; num < 10; num++)
	{
		char PathName[60];
		sprintf_s(PathName, ".\\samples\\sample%d.txt", num);
		std::ofstream OutputFile(PathName);
		if (OutputFile.is_open())
		{
			while (ptr != nullptr && num == ptr->target)
			{
				OutputFile << ptr->target << " ";
				for (int i = 0; i < MaxInputNum; i++)
				{
					OutputFile << ptr->sample[i] << " ";
				}
				ptr = ptr->next;
			}
		}
		std::cout << "saving sample " << num << " done..." << std::endl;
		OutputFile.close();
	}
	std::cout << "saving sample data done" << std::endl;
}
void Sample::ReadDataFromFile()
{
	std::cout << "do you want to test the stored sample data?" << std::endl << "1.yes 2.no" << std::endl;
	int choice;
	std::cin >> choice;
	while (choice != 1 && choice != 2)
	{
		std::cout << "wrong input.input again" << std::endl;
		std::cin >> choice;
	}
	if (choice == 2)
	{
		std::cout << "skip testing " << std::endl;
		return;
	}
	std::cout << "reading data now" << std::endl;
	std::cout << "select the file you want to test" << std::endl;
	int num;
	std::cin >> num;
	if (num < 0 || num>9)
	{
		std::cout << "unsafety operation.End testing." << std::endl;
		return;
	}
	char pathname[30];
	sprintf_s(pathname, ".\\samples\\sample%d.txt", num);
	std::vector<unsigned short>data;
	std::ifstream InputFile(pathname);
	int count = 0;
	unsigned short number;
	while (InputFile >> number && count < MaxInputNum + 1)
	{
		data.push_back(number);
		count++;
	}
	InputFile.close();
	printf("data size is %d\n", (int)data.size());
	if (data.size() < 1 + 28 * 28)
	{
		std::cout << "files does not exist or is damaged" << std::endl;
		return;
	}
	printf("target is:    %d\n", data[0]);
	count = 1;
	for (int i = 0; i < 28; i++)
	{
		for (int j = 0; j < 28; j++)
		{
			printf("%3d ", data[count]);
			count++;
		}
		printf("\n\n");
	}
	std::cout << "end test" << std::endl;
}