/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                  File: IModel.hpp                                                    *
*                                                                                                                      *
*                                                                                                                      *
 ***********************************************************************************************************************/

#ifndef Def_IModel
#define Def_IModel
#include "QueueArray.hpp"
#include "NodeArray.hpp"
#include "JobArray.hpp"

class IModel
{
	protected:
		QueueArray _queue;
		NodeArray _nodes;
		JobArray _jobs;
	public:
		virtual JobArray solve(int refTime)=0;
		void setJobArray(JobArray j){_jobs=j;}
	
};
#endif
