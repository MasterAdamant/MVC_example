#pragma once

#include <functional>
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
#include <chrono>
#include <map>

class Task {
private:
	using string = std::string;
	using time_point = std::chrono::system_clock::time_point;
	inline static size_t counter = 0;
	
	string name, description;
	uint16_t priority = 0;
	time_point begin_point, end_point;

	bool is_closed = false, is_success = false;
	size_t id = 0;
public:
	Task() { ; }
	Task(const string &name, const string &desc, const time_point &begin_point, const time_point &end_point, const uint16_t& priority) noexcept {
		if (is_closed) {
			return;
		}
		id = ++counter;
		this->name = name;
		this->description = desc;
		this->priority = priority;
		if (begin_point>end_point) {
			this->begin_point = end_point;
			this->end_point = begin_point;
		}
		else {
			this->begin_point = begin_point;
			this->end_point = end_point;
		}
		Check();
	}

	string GetName() const noexcept {
		return name;
	}

	string GetDescription() const noexcept {
		return description;
	}

	uint16_t GetPriority() const noexcept {
		return priority;
	}

	time_point GetBeginPoint() const noexcept {
		return begin_point;
	}

	time_point GetEndPoint() const noexcept {
		return end_point;
	}

	bool IsClosed() const noexcept {
		return is_closed;
	}

	bool IsSuccess() const noexcept {
		return is_success;
	}

	size_t GetID() const noexcept {
		return id;
	}

	void Close(bool is_success) noexcept {
		if (is_closed) {
			return;
		}
		is_closed = true;
		this->is_success = is_success;
	}

	void Check() noexcept {
		if (is_closed) {
			return;
		}
		if (begin_point>=end_point) {
			Close(false);
		}
	}

	Task& operator=(const Task& other) {
		this->name = other.GetName();
		this->description = other.GetDescription();
		this->id = other.GetID();
		this->priority = other.GetPriority();
		this->begin_point = other.GetBeginPoint();
		this->end_point = other.GetEndPoint();
		return *this;
	}

	~Task() { ; }
};

std::ostream& operator<<(std::ostream& os, const Task& task) {
	os << "Task name: " << task.GetName() << '\n';
	os << "Task description: " << task.GetDescription() << '\n';
	os << "Task global ID: " << task.GetID() << '\n';
	os << "Task priority:" << task.GetPriority() << '\n';
	os << "Task begin time:" << std::chrono::system_clock::to_time_t(task.GetBeginPoint()) << '\n';
	os << "Task deadline:" << std::chrono::system_clock::to_time_t(task.GetEndPoint()) << '\n';
	os << "Task opened:" << task.IsClosed() << '\n';
	os << "Task completed:" << task.IsSuccess() << '\n';
	return os;
}

class TaskManager {
private:
	std::vector<Task> all_tasks;
public:
	
	//Filters
	static inline const std::function<bool(const Task&)> any_task = [](const Task&) {return true; };
	static inline const std::function<bool(const Task&)> closed_tasks = [](const Task& t) {return t.IsClosed(); };
	static inline const std::function<bool(const Task&)> completed_tasks = [](const Task& t) {return t.IsSuccess(); };
	
	//Sort options
	static inline const std::function<bool(const Task&, const Task&)> sort_by_id = [](const Task& i, const Task& t) {if (i.GetID()>t.GetID()) { return true; } return false; };
	static inline const std::function<bool(const Task&, const Task&)> sort_by_priority = [](const Task& i, const Task& t) {if (i.GetPriority() > t.GetPriority()) { return true; } return false; };

	TaskManager() { ; }
	
	TaskManager(std::vector<Task> tasks) {
		all_tasks = tasks;
	}
	
	~TaskManager() { ; }

	void AddTask(const Task& task){
		all_tasks.push_back(task);
	}

	std::vector<Task> GetTasks(std::function<bool(const Task&)> filter) const {
		std::vector<Task> result;
		for (Task t : all_tasks) {
			if (filter(t)) {
				result.push_back(t);
			}
		}
		return result;
	}

	std::vector<Task> GetSortedTasks(std::function<bool(const Task&, const Task&)> comparator) const {
		std::vector<Task> result = all_tasks;
		for (size_t i = 0; i < result.size(); ++i) {
			for (size_t t = i; t < result.size(); ++t) {
				if (comparator(result[i], result[t])) {
					Task temp = result [i];
					result[i] = result[t];
					result[t] = temp;
				}
			}
		}
		return result;
	}
};
std::ostream& operator<<(std::ostream& os, const TaskManager& task) {
	for (auto t : task.GetTasks(TaskManager::any_task)) {
		os << t;
	}
	return os;
}

std::istream& operator>>(std::istream& is, Task& task) {
	std::string name, desc;
			
	std::cout << "Name:";
	is >> name;
	
	std::cout << "Description:";
	std::getline(std::cin, desc);
	std::getline(std::cin, desc);
			
	std::cout << "Priority:";
	uint16_t prior;
	is >> prior;
	std::chrono::system_clock::time_point begin = std::chrono::system_clock::now();
	
	std::cout << "Duration:";
	size_t duration;
	is >> duration;
	std::chrono::system_clock::time_point end = std::chrono::system_clock::now() + std::chrono::hours(duration);
	task = Task(name, desc, begin, end, prior);
	return is;
}

class Organiser {
	private:
		Organiser() { ; }
		~Organiser() { ; }
		Organiser& operator= (const Organiser&) { ; }
		
		static inline TaskManager manager;

		static size_t Menu(const std::map<std::string, std::function<void()>>& options_list) {
			for (size_t i = 0; i < options_list.size(); ++i) {
				auto it = options_list.begin();
				std::advance(it, i);
				std::cout << i+1 << ". " << it->first << '\n';
			}
			std::cout << "Enter option number:";
			size_t input;
			std::cin >> input;
			input = (--input < options_list.size())?input:options_list.size()-1;
			auto it = options_list.begin();
			std::advance(it, input);
			it->second();
			return input;
		}

	public:

		static inline const std::map<std::string, std::function<void()>> view_options_list = {
			{"Sort by ID", []() { std::cout << manager.GetSortedTasks(TaskManager::sort_by_id); system("pause"); } },
			{"Sort by priority", []() { std::cout << manager.GetSortedTasks(TaskManager::sort_by_priority); system("pause"); }},
			{"Only completed", []() { std::cout << manager.GetTasks(TaskManager::completed_tasks); system("pause"); }},
			{"Only closed", []() { std::cout << manager.GetTasks(TaskManager::closed_tasks); system("pause"); }},
			{"All", []() { std::cout << manager.GetTasks(TaskManager::any_task); system("pause"); }}
		};
		static inline const std::map<std::string, std::function<void()>> tasks_options_list = {
			{"Add task",	[]() { Task task; std::cin>>task; manager.AddTask(task); }},
			{"View tasks",	[]() { Menu(view_options_list); }}
		};
		static inline const std::map<std::string, std::function<void()>> main_options_list = {
			{"Task management", []() {Menu(tasks_options_list); }},
			{"Exit", []() { exit(0); }}
		};
		
		static void SetManager(const TaskManager& value) {
			Organiser::manager = value;
		}

		static void Cycle() {
			while (true) {
				system("cls");
				Menu(main_options_list);
			}
		}
};