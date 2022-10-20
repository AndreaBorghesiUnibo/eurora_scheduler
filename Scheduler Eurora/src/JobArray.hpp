/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                  File: JobArray.hpp                                                  *
*                                                                                                                      *
*                                                                                                                      *
 ***********************************************************************************************************************/

#ifndef Def_JobArray
#define Def_JobArray
#include <vector>
#include <climits>
#include "JobReader.hpp"
#include "JobWriter.hpp"

class JobArray
{
private:
	std::vector<Job> _jobs;
	double _makespan;
	int min(int a,int b){return (a<b?a:b);}
public:
	int isPresent(std::string id)
	{
		for(unsigned int i=0;i<_jobs.size();i++)
		{
			unsigned int pos;
			if((pos=_jobs[i].getJobId().find(id))<_jobs[i].getJobId().size())
			{
				return i;
			}
		}
		return -1;
	}
	JobArray(NodeArray nodes)
	{
		JobReader r;
		std::vector<Job> empty;
		_jobs=r.readFromPBS(empty,nodes);
	}
	JobArray(JobArray old,NodeArray nodes)
	{
		JobReader r;
		/*std::vector<Job> nonResvOld;
		for(int i=0;i<old.size();i++)
		{
			if(!old[i].isReservation() || old[i].isReservationJob())
			{
				nonResvOld.push_back(old[i]);
			}
		}*/
		_jobs=r.readFromPBS(old.toArray(),nodes);
		std::vector<Job> resv=r.readReservations(old.toArray(),nodes);
		for(unsigned int i=0;i<resv.size();i++)
		{
			_jobs.push_back(resv[i]);
		}
	}
	JobArray(){}
	void remove(unsigned int i)
	{
		if(_jobs.size()>i)	
			_jobs.erase(_jobs.begin()+i);
	}
	void Add(Job j){_jobs.push_back(j);}
	std::vector<Job> toArray(){return _jobs;}
	std::vector<Job> * toArrayPointer(){ return & _jobs;}
	void setMakespan(double mksp){_makespan=mksp;}
	double getMakespan(){return _makespan;}
	JobArray(std::string fJob);
	JobArray(std::vector<Job> j){_jobs=j;}
	Job& operator[] (int x) {return _jobs[x];}
	void push_back(Job j){_jobs.push_back(j);}
	std::vector<int> getJobUtilizzation(int job){return _jobs[job].getUsedNodes();}
	int getJobStartTime(int job){return _jobs[job].getStartTime();}
	int size(){return _jobs.size();}
	void write(std::string fJob,int refTime);
	void write(std::string fJob){write(fJob,0);}
	int findNextIstant(int fromTime)
	{
		int minTime=INT_MAX;
		for(unsigned int i=0;i<_jobs.size();i++)
		{
			if(_jobs[i].getEnterQueueTime()>fromTime && _jobs[i].getEnterQueueTime()<minTime)
				minTime=_jobs[i].getEnterQueueTime();
			if(_jobs[i].getStartTime()+min(_jobs[i].getRealDuration(),_jobs[i].getEstimatedDuration())>fromTime && _jobs[i].getStartTime()+min(_jobs[i].getRealDuration(),_jobs[i].getEstimatedDuration())<minTime)
				minTime=_jobs[i].getStartTime()+min(_jobs[i].getRealDuration(),_jobs[i].getEstimatedDuration());
			//if(_jobs[i].getStartTime()+_jobs[i].getEstimatedDuration()>fromTime && _jobs[i].getStartTime()+_jobs[i].getEstimatedDuration()<minTime)
			//	minTime=_jobs[i].getStartTime()+_jobs[i].getEstimatedDuration();
		}
		if(minTime==INT_MAX)
			return -1;
		return minTime;
	}
};

inline JobArray::JobArray(std::string fJob)
{
	JobReader r(fJob);
	_jobs=r.read();
	
}



inline void JobArray::write(std::string fJob,int refTime)
{
	//JobWriter r(fJob);
	//r.write(_jobs,refTime);
}
#endif
