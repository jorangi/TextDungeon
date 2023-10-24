#include <iostream>
#include <cmath>
#include <cstring>
#include <sstream>
#include <windows.h>
#include <list>
#include <algorithm>
using namespace std;

struct context
{
	string text;
	bool cleaning;
	bool input;
};
class IObserver
{
public:
	virtual void Update() {};
};
class ISubject 
{
public:
	list<IObserver*> observers;
	virtual void AddObserver(IObserver* obs) {};
	virtual void RemoveObserver(IObserver* obs) {};
	virtual void UpdateToObservers() {};
};
class Observer : public IObserver
{
public:
	list<ISubject*> subs;
	Observer(ISubject& subject)
	{
		subject.AddObserver(this);
	}
	void Remove()
	{
		list<ISubject*>::iterator iterator = subs.end();
		while (iterator != subs.begin())
		{
			(*iterator)->RemoveObserver(this);
			iterator--;
		}
	}
	void Update()
	{

	}
};
class Subject : public ISubject
{
public:
	~Subject()
	{
		list<IObserver*>::iterator iterator = observers.end();
		while (iterator != observers.begin())
		{
			RemoveObserver(*iterator);
			iterator--;
		}
	}
	void AddObserver(IObserver* obs) override
	{
		observers.push_back(obs);
	}
	void RemoveObserver(IObserver* obs) override
	{
		observers.remove(obs);
	}
	void UpdateToObservers() override
	{
		list<IObserver*>::iterator iterator = observers.begin();
		while (iterator != observers.end())
		{
			(*iterator)->Update();
			iterator++;
		}
	}
};

string PrintBox(string text, bool cleaning, bool input);
bool YesOrNo(string text);
void PlayerInit();
void Village();

enum Job
{
	none = 0,
	warrior = 1,
	mage = 2,
	thief = 3
};
string Dungeon[5] = { "토끼굴", "늑대의 숲", "흙빛 늪지대", "폐저택", "구름과 닿은 곳" };
string Shops[5] = { "잡화점", "무기점", "주술사의 집", "투기장", "마을회관" };
context con;

class PlayerData:public Subject
{
	class Skill
	{
	public:
		enum skillID
		{
			HPup = 0,
			SPup = 1
		};
		skillID id;
		unsigned int lv;
		
		void SetLv(unsigned int val)
		{
			lv = val;
		}
	};
public:
	string PlayerName;
	Job PlayerJob = Job::none;
	string JobToString()
	{
		switch(PlayerJob)
		{
			case Job::warrior:
				return "전사";
			case Job::mage:
				return "마법사";
			case Job::thief:
				return "도적";
		}
	}
	Job StringToJob(string job)
	{
		if(job == "전사")
			return Job::warrior;
		else if(job == "마법사")
			return Job::mage;
		else if(job == "도적")
			return Job::thief;
		
		return Job::none;
	}
	unsigned int HP = 0;
	unsigned int MaxHP = 0;
	unsigned int SP = 0;
	unsigned int MaxSP = 0;
	unsigned int Money = 0;
	Skill skills[2];
	bool Dungeons[5];

	void SkillSet(Skill::skillID skill, unsigned int level)
	{
		if (skill == Skill::skillID::HPup)
		{
			skills[(int)skill].SetLv(level);
		}
	}
	void SetMaxHP(unsigned int val, bool update = true)
	{
		MaxHP = val;
		if(update)Subject::UpdateToObservers();
	}
	void SetHP(unsigned int val, bool update = true)
	{
		HP = val;
		if (update)Subject::UpdateToObservers();
	}
	void AddHP(int val)
	{
		SetHP(min(MaxHP, HP + val));
	}
	void SetMaxSP(unsigned int val, bool update = true)
	{
		MaxSP = val;
		if (update)Subject::UpdateToObservers();
	}
	void SetSP(unsigned int val, bool update = true)
	{
		SP = val;
		if (update)Subject::UpdateToObservers();
	}
	void AddSP(int val)
	{
		SetSP(min(MaxSP, SP + val));
	}
	void AddMoney(int val)
	{
		Money += val;
		Subject::UpdateToObservers();
	}
};
PlayerData* player;

class MainUI :public Observer
{
	string hpBar = "/n HP : [";
	string spBar = "  |   SP : [";
public:
	MainUI(Subject& subject) :Observer(subject) {};
	void Update() override
	{
		ShowUI(true);
	}
	void ShowUI(bool keepCon = false)
	{
		hpBar = "/n HP : [";
		spBar = "  |  SP : [";

		for (int i = 1; i <= 10; i++)
		{
			if (i <= ((float)player->HP / (float)player->MaxHP * 0.1) * 100)
			{
				hpBar += "♥";
			}
			else
				hpBar += "♡";
		}
		hpBar += "] | " + to_string(player->HP) + "/" + to_string(player->MaxHP);
		for (int i = 1; i <= 10; i++)
		{
			if (i <= ((float)player->SP / (float)player->MaxSP * 0.1) * 100)
			{
				spBar += "◆";
			}
			else
				spBar += "◇";
		}
		spBar += "] | " + to_string(player->SP) + "/" + to_string(player->MaxSP);

		PrintBox("플레이어명 : " + player->PlayerName + "/n직업 : " + player->JobToString() + hpBar + spBar + "/n돈 : " + to_string(player->Money) + " G", true, false);
		if (keepCon) PrintBox(con.text, con.cleaning, con.input);
	}
};
MainUI* mainUI;

string PrintBox(string text, bool cleaning = true, bool input = true)
{
	if(cleaning) system("cls");
	int len = text.length();
	int lineNum = 0;
	string arr[100];
	int textCount = 0;

	for (int i = 0; i < text.length(); i++, textCount++)
	{
		if (textCount == 160)
		{
			arr[lineNum] = text.substr(i - textCount, 160);
			textCount = 0;
			lineNum++;
		}
		else if (i == text.length() - 1)
		{
			arr[lineNum] = text.substr(i - textCount, i + 1);
			textCount = 0;
			lineNum++;
		}
		else if (text[i] == '/' && text[i + 1] == 'n')
		{
			arr[lineNum] = text.substr(i - textCount, textCount);
			i += 2;
			textCount = 0;
			lineNum++;
		}
	}

	cout << "┌";
	for (int i = 0; i < 197; i++)
	{
		cout << "─";
	}
	cout << "┐";


	for (int i = 0; i < lineNum; i++)
	{
		cout << endl << "│";
		for (int i = 0; i < 197; i++)
		{
			cout << " ";
		}
		cout << "│";

		cout << endl << "│";
		int size = 100 + arr[i].length() * 0.5f;
		cout.width(size);

		cout << arr[i];

		for (int i = 0; i < 197 - size; i++)
		{
			cout << " ";
		}
		cout << "│";
	}

	cout << endl << "│";
	for (int i = 0; i < 197; i++)
	{
		cout << " ";
	}
	cout << "│";

	cout << endl << "└";
	for (int i = 0; i < 197; i++)
	{
		cout << "─";
	}
	cout << "┘" << endl;
	
	if (input)
	{
		cout.width(102);
		cout << "입력 : ";
	}
	return text;
}
string Print(string text, bool input = true)
{
	mainUI->ShowUI();
	return PrintBox(text, false, input);
}
void GameStart()
{
	PlayerInit();
	Village();
}
bool YesOrNo(string text)
{
	PrintBox(text);

	string ans;
	cin >> ans;

	bool errorCheck = false;
	if (ans != "예" && ans != "아니요")
	{
		try
		{
			if (stoi(ans) == 1 || stoi(ans) == 2)
				errorCheck = false;
			else
				errorCheck = true;
		}
		catch (...)
		{
			errorCheck = true;
		}
	}
	else
	{
		errorCheck = true;
	}

	while (errorCheck)
	{
		if (ans != "예" && ans != "아니요")
		{
			try
			{
				if (stoi(ans) == 1 || stoi(ans) == 2)
				{
					errorCheck = false;
					break;
				}
				else
					errorCheck = true;
			}
			catch (...)
			{
				errorCheck = true;
			}
		}
		else
		{
			errorCheck = true;
			break;
		}

		cin.clear();
		cin.ignore(256, '\n');
		system("cls");

		PrintBox(text);
		cin >> ans;
	}

	return ans == "예" || ans == "1" ? true : false;
}
void PlayerInit()
{
	PrintBox("플레이어의 이름을 입력해주세요");

	cin >> player->PlayerName;

	while (!YesOrNo("\"" + player->PlayerName + "\" 으로 결정합니까?/n1: 예   2. 아니요"))
	{
		PrintBox("플레이어의 이름을 입력해주세요");

		cin >> player->PlayerName;
	}
	PrintBox("플레이어의 직업을 골라주세요/n1.전사   2.마법사   3.도적");
	string job;
	cin >> job;
	bool errorCheck = false;

	if (player->StringToJob(job) == Job::none)
	{
		try
		{
			if (stoi(job) < 1 || stoi(job) > 3)
				errorCheck = true;
		}
		catch (...)
		{
			errorCheck = true;
		}
	}

	while (errorCheck)
	{
		if (player->StringToJob(job) == Job::none)
		{
			try
			{
				if (stoi(job) < 1 || stoi(job) > 3)
					errorCheck = true;
				else
				{
					errorCheck = false;
					break;
				}
			}
			catch (...)
			{
				errorCheck = true;
			}
		}
		else
		{
			errorCheck = false;
			break;
		}

		cin.clear();
		cin.ignore(256, '\n');
		system("cls");
		PrintBox("[플레이어의 직업을 골라주세요]/n1. 전사   2.마법사   3.도적");
		cin >> job;
	}
	if (player->StringToJob(job) == Job::none) player->PlayerJob = (Job)stoi(job);
	else player->PlayerJob = player->StringToJob(job);

	while (!YesOrNo("\"" + player->JobToString() + "\" 직업으로 결정합니까?/n1: 예   2. 아니요"))
	{
		PrintBox("[플레이어의 직업을 골라주세요]/n1.전사   2.마법사   3.도적");

		cin >> job;
		if (player->StringToJob(job) == Job::none)
		{
			try
			{
				if (stoi(job) < 1 || stoi(job) > 3)
					errorCheck = true;
			}
			catch (...)
			{
				errorCheck = true;
			}
		}
		while (errorCheck)
		{
			if (player->StringToJob(job) == Job::none)
			{
				try
				{
					if (stoi(job) < 1 || stoi(job) > 3)
						errorCheck = true;
					else
					{
						errorCheck = false;
						break;
					}
				}
				catch (...)
				{
					errorCheck = true;
				}
			}
			else
			{
				errorCheck = false;
				break;
			}

			cin.clear();
			cin.ignore(256, '\n');
			system("cls");
			PrintBox("[플레이어의 직업을 골라주세요]/n1. 전사   2.마법사   3.도적");
			cin >> job;
		}
		if (player->StringToJob(job) == Job::none) player->PlayerJob = (Job)stoi(job);
		else player->PlayerJob = player->StringToJob(job);
	}

	player->Money = 1000;
	player->SetMaxHP(10, false);
	player->SetHP(player->MaxHP, false);
	player->SetMaxSP(10, false);
	player->SetSP(player->MaxSP, false);
}
void Village(int sel)
{
	string portals;
	int portalsCount = 0;
	string villSel;
	bool errorCheck = false;
	switch (sel)
	{
		case 1:
			portals = "수 많은 포탈이 열려있습니다./n어느 포탈로 들어가시겠습니까?/n 0.나간다/n";
			portalsCount = 0;
			for (int i = 0; i < Dungeon->size(); i++)
			{
				if (player->Dungeons[i])
				{
					portals += to_string(portalsCount + 1) + "." + Dungeon[i] + "/n";
					portalsCount++;
				}
			}
			Print(portals);
			cin >> villSel;
			try
			{
				if (stoi(villSel))
				{
					errorCheck = false;
					break;
				}
				else errorCheck = true;
			}
			catch(...)
			{
				errorCheck = true;
			}
			while (errorCheck)
			{
				portals = "수 많은 포탈이 열려있습니다./n어느 포탈로 들어가시겠습니까?/n 0.나간다/n";
				portalsCount = 0;
				for (int i = 0; i < Dungeon->size(); i++)
				{
					if (player->Dungeons[i])
					{
						portals += to_string(portalsCount + 1) + "." + Dungeon[i] + "/n";
						portalsCount++;
					}
				}
				Print(portals);
				cin >> villSel;
			}
			break;
		case 2:
			Print("상점에 왔습니다./n 무엇을 할까요?/n 0 : 나간다/n 1.구매한다/n 2.판매한다/n3.상점 주인과 이야기를 나눈다./n4.상점 안에 누군가와 이야기를 나눈다");
			cin >> villSel;
			break;
	}
}
void Village()
{
	con.text = Print("마을에 도착했습니다./n 행동을 선택해주세요./n1.던전/n2.상점");

	string sel;
	bool errorCheck = false;
	cin >> sel;
	try
	{
		if (stoi(sel)) errorCheck = false;
		else errorCheck = true;
	}
	catch (...)
	{
		errorCheck = true;
	}
	while (errorCheck)
	{
		con.text = Print("마을에 도착했습니다./n 행동을 선택해주세요./n1.던전/n2.상점가");
		cin >> sel;
		try
		{
			if (stoi(sel)) errorCheck = false;
			else errorCheck = true;
		}
		catch (...)
		{
			errorCheck = true;
		}
	}
	Village(stoi(sel));
}
int main()
{
	const char* title[80];
	*title =  "텍스트 던전 v.00";
	char systemText[80] = "mode con: cols=200 lines=50 | title ";
	system(strcat(systemText, *title));
	player = new PlayerData;
	mainUI = new MainUI(*player);
	player->Dungeons[0] = true;
	player->Dungeons[1] = false;
	player->Dungeons[2] = false;
	player->Dungeons[3] = true;
	player->Dungeons[4] = true;

	GameStart();
}