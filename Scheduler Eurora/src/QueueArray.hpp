/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                 File: QueueArray.hpp                                                 *
*                                                                                                                      *
*                                                                                                                      *
 **********************************************************************************************************************/

#ifndef Def_QueueArray
#define Def_QueueArray
#include <vector>
#include <string>
#include "QueueReader.hpp"
#include "Job.hpp"

class QueueArray
{
private:
	std::vector<Queue> _queue;
public:
	int getMaxMinutesToWait(std::string id);
	int getPriority(std::string id);
	QueueArray(std::string fQueue);
	QueueArray();
	int size(){return _queue.size();}
	Queue * getQueue(Job j);
	Queue& operator[] (int x) {return _queue[x];}
};

inline Queue * QueueArray::getQueue(Job j)
{

	for(unsigned int i=0;i<_queue.size();i++)
	{
		if(_queue[i].getId().compare(j.getQueue())==0)
		{
			return &_queue[i];
		}
	}
	for(unsigned int i=0;i<_queue.size();i++)
	{	
		if(_queue[i].getId().compare("debug")==0)
			return &_queue[i];
	}
	return &_queue[0];
}

/*inline QueueArray::QueueArray(JobArray reservation,JobArray jobs)
{
	
}*/

inline QueueArray::QueueArray()
{
	QueueReader r;
	_queue=r.readFromPBS();
}

inline QueueArray::QueueArray(std::string fQueue)
{
	QueueReader r(fQueue);
	_queue=r.read();
}

inline int QueueArray::getPriority(std::string id)
{
	for(unsigned int i=0;i<_queue.size();i++)
	{
		if(_queue[i].getId().compare(id)==0)
		{
			return _queue[i].getPriority();
		}
	}
	if(id[0]=='R')
	{
		for(unsigned int i=0;i<_queue.size();i++)
			if(_queue[i].getId().compare("reservation")==0)
				return _queue[i].getPriority();
	}
	else
	{
		for(unsigned int i=0;i<_queue.size();i++)
			if(_queue[i].getId().compare("others")==0)
				return _queue[i].getPriority();
	}
	return 0;
}

inline int QueueArray::getMaxMinutesToWait(std::string id)
{
	for(unsigned int i=0;i<_queue.size();i++)
		if(_queue[i].getId().compare(id)==0)
			return _queue[i].getMaxMinutesToWait();
	if(id[0]=='R')
	{
		for(unsigned int i=0;i<_queue.size();i++)
			if(_queue[i].getId().compare("reservation")==0)
				return _queue[i].getMaxMinutesToWait();
	}
	else
	{
		for(unsigned int i=0;i<_queue.size();i++)
			if(_queue[i].getId().compare("others")==0)
				return _queue[i].getMaxMinutesToWait();
	}
	return 0;
}
#endif
