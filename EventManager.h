#pragma once
#include "Singleton.h"

class EventManager :
	public Singleton<EventManager>
{
public:
	EventManager();
	~EventManager();
	//�̺�Ʈ�� �߰��Ѵ�
	void AddEvent(std::function<void()> func, EEvent);
	//�̺�Ʈ�� �߻� �� ��ϵ� ��� �Լ� ����
	void OnEvent(EEvent);

private:
	//�̺�Ʈ(Key)�� ���� ������ �Լ��� ������ Map 
	std::unordered_map<EEvent,std::vector<std::function<void()>>> dicEvent;
};

EventManager* EventManager::instance = nullptr;

