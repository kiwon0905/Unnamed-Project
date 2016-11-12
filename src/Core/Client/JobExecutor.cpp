#include "JobExecutor.h"

void JobExecutor::addJob(const std::function<void()> & job, float period)
{
	m_jobs.push_back({ job, period, 0.f });
}

void JobExecutor::update(float dt)
{
	for (auto & job : m_jobs)
	{
		job.elapsed += dt;
		if (job.elapsed > job.period)
		{
			job.func();
			job.elapsed = 0.f;
		}
	}
}
