#include "Head.h"

// 对比对象，没有实际意义
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

	// 加载男性NPC数据，生成 male对象，存储
	void loadMaleData(const char *name)
	{
		FILE *fr = nullptr;
		fopen_s(&fr, name, "r");
		if (nullptr == fr) { cout << name << "文件不存在！" << endl;	return; }

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
	// 加载女性NPC数据，生成 female对象，存储
	void loadFemaleData(const char *name)
	{
		FILE *fr = nullptr;
		fopen_s(&fr, name, "r");
		if (nullptr == fr) { cout << name << "文件不存在！" << endl;	return; }

		int data[7];

		while (!feof(fr))
		{
			fscanf_s(fr, "%d,%d,%d,%d,%d,%d,%d",
				&data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6]);
			femaleVec.push_back(Female((int *)data));
		}
		fclose(fr);
		fr = nullptr;

		// 男性NPC 记录 女性NPC的 满意度
		add_NPCsatisfaction();
	}
	// 加入主角
	void join_player(int NUM_player, int *data)
	{
		// 是男主 or 女主
		if (MALE == data[0])
		{
			data[0] = -1;
			maleVec.push_back(data);	// 放入男性NPC中
										// 让男主角 做出对每一位女性的满意度评价
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
			femaleVec.push_back(data);	// 放入女性NPC中
										// 让每一位男性 对评价女主角评价
			Female *p_female = &femaleVec.back();
			for (auto itmale = maleVec.begin(); itmale != maleVec.end(); ++itmale)
			{
				itmale->insert(-1,
					itmale->Get_b1() * p_female->Get_a1() + itmale->Get_b2() * p_female->Get_a2() + itmale->Get_b3() * p_female->Get_a3(),
					itmale->Get_b1() + itmale->Get_b2() + itmale->Get_b3()
					);
			}
		}

		// 【第一次】所有男性 对女性 发出第一次邀请
		frist_send(NUM_player);
		// 女性嫁人，取消单身徽章
		pair_resend(NUM_player);
		// 【重置】
		reclear();
	}

private:
	// 男性NPC 记录 女性NPC的 满意度
	void add_NPCsatisfaction()
	{
		// 记录 所有女性NPC的 满意度
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

	// 【遍历】在 女性列表中找 最佳新娘
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

	// 【寻找】在 存储样本的容器中 寻找一个ID
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

	// 【交换】将第Num对新人 交换到前面去，不参与后续匹配
	void swap(Male &groom, Female &bride)
	{
		// 遍历剩余所有男性 将新娘（在其满意度列表中） 交换到前面去，不参与后续比较
		auto itmale = maleVec.begin();
		auto itmale_end = maleVec.end();
		for (; itmale != itmale_end; ++itmale)
		{
			itmale->swap(NUM_MATCH, bride.Get_id());
		}

		// 将新郎 交换到前面去，不参与后续匹配
		Male tmpmale = move(maleVec[NUM_MATCH]);
		maleVec[NUM_MATCH] = move(groom);
		groom = move(tmpmale);

		// 将新娘 交换到前面去，不参与后续匹配
		Female tmpfemale = move(femaleVec[NUM_MATCH]);
		femaleVec[NUM_MATCH] = move(bride);
		bride = move(tmpfemale);

		++NUM_MATCH;
	}

	// 【重置】
	void reclear()
	{
		// 删除主角
		Male &player_male = this->find(1, -1);
		if (WITHOUT != player_male.Get_id())
		{
			maleVec.pop_back();
		}

		Female &player_female = this->find(-1);
		if (WITHOUT != player_female.Get_id())
		{
			femaleVec.pop_back();
			// 重置 男性NPC 
			for (auto itmale = maleVec.begin(); itmale != maleVec.end(); ++itmale)
			{
				itmale->Del_back();
			}
		}

		// 重置 女性NPC 
		for (auto itfemale = femaleVec.begin(); itfemale != femaleVec.end(); ++itfemale)
		{
			itfemale->clear();
		}

		NUM_MATCH = 0;
	}

	// 【第一次】所有男性 对女性 发出第一次邀请
	void frist_send(int NUM_player)
	{
		// 遍历男生
		auto itmale = maleVec.begin();
		auto itmale_end = maleVec.end();
		for (; itmale != itmale_end; ++itmale)
		{
			// 找每个男生 满意度优先级最高的女性的 ID
			int IDfemale = itmale->find_love(0);

			// 在该女性的 在她的接收到的邀请表上 加上该男生ID & 满意度
			Female &girl = this->find(IDfemale);

			girl.insert(itmale->Get_id(),
				girl.Get_b1() * itmale->Get_a1() + girl.Get_b2() * itmale->Get_a2() + girl.Get_b3() * itmale->Get_a3(),
				girl.Get_b1() + girl.Get_b2() + girl.Get_b3()
				);
		}
	}

	// 女性嫁人，取消单身徽章
	void pair_resend(int NUM_player)
	{
		int IDmale = WITHOUT;	// 新郎ID
		int IDfemale = WITHOUT;	// 新娘ID

		// 找 新娘
		IDfemale = find_bride();
		// 没新娘了!!!
		if (WITHOUT == IDfemale)
		{
			pair_map.insert(make_pair(NUM_player, make_pair(IDmale, IDfemale)));
			return;
		}

		// 新娘 选人
		Female &bride = this->find(IDfemale);
		IDmale = bride.find_love();
		// 没新郎了!!!
		if (WITHOUT == IDmale)
		{
			pair_map.insert(make_pair(NUM_player, make_pair(IDmale, IDfemale)));
			return;
		}

		// 看 是不是 主角
		if (-1 == IDmale || -1 == IDfemale)
		{
			pair_map.insert(make_pair(NUM_player, make_pair(IDmale, IDfemale)));
			return;
		}
		else
		{
			// 【交换】将第Num对新人 交换到前面去，不参与后续匹配
			Male &groom = this->find(1, IDmale);
			swap(groom, bride);
			// NPC 匹配完了。。。
			if (100 == NUM_MATCH)
			{
				pair_map.insert(make_pair(NUM_player, make_pair(WITHOUT, WITHOUT)));
				return;
			}
			// 上一位新娘的追求者 重向下一位女性发出邀请
			loser_resend(NUM_player);
		}
	}

	// 上一位新娘的追求者 重向下一位女性发出邀请
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
		// 女性嫁人，取消单身徽章
		pair_resend(NUM_player);
	}

	friend Male;
	friend Female;


	vector<Male> maleVec;		// 存储样本男性的容器
	vector<Female> femaleVec;	// 存储样本女性的容器
	int NUM_MATCH;				// 一次匹配中 第几对新人（从 0 ->）
};

// 输出线程
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
			cout << "第" << num++ << "组player加入";
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
// 遍历主角文件,填入 DATA中
void Join_players(const char *name)
{
	FILE *fr = nullptr;
	fopen_s(&fr, name, "r");
	if (nullptr == fr) { cout << name << "文件不存在！" << endl;		return; }

	int num = 0;	// 第num组player加入：
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
		//thread_mutex.lock();	// 方法一
		{
			unique_lock<mutex> lock(thread_mutex);	// 方法二
			if (NUM < PEPLE_SIZE)
			{
				++NUM;
			}
		}
		//thread_mutex.unlock();	// 方法一

		// 放入NPC中
		match.join_player(NUM, *(DATA + NUM - 1));
	}
}
int main()
{
	MyTimer timer;
	timer.Start();
//////////////////////////////// 计时开始

	Match match;
	//match.loadMaleData("G:\\文档总集\\visual studio 2012\\cheng xu\\zi_ji\\项目\\C++\\thread_pipei\\thread_pipei\\male.txt");
	//match.loadFemaleData("G:\\文档总集\\visual studio 2012\\cheng xu\\zi_ji\\项目\\C++\\thread_pipei\\thread_pipei\\female.txt");
	//Join_players("G:\\文档总集\\visual studio 2012\\cheng xu\\zi_ji\\项目\\C++\\thread_pipei\\thread_pipei\\players.txt");
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
//////////////////////////////// 计时结束
	timer.Stop();
	timer.showTime();

	return 0;
}