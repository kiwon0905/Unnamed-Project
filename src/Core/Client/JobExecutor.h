#pragma once

#include <functional>
#include <vector>

class JobExecutor
{
public:
	void addJob(const std::function<void()> & job, float period);
	void update(float dt);
private:
	struct Job
	{
		std::function<void()> func;
		float period;
		float elapsed;
	};
	std::vector<Job> m_jobs;
};