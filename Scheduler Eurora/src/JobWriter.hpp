/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                 File: JobWriter.hpp                                                  *
*                                                                                                                      *
*                                                                                                                      *
 ***********************************************************************************************************************/

#ifndef Def_JobWriter
#define Def_JobWriter
#include <vector>
#include <iostream>
#include <fstream>
#include <time.h>
#include "Job.hpp"

class JobWriter
{
private:
	std::string _file;
public:
	JobWriter(std::string file);
	void write(std::vector<Job> jobs,int refTime);
	void write(std::vector<Job> jobs){write(jobs,0);}
};

inline JobWriter::JobWriter(std::string file)
{
	_file=file;
}


inline void JobWriter::write(std::vector<Job> jobs,int refTime)
{
	/*JobReader j(_file);
	std::vector<Job> old=j.read();
	for(unsigned int i=0;i<old.size();i++)
	{
		bool toInsert=true;
		for(unsigned int j=0;j<jobs.size();j++)
		{
			unsigned int pos;
			if((pos=jobs[j].getJobId().find(old[i].getJobId()))<jobs[j].getJobId().size())
			{
				toInsert = false;
			}
		}
		if(toInsert)
		{
			jobs.insert(jobs.begin(),old[i]);
		}
	}*/
	std::ofstream output(_file.c_str());
	if (!output) 
	{
		std::cout << "ERROR: Could not open file \"" << _file << "\"" << std::endl;
		exit(1);
	}
	
	//output<<"**Scheduling_number:"<<numIteration<<"**"<<std::endl;
	for (unsigned int i=0;i<jobs.size();i++)
	{
		output<<jobs[i].getJobId()<<";";
		output<<jobs[i].getJobName()<<";";
		output<<jobs[i].getUserName()<<";";
		output<<jobs[i].getQueue()<<";";
		output<<Util::timeToStr(jobs[i].getEnterQueueTime())<<"__";
		
		for(unsigned int j=0;j<jobs[i].getUsedNodes().size();j++)
		{
			std::vector<int> usedNodes=jobs[i].getUsedNodes();
			while(usedNodes[j]>=1)
			{
				output<<j<<";";
				output<<jobs[i].getNumberOfCores()/jobs[i].getNumberOfNodes()<<";";
				output<<jobs[i].getNumberOfGPU()/jobs[i].getNumberOfNodes()<<";";
				output<<jobs[i].getNumberOfMIC()/jobs[i].getNumberOfNodes()<<";";
				output<<jobs[i].getMemory()/jobs[i].getNumberOfNodes()<<"#";
				
				usedNodes[j]--;
			}
		}
		output<<"__"<<Util::timeToStr(jobs[i].getStartTime())<<";";
		output<<Util::timeToStr(jobs[i].getRealDuration()+jobs[i].getStartTime())<<";";
		output<<jobs[i].getNumberOfNodes()<<";";
		output<<jobs[i].getNumberOfCores()<<";";
		//output<<jobs[i].getNumberOfGPU()<<";";
		//output<<jobs[i].getNumberOfMIC()<<";";
		output<<jobs[i].getMemory()<<";";
		output<<Util::timeHHMMToStr(jobs[i].getEstimatedDuration())<<std::endl;
		
		
	}
	
	output.close();
}
#endif
