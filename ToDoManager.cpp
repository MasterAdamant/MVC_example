#include "TaskLib.h"
#include <chrono>

using namespace std;

int main() {
	Task hometask("Homework", "Do math", chrono::system_clock::now(), chrono::system_clock::time_point(), 1);
	Task project("Project", "Develop project", chrono::system_clock::time_point(), chrono::system_clock::time_point(), 3);
	Task job("Work", "Don't sleep", chrono::system_clock::time_point(), chrono::system_clock::time_point(), 2);
	TaskManager manager({hometask, project, job});

	Organiser::SetManager(manager);
	Organiser::Cycle();
	return 0;
}