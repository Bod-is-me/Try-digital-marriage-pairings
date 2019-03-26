#include "Head.h"

// �Աȶ���û��ʵ������
Male OUT_Male(WITHOUT);
Female OUT_Female(WITHOUT);

class Match
{
public:
	Match()
		:NUM_MATCH(0)
	{
		maleVec.reserve(PEPLE_SIZE + 1);
		femaleVec.reserve(PEPLE_SIZE + 1);
	}
	~Match() = default;

	// ��������NPC���ݣ����� male���󣬴洢
	void loadMaleData(const char *name)
	{
		FILE *fr = nullptr;
		fopen_s(&fr, name, "r");
		if (nullptr == fr) { cout << name << "�ļ������ڣ�" << endl;	return; }

		int data[7];

		while (!feof(fr))
		{
			fscanf_s(fr, "%d,%d,%d,%d,%d,%d,%d",
				&data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6]);
			maleVec.push_back(Male((int *)data));
		}
		fclose(fr);
		fr = nullptr;
	}
	// ����Ů��NPC���ݣ����� female���󣬴洢
	void loadFemaleData(const char *name)
	{
		FILE *fr = nullptr;
		fopen_s(&fr, name, "r");
		if (nullptr == fr) { cout << name << "�ļ������ڣ�" << endl;	return; }

		int data[7];

		while (!feof(fr))
		{
			fscanf_s(fr, "%d,%d,%d,%d,%d,%d,%d",
				&data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6]);
			femaleVec.push_back(Female((int *)data));
		}
		fclose(fr);
		fr = nullptr;

		// ����NPC ��¼ Ů��NPC�� �����
		add_NPCsatisfaction();
	}
	// ��������
	void join_player(int NUM_player, int *data)
	{
		// ������ or Ů��
		if (MALE == data[0])
		{
			data[0] = -1;
			maleVec.push_back(data);	// ��������NPC��
										// �������� ������ÿһλŮ�Ե����������
			Male *p_male = &maleVec.back();
			for (auto itfemale = femaleVec.begin(); itfemale != femaleVec.end(); ++itfemale)
			{
				p_male->insert(itfemale->Get_id(),
					p_male->Get_b1() * itfemale->Get_a1() + p_male->Get_b2() * itfemale->Get_a2() + p_male->Get_b3() * itfemale->Get_a3(),
					p_male->Get_b1() + p_male->Get_b2() + p_male->Get_b3()
					);
			}
		}
		else
		{
			data[0] = -1;
			femaleVec.push_back(data);	// ����Ů��NPC��
										// ��ÿһλ���� ������Ů��������
			Female *p_female = &femaleVec.back();
			for (auto itmale = maleVec.begin(); itmale != maleVec.end(); ++itmale)
			{
				itmale->insert(-1,
					itmale->Get_b1() * p_female->Get_a1() + itmale->Get_b2() * p_female->Get_a2() + itmale->Get_b3() * p_female->Get_a3(),
					itmale->Get_b1() + itmale->Get_b2() + itmale->Get_b3()
					);
			}
		}

		// ����һ�Ρ��������� ��Ů�� ������һ������
		frist_send(NUM_player);
		// Ů�Լ��ˣ�ȡ���������
		pair_resend(NUM_player);
		// �����á�
		reclear();
	}

private:
	// ����NPC ��¼ Ů��NPC�� �����
	void add_NPCsatisfaction()
	{
		// ��¼ ����Ů��NPC�� �����
		auto itmale = maleVec.begin();
		auto itmale_end = maleVec.end();
		for (; itmale != itmale_end; ++itmale)
		{
			auto itfemale = femaleVec.begin();
			auto itfemale_end = femaleVec.end();

			for (; itfemale != itfemale_end; ++itfemale)
			{
				itmale->insert(itfemale->Get_id(),
					itmale->Get_b1() * itfemale->Get_a1() + itmale->Get_b2() * itfemale->Get_a2() + itmale->Get_b3() * itfemale->Get_a3(),
					itmale->Get_b1() + itmale->Get_b2() + itmale->Get_b3()
					);
			}
		}
	}

	// ���������� Ů���б����� �������
	int find_bride()
	{
		register int IDfemale = WITHOUT;
		register int count = WITHOUT;
		register int sum_a123 = WITHOUT;
		
		auto itfemale = femaleVec.begin() + NUM_MATCH;
		auto itfemale_end = femaleVec.end();
		for (; itfemale != itfemale_end; ++itfemale)
		{
			int count_suitors = itfemale->Get_size();

			if (count_suitors > count)
			{
				IDfemale = itfemale->Get_id();
				count = count_suitors;
				sum_a123 = itfemale->Get_a1() + itfemale->Get_a2() + itfemale->Get_a3();
			}
			else if (count_suitors == count)
			{
				register int new_sum_a123 = itfemale->Get_a1() + itfemale->Get_a2() + itfemale->Get_a3();
				if(new_sum_a123 > sum_a123
					|| new_sum_a123 == sum_a123 && itfemale->Get_id() < IDfemale)
				{
					IDfemale = itfemale->Get_id();
					count = count_suitors;
					sum_a123 = itfemale->Get_a1() + itfemale->Get_a2() + itfemale->Get_a3();
				}
			}
		}
		return IDfemale;
	}

	// ��Ѱ�ҡ��� �洢������������ Ѱ��һ��ID
	Male& find(int sex, int ID)
	{
		int i = 0;
		for (auto &val : maleVec)
		{
			if (NUM_MATCH > i)
			{
				++i;
				continue;
			}

			if (val.Get_id() == ID)
			{
				return maleVec[i];
			}
			++i;
		}

		return OUT_Male;
	}
	Female& find(int ID)
	{
		int i = 0;
		for (auto &val : femaleVec)
		{
			if (NUM_MATCH > i)
			{
				++i;
				continue;
			}

			if (val.Get_id() == ID)
			{
				return femaleVec[i];
			}
			++i;
		}

		return OUT_Female;
	}

	// ������������Num������ ������ǰ��ȥ�����������ƥ��
	void swap(Male &groom, Female &bride)
	{
		// ����ʣ���������� ���������������б��У� ������ǰ��ȥ������������Ƚ�
		auto itmale = maleVec.begin();
		auto itmale_end = maleVec.end();
		for (; itmale != itmale_end; ++itmale)
		{
			itmale->swap(NUM_MATCH, bride.Get_id());
		}

		// ������ ������ǰ��ȥ�����������ƥ��
		Male tmpmale = move(maleVec[NUM_MATCH]);
		maleVec[NUM_MATCH] = move(groom);
		groom = move(tmpmale);

		// ������ ������ǰ��ȥ�����������ƥ��
		Female tmpfemale = move(femaleVec[NUM_MATCH]);
		femaleVec[NUM_MATCH] = move(bride);
		bride = move(tmpfemale);

		++NUM_MATCH;
	}

	// �����á�
	void reclear()
	{
		// ɾ������
		Male &player_male = this->find(1, -1);
		if (WITHOUT != player_male.Get_id())
		{
			maleVec.pop_back();
		}

		Female &player_female = this->find(-1);
		if (WITHOUT != player_female.Get_id())
		{
			femaleVec.pop_back();
			// ���� ����NPC 
			for (auto itmale = maleVec.begin(); itmale != maleVec.end(); ++itmale)
			{
				itmale->Del_back();
			}
		}

		// ���� Ů��NPC 
		for (auto itfemale = femaleVec.begin(); itfemale != femaleVec.end(); ++itfemale)
		{
			itfemale->clear();
		}

		NUM_MATCH = 0;
	}

	// ����һ�Ρ��������� ��Ů�� ������һ������
	void frist_send(int NUM_player)
	{
		// ��������
		auto itmale = maleVec.begin();
		auto itmale_end = maleVec.end();
		for (; itmale != itmale_end; ++itmale)
		{
			// ��ÿ������ ��������ȼ���ߵ�Ů�Ե� ID
			int IDfemale = itmale->find_love(0);

			// �ڸ�Ů�Ե� �����Ľ��յ���������� ���ϸ�����ID & �����
			Female &girl = this->find(IDfemale);

			girl.insert(itmale->Get_id(),
				girl.Get_b1() * itmale->Get_a1() + girl.Get_b2() * itmale->Get_a2() + girl.Get_b3() * itmale->Get_a3(),
				girl.Get_b1() + girl.Get_b2() + girl.Get_b3()
				);
		}
	}

	// Ů�Լ��ˣ�ȡ���������
	void pair_resend(int NUM_player)
	{
		int IDmale = WITHOUT;	// ����ID
		int IDfemale = WITHOUT;	// ����ID

		// �� ����
		IDfemale = find_bride();
		// û������!!!
		if (WITHOUT == IDfemale)
		{
			pair_map.insert(make_pair(NUM_player, make_pair(IDmale, IDfemale)));
			return;
		}

		// ���� ѡ��
		Female &bride = this->find(IDfemale);
		IDmale = bride.find_love();
		// û������!!!
		if (WITHOUT == IDmale)
		{
			pair_map.insert(make_pair(NUM_player, make_pair(IDmale, IDfemale)));
			return;
		}

		// �� �ǲ��� ����
		if (-1 == IDmale || -1 == IDfemale)
		{
			pair_map.insert(make_pair(NUM_player, make_pair(IDmale, IDfemale)));
			return;
		}
		else
		{
			// ������������Num������ ������ǰ��ȥ�����������ƥ��
			Male &groom = this->find(1, IDmale);
			swap(groom, bride);
			// NPC ƥ�����ˡ�����
			if (100 == NUM_MATCH)
			{
				pair_map.insert(make_pair(NUM_player, make_pair(WITHOUT, WITHOUT)));
				return;
			}
			// ��һλ�����׷���� ������һλŮ�Է�������
			loser_resend(NUM_player);
		}
	}

	// ��һλ�����׷���� ������һλŮ�Է�������
	void loser_resend(int NUM_player)
	{
		auto &groom = maleVec[NUM_MATCH - 1];
		auto &bride = femaleVec[NUM_MATCH - 1];
		for (; bride.Get_size() ;)
		{
			auto it = bride.inviteList_front();
			if (it.first == groom.Get_id())
			{
				bride.Del_front();
				continue;
			}
			Male &new_groom = this->find(1, it.first);

			int IDfemale = new_groom.find_love(NUM_MATCH);
			Female &new_bride = this->find(IDfemale);

			new_bride.insert(it.first,
				new_bride.Get_b1() * new_groom.Get_a1() + new_bride.Get_b2() * new_groom.Get_a2() + new_bride.Get_b3() * new_groom.Get_a3(),
				new_bride.Get_b1() + new_bride.Get_b2() + new_bride.Get_b3()
				);
			bride.Del_front();
		}
		// Ů�Լ��ˣ�ȡ���������
		pair_resend(NUM_player);
	}

	friend Male;
	friend Female;


	vector<Male> maleVec;		// �洢�������Ե�����
	vector<Female> femaleVec;	// �洢����Ů�Ե�����
	int NUM_MATCH;				// һ��ƥ���� �ڼ������ˣ��� 0 ->��
};

// ����߳�
void first_thread()
{
	int num = 1;
	while (true)
	{
		if (PEPLE_SIZE + 1 == num)
		{
			break;
		}
		auto it = pair_map.find(num);
		if (pair_map.end() != it)
		{
			cout << "��" << num++ << "��player����";
			if (WITHOUT == it->second.first || WITHOUT == it->second.second)
			{
				cout << endl;
			}
			else
			{
				cout << it->second.first << ":" << it->second.second << endl;
			}
		}
	}
}
// ���������ļ�,���� DATA��
void Join_players(const char *name)
{
	FILE *fr = nullptr;
	fopen_s(&fr, name, "r");
	if (nullptr == fr) { cout << name << "�ļ������ڣ�" << endl;		return; }

	int num = 0;	// ��num��player���룺
	while (num < PEPLE_SIZE)
	{
		fscanf_s(fr, "%d,%d,%d,%d,%d,%d,%d",
			*(DATA + num), *(DATA + num) + 1, *(DATA + num) + 2, *(DATA + num) + 3,
			*(DATA + num) + 4, *(DATA + num) + 5, *(DATA + num) + 6);

		++num;
	}

	fclose(fr);
	fr = nullptr;
}
void thread_fun(Match match)
{
	while (NUM < PEPLE_SIZE)
	{
		//thread_mutex.lock();	// ����һ
		{
			unique_lock<mutex> lock(thread_mutex);	// ������
			if (NUM < PEPLE_SIZE)
			{
				++NUM;
			}
		}
		//thread_mutex.unlock();	// ����һ

		// ����NPC��
		match.join_player(NUM, *(DATA + NUM - 1));
	}
}
int main()
{
	MyTimer timer;
	timer.Start();
//////////////////////////////// ��ʱ��ʼ

	Match match;
	//match.loadMaleData("G:\\�ĵ��ܼ�\\visual studio 2012\\cheng xu\\zi_ji\\��Ŀ\\C++\\thread_pipei\\thread_pipei\\male.txt");
	//match.loadFemaleData("G:\\�ĵ��ܼ�\\visual studio 2012\\cheng xu\\zi_ji\\��Ŀ\\C++\\thread_pipei\\thread_pipei\\female.txt");
	//Join_players("G:\\�ĵ��ܼ�\\visual studio 2012\\cheng xu\\zi_ji\\��Ŀ\\C++\\thread_pipei\\thread_pipei\\players.txt");
	match.loadMaleData("male.txt");
	match.loadFemaleData("female.txt");
	Join_players("players.txt");

	//thread_fun(match);
	thread th_1(thread_fun, match); 
	thread th_2(thread_fun, match);
	thread th_3(thread_fun, match); 
	
	th_1.detach();
	th_2.detach();
	th_3.detach();

	thread thread_st(first_thread);
	thread_st.join();

	cout << endl;
	cout << "Mumm...";
//////////////////////////////// ��ʱ����
	timer.Stop();
	timer.showTime();

	return 0;
}