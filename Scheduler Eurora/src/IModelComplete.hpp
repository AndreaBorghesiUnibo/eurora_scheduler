/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                              File: IModelComplete.hpp                                                *
*                                                                                                                      *
*                                                                                                                      *
 ***********************************************************************************************************************/

#ifndef Def_IModelComplete
#define Def_IModelComplete
#include "QueueArray.hpp"
#include "NodeArray.hpp"
#include "JobArray.hpp"
#include "ObjFDefine.hpp"
#include <vector>

class IModelComplete 
{
protected:
	double _os;
	double _wt;
	double _wtDelta;
	double _nl;
	double _nlDelta;
	double _mk;
	double _mkDelta;
	int _initialTime;
public:
	virtual JobArray solve(JobArray _jobs, QueueArray _queue, NodeArray _nodes,int refTime,int objectiveFunction,int * secondiEsecuzione,bool * risolto,int * totaleEsecuzione,bool * riprova,bool optionalResv,bool optionalJobs,bool print,time_t * modelCreationEnd,std::vector<bool>errorNodes)=0;
	virtual JobArray solveReservation(JobArray _jobs,JobArray _reservations, QueueArray _queue, NodeArray _nodes,int refTime,int objectiveFunction,int secondiEsecuzione)=0;
	void setWT(double wt,double wtDelta){_wt=wt;_wtDelta=wtDelta;}
	void setNumLate(double nl,double nlDelta){_nl=nl;_nlDelta=nlDelta;}
	void setMK(double mk,double mkDelta){_mk=mk;_mkDelta=mkDelta;}
	double getOptimalSolution(){return _os;}
	//void setJobArray(JobArray j){_jobs=j;}
	void setInitialTime(int time){_initialTime=time;}
	//JobArray getJobArray(){return _jobs;}
};
#endif
