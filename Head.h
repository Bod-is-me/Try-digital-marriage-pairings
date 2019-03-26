#pragma once
#include <cstdio>
#include <iostream>
using namespace std;
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>
#include <utility>
/* �������� */
#include <windows.h>
class MyTimer
{
public:
	MyTimer()
	{
		QueryPerformanceFrequency(&_freq);
		costTime = 0.0;
	}

	void Start()
	{
		for (int i = 0; i<EN_NUMER; ++i)
		{
			QueryPerformanceCounter(&_array[i]._begin);
		}
	}
	void Stop()
	{
		for (int i = 0; i<EN_NUMER; ++i)
		{
			QueryPerformanceCounter(&_array[i]._end);
		}
	}
	void Reset()
	{
		costTime = 0.0;
	}
	void showTime()
	{
		double allTime = 0.0;
		for (int i = 0; i<EN_NUMER; ++i)
		{
			allTime += (((double)_array[i]._end.QuadPart -
				(double)_array[i]._begin.QuadPart) / (double)_freq.QuadPart);
		}
		costTime = allTime / EN_NUMER;
		costTime *= 1000000;

		if ((((int)costTime) / 1000000) > 0)
		{
			cout << costTime / 1000000 << " s" << endl;
		}
		else if (((int)costTime) / 1000 > 0)
		{
			cout << costTime / 1000 << " ms" << endl;
		}
		else
		{
			cout << costTime << " us" << endl;
		}
	}

private:
	class Array
	{
	public:
		LARGE_INTEGER _begin;
		LARGE_INTEGER _end;
	};
	enum { EN_NUMER = 5 };
	LARGE_INTEGER _freq;
	double costTime;
	Array _array[EN_NUMER];
};

#define	PEPLE_SIZE	100				// ���Ǹ���
#define	MALE		1				// �е�
#define	FEMALE		0				// Ů��
#define	WITHOUT		-9				

unordered_map<int, pair<int, int>> pair_map;
int DATA[PEPLE_SIZE][7];
volatile int NUM = 0;
mutex thread_mutex;

class Male
{
public:
	Male(int n = WITHOUT)
		:_id(n)
	{}
	Male(int *arr)
		:_id(arr[0]), _a1(arr[1]), _a2(arr[2]), _a3(arr[3]), _b1(arr[4]), _b2(arr[5]), _b3(arr[6])
	{
		satisfyVec.reserve(PEPLE_SIZE + 1);
	}
	~Male() = default;

	Male(const Male &src)
	{
		cpy(src);
		satisfyVec = src.satisfyVec;
	}
	Male(Male &&src)
	{
		cpy(src);
		satisfyVec = move(src.satisfyVec);
	}
	Male& operator=(const Male &src)
	{
		cpy(src);
		satisfyVec = src.satisfyVec;
		return *this;
	}
	Male& operator=(Male &&src)
	{
		cpy(src);

		satisfyVec = move(src.satisfyVec);
		return *this;
	}

/* getϵ�� */#if 1
	int Get_id() { return _id; }
	int Get_a1() { return _a1; }	// �Ƹ�
	int Get_a2() { return _a2; }	// ��ò
	int Get_a3() { return _a3; }	// Ʒ��
	int Get_b1() { return _b1; }	// �Ƹ���Ҫ��Է���
	int Get_b2() { return _b2; }	// ��ò��Ҫ��Է���
	int Get_b3() { return _b3; }	// Ʒ��Ҫ��Է���
#endif

	int Get_size()
	{
		return satisfyVec.size();
	}
	void Del_back()
	{
		satisfyVec.pop_back();
	}

	// �����롿�� ���Զ�Ů�Ե�������б��� ����һ��Ԫ��
	void insert(int first, int second, int third)
	{
		satisfyVec.push_back(make_pair(first, make_pair(second, third)));
	}

	// �����������������������б��У� ������ǰ��ȥ������������Ƚ�
	void swap(int Num_match, int bride_id)
	{
		auto it = satisfyVec.begin();
		auto it_end = satisfyVec.end();
		int count = 0;
		for (; it != it_end; ++it)
		{
			if (it->first == bride_id)
			{
				break;
			}
			++count;
		}

		auto tmp = move(satisfyVec[Num_match]);
		satisfyVec[Num_match] = move(satisfyVec[count]);
		satisfyVec[count] = move(tmp);
	}

	// ���������� ���Զ�Ů�Ե�������б���Ѱ�� ���������ѡ
	int find_love(int Num_match)
	{
		register int ID = WITHOUT;
		register int max_Satisfaction = WITHOUT;
		register int sum_a123 = WITHOUT;

		auto it = satisfyVec.begin();
		while (Num_match > 0)
		{
			++it;
			--Num_match;
		}
		auto it_end = satisfyVec.end();
		for (; it != it_end; ++it)
		{
			if (it->second.first > max_Satisfaction)
			{
				ID = it->first;
				max_Satisfaction = it->second.first;
				sum_a123 = it->second.second;
			}
			else if (it->second.first == max_Satisfaction)
			{
				if (it->second.second > sum_a123
					|| it->second.second == sum_a123 && it->first < ID)
				{
					ID = it->first;
					max_Satisfaction = it->second.first;
					sum_a123 = it->second.second;
				}
			}
		}

		return ID;
	}

private:
	// ���Զ�Ů�Ե�������б�
	vector<pair<int, pair<int, int>>> satisfyVec;

	// ��������
	void cpy(const Male &src)
	{
		_id = src._id;
		_a1 = src._a1;
		_a2 = src._a2;
		_a3 = src._a3;
		_b1 = src._b1;
		_b2 = src._b2;
		_b3 = src._b3;
	}

	int _id;
	int _a1;	// �Ƹ�
	int _a2;	// ��ò
	int _a3;	// Ʒ��
	int _b1;	// �Ƹ���Ҫ��Է���
	int _b2;	// ��ò��Ҫ��Է���
	int _b3;	// Ʒ��Ҫ��Է���
};

class Female
{
public:
	Female(int n = WITHOUT)
		:_id(n)
	{}
	Female(int *arr)
		:_id(arr[0]), _a1(arr[1]), _a2(arr[2]), _a3(arr[3]), _b1(arr[4]), _b2(arr[5]), _b3(arr[6])
	{}
	~Female() = default;

	Female(const Female &src)
	{
		cpy(src);
		inviteList = src.inviteList;
	}
	Female(Female &&src)
	{
		cpy(src);
		inviteList = move(src.inviteList);
	}
	Female& operator=(const Female &src)
	{
		cpy(src);
		inviteList = src.inviteList;
		return *this;
	}
	Female& operator=(Female &&src)
	{
		cpy(src);
		inviteList = move(src.inviteList);
		return *this;
	}

/* getϵ�� */#if 1
	int Get_id() { return _id; }
	int Get_a1() { return _a1; }	// �Ƹ�
	int Get_a2() { return _a2; }	// ��ò
	int Get_a3() { return _a3; }	// Ʒ��
	int Get_b1() { return _b1; }	// �Ƹ���Ҫ��Է���
	int Get_b2() { return _b2; }	// ��ò��Ҫ��Է���
	int Get_b3() { return _b3; }	// Ʒ��Ҫ��Է���
#endif

	int Get_size() { return inviteList.size(); }
	pair<int, pair<int, int>>& inviteList_front() { return inviteList.front(); }
	void Del_front() { inviteList.erase(inviteList.begin()); }

	// �����롿�� Ů�Խ��յ��������б��� ����һ��Ԫ��
	void insert(int first, int second, int third)
	{
		inviteList.push_front(make_pair(first, make_pair(second, third)));
	}
	// clear()
	void clear()
	{
		inviteList.clear();
	}

	// ���������� Ů�Խ��յ��������б����� ���������ѡ
	int find_love()
	{
		register int ID = WITHOUT;
		register int max_Satisfaction = WITHOUT;
		register int sum_a123 = WITHOUT;

		auto it = inviteList.begin();
		auto it_end = inviteList.end();
		for (; it != it_end; ++it)
		{
			if (it->second.first > max_Satisfaction)
			{
				ID = it->first;
				max_Satisfaction = it->second.first;
				sum_a123 = it->second.second;
			}
			else if (it->second.first == max_Satisfaction)
			{
				if (it->second.second > sum_a123
					|| it->second.second == sum_a123 && it->first < ID)
				{
					ID = it->first;
					max_Satisfaction = it->second.first;
					sum_a123 = it->second.second;
				}
			}
		}

		return ID;
	}

private:
	// Ů�Խ��յ��������б�
	list<pair<int, pair<int, int>>> inviteList;

	// ��������
	void cpy(const Female &src)
	{
		_id = src._id;
		_a1 = src._a1;
		_a2 = src._a2;
		_a3 = src._a3;
		_b1 = src._b1;
		_b2 = src._b2;
		_b3 = src._b3;
	}

	int _id;
	int _a1;	// �Ƹ�
	int _a2;	// ��ò
	int _a3;	// Ʒ��
	int _b1;	// �Ƹ���Ҫ��Է���
	int _b2;	// ��ò��Ҫ��Է���
	int _b3;	// Ʒ��Ҫ��Է���
};