#pragma once
class Singleton 
{
public:
	static Singleton& instance()
	{
		static Singleton instance;
		return instance;
	}
private:
	Singleton() {}
	~Singleton() {}
	Singleton &operator=(const Singleton& singleton) {}
	Singleton(const Singleton&) {}
};
#include<mutex>//call_once 在mutex头文件中
class Singleton
{
public:
	Singleton* instance()
	{
		static std::once_flag oc;//用于call_once的局部静态变量
		Singleton * m_singinstance;
		std::call_once(oc, [&] {  m_singinstance = new Singleton(); });
		return m_singinstance;
	}
private:
	Singleton() {}
	~Singleton() {}
	Singleton &operator=(const Singleton& singleton);
	Singleton(const Singleton&);
};
#include<atomic>
class Singleton
{
public:
	static Singleton* getInstance();
private:
	static std::atomic<Singleton*> m_instance;
	static std::mutex m_mutex;
	Singleton() {}
	~Singleton() {}
	Singleton &operator=(const Singleton& singleton);
	Singleton(const Singleton&);
};
std::atomic<Singleton*> Singleton::m_instance = nullptr;//静态成员需要先初始化
std::mutex Singleton::m_mutex;
Singleton* Singleton::getInstance() {
	Singleton* tmp = m_instance.load(std::memory_order_relaxed);
	std::atomic_thread_fence(std::memory_order_acquire);
	if (tmp == nullptr) {
		std::lock_guard<std::mutex> lock(m_mutex);
		tmp = m_instance.load(std::memory_order_relaxed);
		if (tmp == nullptr) {
			tmp = new Singleton;
			std::atomic_thread_fence(std::memory_order_release);
			m_instance.store(tmp, std::memory_order_relaxed);
		}
	}
	return tmp;
}
