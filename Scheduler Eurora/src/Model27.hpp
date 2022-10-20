/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                   File: Model27.hpp                                                  *
*                                                                                                                      *
*                                                                                                                      *
 ***********************************************************************************************************************/

#ifndef Def_Model27
#define Def_Model27
#include "QueueArray.hpp"
#include "NodeArray.hpp"
#include "JobArray.hpp"
#include "IModelComplete.hpp"
#include "Instant.hpp"
#include <ilcp/cp.h>
#include <vector>
#include <stdio.h>
#include <typeinfo>

class Model27: public IModelComplete
{
private:
	double _secondsOfExecution;
	double _minTime;
	Instant * _intervals;
	int _makespanUB;
	
	int numberOfJointNodes(Job j,Node n,Queue * coda,NodeArray _nodes,std::vector<bool>errorNodes,int indexNode)
	{
		//std::cout<<"QUI1"<<std::endl;
		if(!n.isUsable() || (errorNodes[indexNode] && !j.isScheduled()) || (!n.isWorking()  && !j.isScheduled()))
		{
			//std::cout<<"Job "<<j.getJobId()<<" cannot run on node "<<n.getNodeName()<<" node not working "<<std::endl;
			return 0;
		}
		//std::cout<<"QUI2"<<std::endl;
		if(j.getGHz()>0 && n.getGHz()>0 && j.getGHz()!=n.getGHz())
		{
			//std::cout<<"Job "<<j.getJobId()<<" ghz "<<j.getGHz()<<" cannot run on node "<<n.getNodeName()<<" ghz "<<n.getGHz()<<std::endl;
			//getchar();
			return 0;
		}
		//std::cout<<"QUI3"<<std::endl;
		if(j.getHost().compare("")!=0 && j.getHost().compare(n.getNodeName())!=0)
		{
			//std::cout<<"Job "<<j.getJobId()<<" cannot run on node "<<n.getNodeName()<<" Different Host selection "<<std::endl;
			return 0;
		}
		//std::cout<<"QUI4"<<std::endl;
		int r=j.getNumberOfNodes();
		//std::cout<<"r1"<<j.getNumberOfNodes()<<std::endl;
		if(j.getNumberOfCores()!=0)
			r=Util::min(r,n.getCoresNumber()/(j.getNumberOfCores()/j.getNumberOfNodes()));
		//std::cout<<"r2"<<r<<std::endl;
		if(j.getNumberOfGPU()!=0)
			r=Util::min(r,n.getGPUsNumber()/(j.getNumberOfGPU()/j.getNumberOfNodes()));
		//std::cout<<"r3"<<r<<std::endl;
		if(j.getNumberOfMIC()!=0)
			r=Util::min(r,n.getMICsNumber()/(j.getNumberOfMIC()/j.getNumberOfNodes()));
		//std::cout<<"r4"<<r<<std::endl;
		if(j.getMemory()!=0)
			r=Util::min(r,n.getTotalMemory()/(j.getMemory()/j.getNumberOfNodes()));
		//std::cout<<"r5"<<r<<std::endl;

		//std::cout<<"QUI5"<<std::endl;
		if(!n.canAllocateToQueue(*coda))
		{
			r=0;
			//std::cout<<"Job "<<j.getJobId()<<" con coda "<<j.getQueue()<<" non puo' eseguire sul nodo "<<n.getNodeName()<<std::endl;
		}
		/*else
		{
			//std::cout<<"Posso allocare il job "<<j.getJobId()<<" con coda "<<j.getQueue()<<" chunk ";
			for(unsigned int i=0;i<coda->getDefaultChunk().size();i++)
			{
				std::cout<<coda->getDefaultChunk()[i]<<", ";
			}
			//std::cout<<" sul nodo "<<n.getNodeName()<<std::endl;
		}*/
		//std::cout<<"QUI6"<<std::endl;
		//getchar();
		/*if(r>1)
			r=1;*/
		return r;
	}

	int numberOfJointNodes(Job j,Node n,Queue * coda,NodeArray _nodes)
	{
		//std::cout<<"QUI1"<<std::endl;
		if(!n.isUsable() || ((!n.isWorking() )&& !j.isScheduled()))
		{
			//std::cout<<"Job "<<j.getJobId()<<" cannot run on node "<<n.getNodeName()<<" node not working "<<std::endl;
			return 0;
		}
		//std::cout<<"QUI2"<<std::endl;
		if(j.getGHz()>0 && n.getGHz()>0 && j.getGHz()!=n.getGHz())
		{
			//std::cout<<"Job "<<j.getJobId()<<" ghz "<<j.getGHz()<<" cannot run on node "<<n.getNodeName()<<" ghz "<<n.getGHz()<<std::endl;
			//getchar();
			return 0;
		}
		//std::cout<<"QUI3"<<std::endl;
		if(j.getHost().compare("")!=0 && j.getHost().compare(n.getNodeName())!=0)
		{
			//std::cout<<"Job "<<j.getJobId()<<" cannot run on node "<<n.getNodeName()<<" Different Host selection "<<std::endl;
			return 0;
		}
		//std::cout<<"QUI4"<<std::endl;
		int r=j.getNumberOfNodes();
		//std::cout<<"r1"<<j.getNumberOfNodes()<<std::endl;
		if(j.getNumberOfCores()!=0)
			r=Util::min(r,n.getCoresNumber()/(j.getNumberOfCores()/j.getNumberOfNodes()));
		//std::cout<<"r2"<<r<<std::endl;
		if(j.getNumberOfGPU()!=0)
			r=Util::min(r,n.getGPUsNumber()/(j.getNumberOfGPU()/j.getNumberOfNodes()));
		//std::cout<<"r3"<<r<<std::endl;
		if(j.getNumberOfMIC()!=0)
			r=Util::min(r,n.getMICsNumber()/(j.getNumberOfMIC()/j.getNumberOfNodes()));
		//std::cout<<"r4"<<r<<std::endl;
		if(j.getMemory()!=0)
			r=Util::min(r,n.getTotalMemory()/(j.getMemory()/j.getNumberOfNodes()));
		//std::cout<<"r5"<<r<<std::endl;

		//std::cout<<"QUI5"<<std::endl;
		if(!n.canAllocateToQueue(*coda))
		{
			r=0;
			//std::cout<<"Job "<<j.getJobId()<<" con coda "<<j.getQueue()<<" non puo' eseguire sul nodo "<<n.getNodeName()<<std::endl;
		}
		/*else
		{
			//std::cout<<"Posso allocare il job "<<j.getJobId()<<" con coda "<<j.getQueue()<<" chunk ";
			for(unsigned int i=0;i<coda->getDefaultChunk().size();i++)
			{
				std::cout<<coda->getDefaultChunk()[i]<<", ";
			}
			//std::cout<<" sul nodo "<<n.getNodeName()<<std::endl;
		}*/
		//std::cout<<"QUI6"<<std::endl;
		//getchar();
		/*if(r>1)
			r=1;*/
		return r;
	}
	
	void setMakespanUpperBound(JobArray _jobs,bool print)
	{
		int totale=_initialTime+7*24*60*60;
		for(int i=0;i<_jobs.size();i++)
		{
			totale+=_jobs[i].getRealDuration();
		}
		if(print)
			std::cout<<"Makespan Upper Bound "<<totale<<std::endl;
		_makespanUB= totale;
	}
	int getMakespanUpperBound()
	{
		return _makespanUB;
	}
	
	
	void setPrimeTimeIntervals(JobArray _jobs,IloEnv env,IloIntervalVarArray primeTimeIntervals)
	{
		//std::cout<<"Getting makespanUB"<<std::endl;
		int totale=getMakespanUpperBound();
		//std::cout<<"Getting Intervals"<<std::endl;
		std::vector< std::vector<int> > intervalli=_intervals->getIntervals(_initialTime,totale,true);
		//std::cout<<"Creating IloIntervalVar"<<std::endl;
		for(unsigned int i=0;i<intervalli[0].size();i++)
		{
			IloIntervalVar temp(env,intervalli[1][i]);
			temp.setStartMin(intervalli[0][i]);
			temp.setStartMax(intervalli[0][i]);
			//std::cout<<tempo+21600<<" - "<<tempo+21600+41400<<std::endl;
			primeTimeIntervals.add(temp);
			//tempo+=86400;
		}
	}
	
	void setNonPrimeTimeIntervals(JobArray _jobs,IloEnv env,IloIntervalVarArray nonPrimeTimeIntervals )
	{
		//std::cout<<"Getting makespanUB"<<std::endl;
		int totale=getMakespanUpperBound();
		//std::cout<<"Getting Intervals"<<std::endl;
		std::vector< std::vector<int> > intervalli=_intervals->getIntervals(_initialTime,totale,false);
		//std::cout<<"Creating IloIntervalVar"<<std::endl;
		for(unsigned int i=0;i<intervalli[0].size();i++)
		{
			IloIntervalVar temp(env,intervalli[1][i]);
			temp.setStartMin(intervalli[0][i]);
			temp.setStartMax(intervalli[0][i]);
			//std::cout<<tempo+21600<<" - "<<tempo+21600+41400<<std::endl;
			nonPrimeTimeIntervals.add(temp);
			//tempo+=86400;
		}
		/*int totale=makespanUpperBound(_jobs);
		int tempo=0;
		for(int i=_initialTime;i<=totale;i+=41400)
		{
			IloIntervalVar temp(env,21600);
			temp.setStartMin(tempo);
			temp.setStartMax(tempo);
			IloIntervalVar temp2(env,23400);
			temp.setStartMin(tempo+63000);
			temp.setStartMax(tempo+63000);
			//std::cout<<tempo+21600<<" - "<<tempo+21600+41400<<std::endl;
			nonPrimeTimeIntervals.add(temp);
			nonPrimeTimeIntervals.add(temp2);
			tempo+=86400;
		}*/
	}
	
public:
	Model27(){_minTime=2;_os=-1;_wt=-1;_wtDelta=-1;_nl=-1;_nlDelta=-1;_mk=-1;_mkDelta=-1;}
	/*Model27(QueueArray queue,NodeArray nodes,JobArray jobs){_queue=queue,_nodes=nodes,_jobs=jobs;_minTime=1;_os=-1;_wt=-1;_wtDelta=-1;_nl=-1;_nlDelta=-1;_mk=-1;_mkDelta=-1;}*/
	JobArray solve(JobArray _jobs, QueueArray _queue, NodeArray _nodes,int refTime,int objectiveFunction,int * secondiEsecuzione,bool * risolto,int * totaleEsecuzione,bool * riprova,bool optionalResv,bool optionalJobs,bool print,time_t * modelCreationEnd,std::vector<bool> errorNodes);
	JobArray solveReservation(JobArray _jobs,JobArray _reservations, QueueArray _queue, NodeArray _nodes,int refTime,int objectiveFunction,int secondiEsecuzione);
	//double getOptimalSolution(){return _os;}
	
	void setIntervals(Instant * intervalli)
	{
		/*primeTimeIntervals=intervalli[0];
		nonPrimeTimeIntervals=intervalli[1];*/
		//std::cout<<"MODEL.SETINTERVALS"<<std::endl;
		_intervals=intervalli;
	}
};

inline JobArray Model27::solve(JobArray _jobs, QueueArray _queue, NodeArray _nodes,int refTime,int objectiveFunction,int * secondiEsecuzione,bool * risolto,int * totaleEsecuzione,bool * riprova,bool optionalResv,bool optionalJobs,bool print,time_t * modelCreationEnd,std::vector<bool>errorNodes)
{
	bool solved = false;
	//time_t t0,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19;
	//time(&t0);
	//bool retry=false;
	_initialTime=refTime;
	//_secondsOfExecution=_minTime;
	_secondsOfExecution=*secondiEsecuzione;
	if(print)
		std::cout<<"secondi di esecuzione "<<*secondiEsecuzione<<std::endl;
	IloInt timeLimit = _secondsOfExecution;
	//int firstSolution=-1;
	try	
	{
		//do{
			IloEnv env; 
			
			IloModel model(env);
			IloIntervalVarArray task(env);
			std::vector< std::vector<IloIntervalVarArray> > UtilNodes(_jobs.size());
	
			//settaggio start time passati, start time reservation, min start time futuri e vincolo su modello makespan
			if(print)
				std::cout<<"Model27 IL NUMERO di JOBS E': "<<_jobs.size()<<"------------------------------------------"<<std::endl;
			setMakespanUpperBound(_jobs,print);
			
			for (int i = 0; i < _jobs.size(); i++) 
			{
				task.add(IloIntervalVar(env, _jobs[i].getRealDuration()));
				Queue * coda=_queue.getQueue(_jobs[i]);
				
				if(_jobs[i].isScheduled())// && !_jobs[i].isReservation())tolto per mettere l'opzionalità // && _jobs[i].getStartTime()<refTime)
				{
					//std::cout<<_jobs[i].getJobId()<<"JOB GIA SCHEDULATO---------------------------------------"<<std::endl;
					task[i].setStartMin(_jobs[i].getStartTime());
					task[i].setStartMax(_jobs[i].getStartTime());
					task[i].setPresent();
				}
				else
				{
					//std::cout<<_jobs[i].getJobId()<<"Job non ancora schedulato"<<std::endl;
					
					//std::cout<<"Coda "<<(_queue.getQueue(_jobs[i]))->getId()<<std::endl;
					if(_jobs[i].isReservation())
					{
						//std::cout<<"Reservation "<<_jobs[i].getJobId()<<Util::timeToStr(_jobs[i].getStartTime())<<std::endl;
						task[i].setStartMin(_jobs[i].getStartTime());
						task[i].setStartMax(_jobs[i].getStartTime());
						//getchar();
						if(optionalResv)
						{
							task[i].setOptional();
						}
					}
					else if(!coda->isEnabled())
					{
						//std::cout<<"Coda non abilitata "<<coda->getId()<<std::endl;
						task[i].setOptional();
						task[i].setAbsent();
					}
					else
					{
						if(optionalJobs)
						{
							//std::cout<<"OptionalJob"<<std::endl;
							task[i].setOptional();
						}
						if(_jobs[i].isNonPrimeTime())
						{
							//std::cout<<"JNP "<<refTime<<std::endl;
							task[i].setStartMin(refTime);
							IloIntervalVarArray noOverlapIntervals(env);
							setPrimeTimeIntervals(_jobs,env,noOverlapIntervals);
							noOverlapIntervals.add(task[i]);
							IloNoOverlap no(env,noOverlapIntervals);
							model.add(no);
							
						}
						else if(_jobs[i].isPrimeTime())
						{
							//std::cout<<"JP "<<refTime<<std::endl;
							task[i].setStartMin(refTime);
							IloIntervalVarArray noOverlapIntervals(env);
							setNonPrimeTimeIntervals(_jobs,env,noOverlapIntervals);
							noOverlapIntervals.add(task[i]);
							IloNoOverlap no(env,noOverlapIntervals);
							model.add(no);
						}
						else
						{
							//std::cout<<_jobs[i].getJobId()<<" Setto min ref time = "<<refTime<<std::endl;
							task[i].setStartMin(refTime);
						}
						//std::cout<<"Fine settaggio job con coda abilitata"<<std::endl;
					}
					
					//std::cout<<"Fine settaggio job non in esecuzione "<<this<<" "<<&env<<&model<<&task<<&task[0]<<std::endl;
				}
				//std::cout<<"if(_mk>0 && _mkDelta>0)"<<std::endl;
				//std::cout<<_mk<<" "<<_mkDelta<<std::endl;
				if(_mk>0 && _mkDelta>0 && _jobs[i].isAnytime())
				{
					//std::cout<<"Add vincolo makespan start max "<<_mk*_mkDelta-_jobs[i].getRealDuration()<<std::endl;
					task[i].setStartMax(_mk*_mkDelta-_jobs[i].getRealDuration());
				}
				//std::cout<<"Fine settaggio job"<<std::endl;
				UtilNodes[i]=std::vector<IloIntervalVarArray>(_nodes.size());
				std::vector<int> usedNodes=_jobs[i].getUsedNodes();
				
				
				IloIntervalVarArray c(env);
				//std::cout<<"numero nodi "<<_nodes.size()<<std::endl;
				for(int j=0;j<_nodes.size();j++)
				{
					//if(_nodes[j].isUsable())
					//{
					UtilNodes[i][j]=IloIntervalVarArray(env);
					//std::cout<<"Inizio loop"<<std::endl;
					//std::cout<<"numero virtual job usabili sul nodo "<<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)<<std::endl;
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes,errorNodes,j);k++)
					{
						UtilNodes[i][j].add(IloIntervalVar(env, _jobs[i].getRealDuration()));
						if(_jobs[i].isScheduled() || (_jobs[i].isReservation() && _jobs[i].getStartTime()<=refTime))// && !_jobs[i].isReservation())// && _jobs[i].getStartTime()<refTime) 
						{
							//std::cout<<"E' gia' stato schedulato "<<_jobs[i].getJobId()<<" "<<_jobs[i].isScheduled()<<std::endl;

							if(usedNodes[j]>=1)
							{
								UtilNodes[i][j][k].setStartMin(_jobs[i].getStartTime());
								UtilNodes[i][j][k].setStartMax(_jobs[i].getStartTime());
								usedNodes[j]--;
							}
							else
							{
								UtilNodes[i][j][k].setAbsent();
							}
						}
						else
						{ 
							//std::cout<<"NON e' gia' stato schedulato "<<_jobs[i].getJobId()<<" "<<_jobs[i].isScheduled()<<std::endl;
							UtilNodes[i][j][k].setOptional();
							if(!_jobs[i].isReservation())
								UtilNodes[i][j][k].setStartMin(refTime);
						}
						c.add(UtilNodes[i][j][k]);
					}
					//std::cout<<"Fine loop1"<<std::endl;
					
				}

				//alternative
				/*for(int j=0;j<_nodes.size();j++)
				{
					
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						c.add(UtilNodes[i][j][k]);
					}
					
				}*/
				IloAlternative a1(env,task[i],c,_jobs[i].getNumberOfNodes());
				model.add(a1);
			} 
			//std::cout<<"FINITO TASK"<<std::endl;
			
	//time(&t1);
			//vincolo sincronizzazione job eterogenei
			for (int i = 0; i < _jobs.size(); i++) 
			{
				unsigned int pos=INT_MAX;
				if((pos=_jobs[i].getJobId().find("("))<_jobs[i].getJobId().size())
				{
					
					std::string first=_jobs[i].getJobId().substr(0,pos);
					IloIntervalVarArray synchronizeEterogenei(env);
					for(int j=i+1; j<_jobs.size(); j++)
					{
						std::string second=_jobs[j].getJobId().substr(0,_jobs[j].getJobId().find("("));
						//std::cout<<"Comparing eterogenei "<<first<<" =?= "<<second;
						if(first.compare(second)==0)
						{
							//std::cout<<"Comparing eterogenei "<<first<<" =?= "<<second<<" YES!!!!!!!!!!"<<std::endl;
							synchronizeEterogenei.add(task[j]);
						}
						//std::cout<<std::endl;
					}
					IloSynchronize sync(env,task[i],synchronizeEterogenei);
					model.add(sync);
				}
			}

	//time(&t2);
//std::cout<<"FINITO SINC ETEROGENEI"<<std::endl;
			//vincolo su code realtime
			for (int i = 0; i < _jobs.size(); i++) 
			{
				Queue * coda=_queue.getQueue(_jobs[i]);
				if(! _jobs[i].isReservation() && _jobs[i].isAnytime() && !_jobs[i].isScheduled() && coda->getPriority()>=1022)
				{
					for (int j = 0; j < _jobs.size(); j++) 
					{
						if(i!=j && !_jobs[j].isScheduled())
						{
							Queue * codaJ=_queue.getQueue(_jobs[j]);
							if(codaJ->getPriority()<coda->getPriority())
							{
								//std::cout<<"Vincolo start before start "<<_jobs[i].getJobId()<<" "<<_jobs[j].getJobId()<<std::endl;
								model.add(IloStartBeforeStart(env,task[i],task[j]));
							}
						}
					}
				}
			}

	//time(&t3);
//std::cout<<"FINITO REALTIME"<<std::endl;
			//vincolo su modello num in late
			if(_nl>0 && _nlDelta>0)
			{
				//std::cout << "AAAARRRGHHHH!!!" << std::endl;
				IloNumExprArray nlSum(env);
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					if( _jobs[i].isAnytime())
					{
						nlSum.add(IloStartOf(task[i])-_jobs[i].getEnterQueueTime()>_queue.getMaxMinutesToWait(_jobs[i].getQueue()));
						//std::cout<<"Add vincolo num late SUM start-EQT("<<_jobs[i].getEnterQueueTime()<<") > maximumWait("<<_queue.getMaxMinutesToWait(_jobs[i].getQueue())<<") <= nl*delta("<<_nl<<"*"<<_nlDelta<<")"<<std::endl;
					}
				}
				model.add(IloSum(nlSum) <= _nl*_nlDelta);
			}
	
	//time(&t4);
//std::cout<<"FINITO NUL late"<<std::endl;
			// vincolo su modello weighted tardiness
			if(_wt>0 && _wtDelta>0)
			{
				//std::cout << "AAAARRRGHHHH!!!" << std::endl;
				IloNumExprArray wtSum(env);
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					
					if( _jobs[i].isAnytime())
					{
						IloNum wait=_queue.getMaxMinutesToWait(_jobs[i].getQueue());
						wtSum.add(IloMax((IloStartOf(task[i])-_jobs[i].getEnterQueueTime()-wait)/wait,0));
						//std::cout<<"Add vincolo weight t MAX start-EQT("<<_jobs[i].getEnterQueueTime()<<") -wait("<<_queue.getMaxMinutesToWait(_jobs[i].getQueue())<<") / wait("<<_queue.getMaxMinutesToWait(_jobs[i].getQueue())<<") <= wt*delta("<<_wt*_wtDelta<<")"<<std::endl;
					}
				}
				model.add(IloSum(wtSum) <= _wt*_wtDelta);
			}

	//time(&t5);
//std::cout<<"FINITO wt"<<std::endl;
			//creazione matrice utilizzazione e settaggio start time passato e min start time futuro
			
			//std::cout<<"UtilNodes creation per "<<_jobs.size()<<" jobs"<<std::endl;
			/*for (int i = 0; i < _jobs.size(); i++) 
			{
				UtilNodes[i]=std::vector<IloIntervalVarArray>(_nodes.size());
				std::vector<int> usedNodes=_jobs[i].getUsedNodes();
				
				//std::cout<<"numero nodi "<<_nodes.size()<<std::endl;
				for(int j=0;j<_nodes.size();j++)
				{
					//if(_nodes[j].isUsable())
					//{
					UtilNodes[i][j]=IloIntervalVarArray(env);
					//std::cout<<"Inizio loop"<<std::endl;
					//std::cout<<"numero virtual job usabili sul nodo "<<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)<<std::endl;
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						UtilNodes[i][j].add(IloIntervalVar(env, _jobs[i].getRealDuration()));
						if(_jobs[i].isScheduled())// && !_jobs[i].isReservation())// && _jobs[i].getStartTime()<refTime) 
						{
							//std::cout<<"E' già stato schedulato "<<_jobs[i].getJobId()<<" "<<_jobs[i].isScheduled()<<std::endl;

							if(usedNodes[j]>=1)
							{
								UtilNodes[i][j][k].setStartMin(_jobs[i].getStartTime());
								UtilNodes[i][j][k].setStartMax(_jobs[i].getStartTime());
								usedNodes[j]--;
							}
							else
							{
								UtilNodes[i][j][k].setAbsent();
							}
						}
						else
						{ 
							//std::cout<<"NON è già stato schedulato "<<_jobs[i].getJobId()<<" "<<_jobs[i].isScheduled()<<std::endl;
							UtilNodes[i][j][k].setOptional();
							if(!_jobs[i].isReservation())
								UtilNodes[i][j][k].setStartMin(refTime);
						}
					}
					//std::cout<<"Fine loop1"<<std::endl;
					
				}
				//std::cout<<"Fine loop2"<<std::endl;	
			}*/

	//time(&t6);
			//std::cout<<"Inizio altervnative"<<std::endl;
			//alternative utilizzazione nodi
			/*for(int i=0;i<_jobs.size();i++)
			{
				IloIntervalVarArray c(env);
				for(int j=0;j<_nodes.size();j++)
				{
					
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						c.add(UtilNodes[i][j][k]);
					}
					
				}
				IloAlternative a1(env,task[i],c,_jobs[i].getNumberOfNodes());
				model.add(a1);
			}*/
			//std::cout<<"Fine altervnative"<<std::endl;

		
	//time(&t7);
			//std::cout<<"Inizio Cumul core"<<std::endl;
			//cumulfunction cores
			IloCumulFunctionExprArray usgCores(env); 
			IloCumulFunctionExprArray usgGPU(env); 
			IloCumulFunctionExprArray usgMIC(env); 
			IloCumulFunctionExprArray usgMem(env); 
			for (int j = 0; j < _nodes.size(); j++) 
			{
				//if(!errorNodes[j])
				//{
					usgCores.add(IloCumulFunctionExpr(env));
					usgGPU.add(IloCumulFunctionExpr(env));
					usgMIC.add(IloCumulFunctionExpr(env));
					usgMem.add(IloCumulFunctionExpr(env));
					for (int i = 0; i < _jobs.size(); i++) 
					{
						
						for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes,errorNodes,j);k++)
						{
							usgCores[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfCores()/_jobs[i].getNumberOfNodes());
							usgGPU[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfGPU()/_jobs[i].getNumberOfNodes());
							usgMIC[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfMIC()/_jobs[i].getNumberOfNodes());
							usgMem[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getMemory()/_jobs[i].getNumberOfNodes());
						}
					}
					if(!errorNodes[j])
					{
						model.add(usgCores[j] <= _nodes[j].getCoresNumber());
						model.add(usgGPU[j] <= _nodes[j].getGPUsNumber());
						model.add(usgMIC[j] <= _nodes[j].getMICsNumber());
						model.add(usgMem[j] <= _nodes[j].getTotalMemory());
					}
				//}		
			}
			//std::cout<<"Fine Cumul core"<<std::endl;

			
	//time(&t8);
			//std::cout<<"Inizio Cumul gpu"<<std::endl;
			//cumulfunction gpu
			/*IloCumulFunctionExprArray usgGPU(env); 
			for (int j = 0; j < _nodes.size(); j++) 
			{
				usgGPU.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						usgGPU[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfGPU()/_jobs[i].getNumberOfNodes());
					}
					//std::cout<<"Limite gpu "<<j<<" "<<_jobs[i].getNumberOfGPU()<<"/"<<_jobs[i].getNumberOfNodes()<<" <= "<<_nodes[j].getGPUsNumber()<<std::endl;
				}
				model.add(usgGPU[j] <= _nodes[j].getGPUsNumber());
				
			}*/
			//std::cout<<"Fine Cumul gpu"<<std::endl;
	//time(&t9);
			//std::cout<<"Inizio Cumul mic"<<std::endl;
			//cumulfunction mic
			//IloCumulFunctionExprArray usgMIC(env); 
			/*for (int j = 0; j < _nodes.size(); ++j) 
			{
				usgMIC.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						usgMIC[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfMIC()/_jobs[i].getNumberOfNodes());
					}
					//std::cout<<"Limite mic "<<j<<" "<<_jobs[i].getNumberOfMIC()/_jobs[i].getNumberOfNodes()<<" <= "<<_nodes[j].getMICsNumber()<<std::endl;
				}
				model.add(usgMIC[j] <= _nodes[j].getMICsNumber());
				
			}*/
			//std::cout<<"Fine Cumul mic"<<std::endl;
			//std::cout<<"Inizio Cumul mem"<<std::endl;
		
	//time(&t10);
			//cumulfunction mem
			/*IloCumulFunctionExprArray usgMem(env); 
			for (int j = 0; j < _nodes.size(); ++j) 
			{
				
				usgMem.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						usgMem[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getMemory()/_jobs[i].getNumberOfNodes());
					
					}
					//std::cout<<"Limite mem "<<j<<" "<<_jobs[i].getMemory()/_jobs[i].getNumberOfNodes()<<" <= "<<_nodes[j].getTotalMemory()<<std::endl;
				}
				model.add(usgMem[j] <= _nodes[j].getTotalMemory());
				
			}*/
			//std::cout<<"Fine Cumul mem"<<std::endl;

			//inserimento funzioni obiettivo

	//time(&t11);
			//std::cout<<"Inizio inserimento funzioni obiettivo"<<std::endl;
			IloIntExprArray delayNL(env);
			IloNumExprArray delayT(env);
			IloIntExprArray ends(env);
			int minEqt=INT_MAX;
			switch(objectiveFunction)
			{
				case NO_OBJ:
				break;
				case MAX_JOBS:
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					delayNL.add(IloPresenceOf(env,task[i]));
				}
				model.add(IloMaximize(env, IloSum(delayNL)));
				break;
				case NUM_JOB_IN_LATE_OBJF: // define the problem objective: num in late
				
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					delayNL.add(IloStartOf(task[i])-_jobs[i].getEnterQueueTime()>_queue.getMaxMinutesToWait(_jobs[i].getQueue()));
				}
				model.add(IloMinimize(env, IloSum(delayNL)));
				break;
				
				case WEIGHTED_TARDINESS_OBJF: // define the problem objective: weight tard
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					IloInt wait=_queue.getMaxMinutesToWait(_jobs[i].getQueue());
					IloInt priority=_queue.getPriority(_jobs[i].getQueue());
					IloIntExpr ex=IloMax((IloStartOf(task[i])-_jobs[i].getEnterQueueTime()-wait)*priority,0);
					delayT.add(ex);
				}
				model.add(IloMinimize(env, IloSum(delayT)));
				break;
				

				case WEIGHTED_QUEUE_TIME_OBJF: // define the problem objective: weight tard
				/*for(int i=0;i<_queue.size();i++)
				{	
					attese.push_back(_queue[i].getMaxMinutesToWait());
					if(attese[i]<minWait)
					{	
						minWait=attese[i];
					}
				}*/
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					IloInt priority=_queue.getPriority(_jobs[i].getQueue());
					IloIntExpr start=IloStartOf(task[i]);
					IloInt qt=_jobs[i].getEnterQueueTime();
					IloIntExpr num=start-qt;
					IloIntExpr ex=num*priority;
					delayT.add(ex);
					//std::cout<<"QT (S-"<<qt<<")*"<<priority<<" ";
				}
				//std::cout<<std::endl;
				model.add(IloMinimize(env, IloSum(delayT)));
				break;
				case MAKESPAN_OBJF: // define the problem objective: makespan
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					if(_jobs[i].getEnterQueueTime()<minEqt)
						minEqt=_jobs[i].getEnterQueueTime();
				}
				for (int i = 0; i < _jobs.size(); ++i) {
					ends.add(IloEndOf(task[i])-minEqt);
				}
				model.add(IloMinimize(env, IloMax(ends)));
				break;
				
				case TEMPERATURE_RANKING_OBJF: // define the problem objective: temperature ranking
				break;
				
				default:
				break;
			}
	//time(&t12);
			//std::cout<<"Inizio inserimento funzioni obiettivo"<<std::endl;
			// ================================
			// = Bulid and configure a solver =
			// ================================
		
			// build a solver
			IloCP cp(model);
			//std::cout<<"IloCP isntaziato"<<std::endl;
			/*if(print)
			{
				if(cp.refineConflict())
				{
			//std::cout<<"refining conflict"<<std::endl;
					cp.writeConflict(cp.out());
					cp.printInformation(std::cout);
				}
			}*/
	//time(&t13);
			//std::cout<<"end refining conflict"<<std::endl;
			/*
			for(int i=0;i<_jobs.size();i++)
			{
				std::cout<<_jobs[i].getJobId()<<" "<<cp.domain(task[i])<<std::endl;
			}
			for(int i=0;i<_jobs.size();i++)
			{
				std::vector<int> nodes(_nodes.size(),0);
				for(int j=0;j<_nodes.size();j++)
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)&& k<1;k++)
					{
						std::cout<<cp.domain(UtilNodes[i][j][0])<<std::endl;
					}
				}
			}
				std::cout<<model<<std::endl;
	
				//getchar();
			}*/
			//std::cout << "MODEL: " << model << std::endl;

			// USE A SINGLE PROCESSOR (DO NOT TOUCH THIS)
			cp.setParameter(IloCP::Workers, 4);
	
			// DEFINING A TIME LIMIT
			// other limit types are described the solver documentation
			timeLimit = _secondsOfExecution;
			if(print)
				std::cout<<"cp.setParameter IloCP::TimeLimit "<<timeLimit<<std::endl;
			cp.setParameter(IloCP::TimeLimit, timeLimit);
			//getchar();
			// LOG EVERY N BRANCHES:
			// cp.setParameter(IloCP::LogPeriod, 100000);
	
			// DO NOT LOG AT ALL
			cp.setParameter(IloCP::LogVerbosity, IloCP::Quiet);

			// SET SEARCH TYPE TO DSF
			// keep commented to use te default search
			// other search modes are available: check the documentation
			cp.setParameter(IloCP::SearchType, IloCP::DepthFirst);//IloCP::DepthFirst IloCP:Restart IloCP::MultiPoint IloCP::Auto 

			// SET FILTERING STRENGTH FOR "ALL" CONSTRAINTS
			// other strength values are available from the documentation
			cp.setParameter(IloCP::DefaultInferenceLevel, IloCP::Low); //IloCP::Low, IloCP::Basic, IloCP::Medium, and IloCP::Extended
	
			// SET FILTERING STRENGTH FOR THE CUMULATIVE CONSTRAINTS ONLY
			// other strength values are available from the documentation
			cp.setParameter(IloCP::CumulFunctionInferenceLevel, IloCP::Extended);
	
			// =====================
			// = Solve the problem =
			// =====================
			time(modelCreationEnd);
	//time(&t14);
			// start to search
			cp.startNewSearch();
			//if(print)
			//	cp.printInformation(std::cout);
			//std::cout<<"Sono qui "<<_jobs.size() <<std::endl;
			/*for(int i=0;i<_jobs.size();i++)
			{
				//std::cout<<"Entro loop1"<<std::endl;
				std::cout<<"Task "<<cp.domain(task[i])<<std::endl;
				std::vector<int> nodes(_nodes.size(),0);
				for(int j=0;j<_nodes.size();j++)
				{
				//std::cout<<"    Entro loop2"<<std::endl;
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
				//std::cout<<"        Entro loop3 "<<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)<<std::endl;
						std::cout<<"\tUtil "<<cp.domain(UtilNodes[i][j][k])<<std::endl;
					}
				}
			}*/
	//time(&t15);
			// loops until there are solution
			//std::cout<<"while (cp.next())"<<std::endl;
			while (cp.next()) 
			{
				
				// "remember" that at least a solution was found
				if(!solved)
				{
					*secondiEsecuzione=cp.getInfo(IloCP::SolveTime);
					//std::cout<<"SETTO *secondiEsecuzione="<<cp.getInfo(IloCP::SolveTime)<<" "<<*secondiEsecuzione<<std::endl;				
				}
				*riprova=false;
				solved = true;
				//retry=false;
				_secondsOfExecution=_minTime;
				
				if(print)
					std::cout << "***** SOLUTION FOUND ***** time: " << cp.getInfo(IloCP::SolveTime) <<" valore soluzione: " << cp.getObjValue() << std::endl;
				/*if(firstSolution==-1)
				{
					firstSolution=	cp.getInfo(IloCP::SolveTime);
					*secondiEsecuzione=cp.getInfo(IloCP::SolveTime);
				}*/
				/*DE-COMMENT THIS TO SEE HOW THE SOLUTION LOOKS LIKE
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					std::cout << _jobs[i].tostd::string(0,0)<<std::endl;
				//std::cout << cp.domain(task[i])<<std::endl;
				}*/
				
				
				if(cp.getObjValue()!=0)
				{
					_os=cp.getObjValue();
					if(objectiveFunction==MAKESPAN_OBJF)
						_os+=minEqt;
				}
				else
				{
					_os=1;
				}
				if(cp.getInfo(IloCP::SolveTime)>10 || timeLimit>60)
					break;
		//std::cout<<"1"<<std::endl;
			}
	//time(&t16);
		//std::cout << "1 e 1/2" << std::endl;
			if(solved)
			{
		//std::cout<<"2"<<std::endl;
				//std::cout<<"SOLVED"<<std::endl;
				//getchar();
				*totaleEsecuzione=cp.getInfo(IloCP::SolveTime);
				for(int i=0;i<_jobs.size();i++)
				{
		//std::cout<<"3"<<std::endl;
					std::vector<int> nodes(_nodes.size(),0);
					for(int j=0;j<_nodes.size();j++)
					{
		//std::cout<<"4"<<std::endl;
						for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes,errorNodes,j);k++)
						{
		//std::cout<<"5"<<std::endl;
							if(cp.isPresent(UtilNodes[i][j][k]))
							{
		//std::cout<<"6"<<std::endl;
								nodes[j]++;
		//std::cout<<"7"<<std::endl;
							}
					
						}
					}
					if(cp.isPresent(task[i]))
					{
						_jobs[i].setFeasibility(_nodes.getStates(),1);
						//std::cout<<"Job "<<_jobs[i].getJobId()<<" is present "<<_jobs[i].getFeasibility(_nodes.getStates())<<std::endl;
					}
					else
					{
						_jobs[i].setFeasibility(_nodes.getStates(),0);
						//std::cout<<"Job "<<_jobs[i].getJobId()<<" is not present "<<_jobs[i].getFeasibility(_nodes.getStates())<<std::endl;
					}
					//getchar();
					_jobs[i].allocate(cp.getStart(task[i]),nodes);
				}
			}

	//time(&t17);
			// terminate search
			cp.endSearch();
			
			//se la ricerca non ha prodotto risultati a causa di un timeLimit troppo std::stringente: aumento il time limit e ritento, 
			if (!solved && cp.getInfo(IloCP::SolveTime) >= timeLimit) 
			{
				*riprova=true;
				*totaleEsecuzione=timeLimit;
				//_secondsOfExecution*=5;
				//std::cout << "changed time limit: min " << _secondsOfExecution << std::endl;
				//se il time limit eccede i 125 minuti ritorno comunque la soluzione precedente (in futuro avrò meno jobs e riuscirò a finire uno scheduling)
				/*std::cout <<"Task:"<<std::endl;
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					//std::cout << _jobs[i].tostd::string(0,0)<<std::endl;
					std::cout << cp.domain(task[i])<<std::endl;
				}
				std::cout <<"Util:"<<std::endl;
				for(int i=0;i<_jobs.size();i++)
				{
					std::vector<int> nodes(_nodes.size(),0);
					for(int j=0;j<_nodes.size();j++)
					{
						for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)&& k<1;k++)	
						{
							std::cout<<cp.domain(UtilNodes[i][j][0])<<std::endl;
						}
					}
				}*/
				//getchar();
				if(_secondsOfExecution>=1250)
				{

					*riprova=false;
					//*totaleEsecuzione=timeLimit;
				/*std::cout <<"Task:"<<std::endl;
					for (int i = 0; i < _jobs.size(); ++i) 
					{
						//std::cout << _jobs[i].tostd::string(0,0)<<std::endl;
						std::cout << cp.domain(task[i])<<std::endl;
					}
				std::cout <<"Util:"<<std::endl;
					for(int i=0;i<_jobs.size();i++)
					{
						std::vector<int> nodes(_nodes.size(),0);
						for(int j=0;j<_nodes.size();j++)
						{
							for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)&& k<1;k++)
							{
								std::cout<<cp.domain(UtilNodes[i][j][0])<<std::endl;
							}
						}
					}*/
					
				}
				//getchar();
			}
			else if(!solved) //se la ricerca termina perché non sono presenti soluzioni ammissibili
			{
				*riprova=false;
				*totaleEsecuzione=timeLimit;
				if(print)
					std::cout << "THE PROBLEM IS INFEASIBLE, time "<<cp.getInfo(IloCP::SolveTime)<<">"<<timeLimit << std::endl;
				std::cout <<"Task:"<<std::endl;
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					std::cout << _jobs[i].toString(0,0)<<std::endl;
					std::cout << cp.domain(task[i])<<std::endl;
				}
				/*std::cout <<"Util:"<<std::endl;
				for(int i=0;i<_jobs.size();i++)
				{
					std::vector<int> nodes(_nodes.size(),0);
					for(int j=0;j<_nodes.size();j++)
					{
						for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)&& k<1;k++)
						{
							std::cout<<cp.domain(UtilNodes[i][j][0])<<std::endl;
						}
					}
				}*/
				//getchar();
				//break;
			}
	//time(&t18);
	
			// DE-COMMENT THIS IF YOU ARE NOT USING LOGGING
			//cp.printInformation();
			
			
			*risolto=solved;
			env.end();
			
		//}while(retry);
		
	}
	catch (IloException& ex) 
	{
		std::string m(ex.getMessage());
		if(m.find("Empty array in expression")==std::string::npos) //se l'eccezione è Empty array in expression allora è una eventualità normalissima (sono finiti i job in coda)
		{
			
			std::cout << "Error: " << m <<". Exception type " <<typeid(ex).name()<< std::endl;
		
			//getchar();
			exit(0);
		}
	}
	//time(&t19);
	//std::cout<<"OverModel t1_"<<t1-t0<<" t2_"<<t2-t1<<" t3_"<<t3-t2<<" t4_"<<t4-t3<<" t5_"<<t5-t4<<" t6_"<<t6-t5<<" t7_"<<t7-t6<<" t8_"<<t8-t7<<" t9_"<<t9-t8<<" t10_"<<t10-t9<<" t11_"<<t11-t10<<" t12_"<<t12-t11<<" t13_"<<t13-t12<<" t14_"<<t14-t13<<" t15_"<<t15-t14<<" t16_"<<t16-t15<<" t17_"<<t17-t16<<" t18_"<<t18-t17<<" t19_"<<t19-t18<<std::endl;
	//*secondiEsecuzione=firstSolution;
	return _jobs;
}

inline JobArray Model27::solveReservation(JobArray _jobs,JobArray _reservations, QueueArray _queue, NodeArray _nodes,int refTime,int objectiveFunction,int secondiEsecuzione)
{
	bool solved = false;
	bool retry=false;

	_initialTime=refTime;
	_secondsOfExecution=secondiEsecuzione;
	std::cout<<"solveReservation secondi di esecuzione "<<secondiEsecuzione<<std::endl;
	//std::cout<<"Model27Resv IL NUMERO di JOBS E': "<<_jobs.size()<<"------------------------------------------"<<std::endl;
	IloInt timeLimit = _secondsOfExecution;
	int firstSolution=-1;
	int iteratoreReservation=0;
	while(refTime>_reservations[iteratoreReservation].getStartTime()+_reservations[iteratoreReservation].getRealDuration())
	{
		//std::cout<<refTime<<" MAGGIORE DI "<<_reservations[iteratoreReservation].getStartTime()+_reservations[iteratoreReservation].getRealDuration()<<std::endl;
		iteratoreReservation++;
	}	
	int numberOfOptionals=0;
	for(int i=0;i<_jobs.size();i++)
		if(!_jobs[i].isScheduled())
			numberOfOptionals++;
	int precOptionals=numberOfOptionals;
	
	try	
	{
		do{
			std::cout<<"Number of optionals "<<numberOfOptionals<<std::endl;
			IloEnv env; 
			
			IloModel model(env);
			IloIntervalVarArray task(env);

			//settaggio start time passati, start time reservation, min start time futuri e vincolo su modello makespan
			
			std::cout<<"IL NUMERO di JOBS E': "<<_jobs.size()<<" numero reservations "<< _reservations.size()<<" iterazione "<<iteratoreReservation<<"------------------------------------------"<<std::endl;
			for (int i = 0; i < _jobs.size(); i++) 
			{
				task.add(IloIntervalVar(env, _jobs[i].getRealDuration()));
				//Queue * coda=_queue.getQueue(_jobs[i]);
				
				if(_jobs[i].isScheduled())// && _jobs[i].getStartTime()<refTime)
				{
					std::cout<<"JOB GIA SCHEDULATO---------------------------------------"<<std::endl;
					task[i].setStartMin(_jobs[i].getStartTime());
					task[i].setStartMax(_jobs[i].getStartTime());
					if(_jobs[i].getStartTime()+_jobs[i].getRealDuration()<_reservations[iteratoreReservation].getStartTime())
					{
						task[i].setAbsent();
						std::cout<<"----->Job "<<i<<" setto absent perchè in reservation precedente"<<std::endl;
					}
				}
				else
				{
					std::cout<<"Job non ancora schedulato"<<std::endl;
					task[i].setOptional();
					/*if(_reservations[iteratoreReservation].isScheduled())//if(!coda->isEnabled())
					{
						std::cout<<"Reservation non schedulata "<<_reservations[iteratoreReservation].getJobId()<<std::endl;
						task[i].setAbsent();
					}
					else
					{*/
						
						int minTime=refTime>_reservations[iteratoreReservation].getStartTime()?refTime:_reservations[iteratoreReservation].getStartTime();
						std::cout<<"Setto start min = "<<minTime<<" end max = "<<_reservations[iteratoreReservation].getStartTime()+_reservations[iteratoreReservation].getRealDuration()+5<<" ID "<<_jobs[i].getJobId()<<std::endl;
						task[i].setStartMin(minTime);
						task[i].setStartMax(minTime);
						task[i].setEndMax(_reservations[iteratoreReservation].getStartTime()+_reservations[iteratoreReservation].getRealDuration()+5);
						std::cout<<"Fine settaggio job con coda abilitata"<<std::endl;
					//}
					
					//std::cout<<"Fine settaggio job non in esecuzione "<<this<<" "<<&env<<&model<<&task<<&task[0]<<std::endl;
				}
				//std::cout<<"Fine settaggio job"<<std::endl;
			} 
			//std::cout<<"FINITO TASK"<<std::endl;
			
			//vincolo sincronizzazione job eterogenei
			for (int i = 0; i < _jobs.size(); i++) 
			{
				unsigned int pos=INT_MAX;
				if((pos=_jobs[i].getJobId().find("("))<_jobs[i].getJobId().size())
				{
					
					std::string first=_jobs[i].getJobId().substr(0,pos);
					IloIntervalVarArray synchronizeEterogenei(env);
					for(int j=i+1; j<_jobs.size(); j++)
					{
						std::string second=_jobs[j].getJobId().substr(0,_jobs[j].getJobId().find("("));
						//std::cout<<"Comparing eterogenei "<<first<<" =?= "<<second;
						if(first.compare(second)==0)
						{
							std::cout<<"Comparing eterogenei "<<_jobs[i].getJobId()<<" =?= "<<_jobs[j].getJobId()<<" YES!!!!!!!!!!";
							synchronizeEterogenei.add(task[j]);
						}
						//std::cout<<std::endl;
					}
					IloSynchronize sync(env,task[i],synchronizeEterogenei);
					model.add(sync);
				}
			}

			//creazione matrice utilizzazione e settaggio start time passato e min start time futuro
			std::vector< std::vector<IloIntervalVarArray> > UtilNodes(_jobs.size());
			for (int i = 0; i < _jobs.size(); i++) 
			{
				UtilNodes[i]=std::vector<IloIntervalVarArray>(_nodes.size());
				std::vector<int> usedNodes=_jobs[i].getUsedNodes();
				for(int j=0;j<_nodes.size();j++)
				{

					UtilNodes[i][j]=IloIntervalVarArray(env);
					//std::cout<<"numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes) = "<<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)<<std::endl;
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						UtilNodes[i][j].add(IloIntervalVar(env, _jobs[i].getRealDuration()));
						/*if(!_nodes[j].isUsable() || !_nodes[j].isWorking())
						{
							UtilNodes[i][j][k].setAbsent();
						}*/
						if(_jobs[i].isScheduled())// && _jobs[i].getStartTime()<refTime) 
						{
							//std::cout<<"E' già stato schedulato"<<std::endl;
							if(_jobs[i].getStartTime()+_jobs[i].getRealDuration()<_reservations[iteratoreReservation].getStartTime())
							{
								UtilNodes[i][j][k].setAbsent();	
								std::cout<<"UtilNodes[i][j][k].setAbsent();1"<<std::endl;
							}
							if(usedNodes[j]>=1)
							{
								UtilNodes[i][j][k].setStartMin(_jobs[i].getStartTime());
								UtilNodes[i][j][k].setStartMax(_jobs[i].getStartTime());
								usedNodes[j]--;
								
								std::cout<<"UtilNodes[i][j][k].setStartMin(_jobs[i].getStartTime());UtilNodes[i][j][k].setStartMax(_jobs[i].getStartTime());"<<std::endl;
							}
							else
							{
								UtilNodes[i][j][k].setAbsent();
								std::cout<<"UtilNodes[i][j][k].setAbsent();2"<<std::endl;
							}
						}
						else
						{
							std::cout<<"NON è già stato schedulato"<<std::endl;
							UtilNodes[i][j][k].setOptional();
							//if(!_jobs[i].isReservation())
							//	UtilNodes[i][j][k].setStartMin(refTime);
						}
					}
					
				}		
			}

			//alternative utilizzazione nodi
			for(int i=0;i<_jobs.size();i++)
			{
				IloIntervalVarArray c(env);
				for(int j=0;j<_nodes.size();j++)
				{
					
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						c.add(UtilNodes[i][j][k]);
					}
					
				}
				IloAlternative a1(env,task[i],c,_jobs[i].getNumberOfNodes());
				//std::cout<<i<<" "<<a1<<std::endl;
				model.add(a1);
			}

		
			//cumulfunction cores
			IloCumulFunctionExprArray usgCores(env); 
			for (int j = 0; j < _nodes.size(); j++) 
			{
				//std::cout<<_nodes[j].getNodeName()<<" limit "<<_nodes[j].getCoresNumber()<<" ";int totCores=0;
				int totCores=0;
				for(int k=0;k<_reservations.size();k++)
				{
					totCores+=_reservations[k].getUsedNodes()[j]*_reservations[k].getNumberOfCores()/_reservations[k].getNumberOfNodes();
				}
				usgCores.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						usgCores[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfCores()/_jobs[i].getNumberOfNodes());
					}
					//std::cout<<"Limite core "<<j<<" "<<_jobs[i].getNumberOfCores()/_jobs[i].getNumberOfNodes()<<" <= "<<totCores<<std::endl;
				}
				model.add(usgCores[j] <= totCores);
				
			}

			//cumulfunction gpu
			IloCumulFunctionExprArray usgGPU(env); 
			for (int j = 0; j < _nodes.size(); j++) 
			{
				int totGPU=0;
				for(int k=0;k<_reservations.size();k++)
				{
					totGPU+=_reservations[k].getUsedNodes()[j]*_reservations[k].getNumberOfGPU()/_reservations[k].getNumberOfNodes();
				}
				usgGPU.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						usgGPU[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfGPU()/_jobs[i].getNumberOfNodes());
					}	
					//std::cout<<"Limite GPU "<<j<<" "<<_jobs[i].getNumberOfGPU()/_jobs[i].getNumberOfNodes()<<" <= "<<totGPU<<std::endl;
				}
				model.add(usgGPU[j] <= totGPU);
			}

			//cumulfunction mic
			IloCumulFunctionExprArray usgMIC(env); 
			for (int j = 0; j < _nodes.size(); ++j) 
			{
				int totMIC=0;
				for(int k=0;k<_reservations.size();k++)
				{
					totMIC+=_reservations[k].getUsedNodes()[j]*_reservations[k].getNumberOfMIC()/_reservations[k].getNumberOfNodes();
				}
				usgMIC.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						usgMIC[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfMIC()/_jobs[i].getNumberOfNodes());
					}
					//std::cout<<"Limite MIC "<<j<<" "<<  _jobs[i].getNumberOfMIC()/_jobs[i].getNumberOfNodes()<<" <= "<<totMIC<<std::endl;
				}
				model.add(usgMIC[j] <= totMIC);
				
			}
		
			//cumulfunction mem
			IloCumulFunctionExprArray usgMem(env); 
			for (int j = 0; j < _nodes.size(); ++j) 
			{
				int totMem=0;
				for(int k=0;k<_reservations.size();k++)
				{
					totMem+=_reservations[k].getUsedNodes()[j]*_reservations[k].getMemory()/_reservations[k].getNumberOfNodes();
				}
				
				usgMem.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						usgMem[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getMemory()/_jobs[i].getNumberOfNodes());
					}
					//std::cout<<"Limite MEM "<<j<<" "<<_jobs[i].getMemory()/_jobs[i].getNumberOfNodes()<<" <= "<<totMem<<std::endl;
				}
				model.add(usgMem[j] <= totMem);
				
			}

			//inserimento funzioni obiettivo

			IloIntExprArray delayNL(env);
			IloNumExprArray delayT(env);
			IloIntExprArray ends(env);
			int minEqt=INT_MAX;
			switch(objectiveFunction)
			{
				case NO_OBJ:
				break;
				case MAX_JOBS:
				std::cout<<"Funzione obiettivo MAX_JOBS"<<std::endl;
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					//delayNL.add(IloPresenceOf(env,task[i])*(90000-IloStartOf(task[i])+_reservations[iteratoreReservation].getStartTime()));
					delayNL.add(IloPresenceOf(env,task[i]));
				}
				model.add(IloMaximize(env, IloSum(delayNL)));
				break;
				case NUM_JOB_IN_LATE_OBJF: // define the problem objective: num in late
				
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					delayNL.add(IloStartOf(task[i])-_jobs[i].getEnterQueueTime()>_queue.getMaxMinutesToWait(_jobs[i].getQueue()));
				}
				model.add(IloMinimize(env, IloSum(delayNL)));
				break;
				
				case WEIGHTED_TARDINESS_OBJF: // define the problem objective: weight tard
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					IloInt wait=_queue.getMaxMinutesToWait(_jobs[i].getQueue());
					IloInt priority=_queue.getPriority(_jobs[i].getQueue());
					IloIntExpr ex=IloMax((IloStartOf(task[i])-_jobs[i].getEnterQueueTime()-wait)*priority,0);
					delayT.add(ex);
				}
				model.add(IloMinimize(env, IloSum(delayT)));
				break;
				

				case WEIGHTED_QUEUE_TIME_OBJF: // define the problem objective: weight tard
				/*for(int i=0;i<_queue.size();i++)
				{	
					attese.push_back(_queue[i].getMaxMinutesToWait());
					if(attese[i]<minWait)
					{	
						minWait=attese[i];
					}
				}*/
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					IloInt priority=_queue.getPriority(_jobs[i].getQueue());
					IloIntExpr start=IloStartOf(task[i]);
					IloInt qt=_jobs[i].getEnterQueueTime();
					IloIntExpr num=start-qt;
					IloIntExpr ex=num*priority;
					delayT.add(ex);
					//std::cout<<"QT (S-"<<qt<<")*"<<priority<<" ";
				}
				//std::cout<<std::endl;
				model.add(IloMinimize(env, IloSum(delayT)));
				break;
				case MAKESPAN_OBJF: // define the problem objective: makespan
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					if(_jobs[i].getEnterQueueTime()<minEqt)
						minEqt=_jobs[i].getEnterQueueTime();
				}
				for (int i = 0; i < _jobs.size(); ++i) {
					ends.add(IloEndOf(task[i])-minEqt);
				}
				model.add(IloMinimize(env, IloMax(ends)));
				break;
				
				case TEMPERATURE_RANKING_OBJF: // define the problem objective: temperature ranking
				break;
				
				default:
				break;
			}
			// ================================
			// = Bulid and configure a solver =
			// ================================
		
			// build a solver
			IloCP cp(model);
			/*if(cp.refineConflict())
			{
				cp.writeConflict(cp.out());
				cp.printInformation(std::cout);
			}
			std::cout<<"first print"<<std::endl;
			for(int i=0;i<_jobs.size();i++)
			{
				std::cout<<i<<std::endl;
				std::cout<<cp.domain(task[i])<<std::endl;
			}
			
			for(int i=0;i<_jobs.size();i++)
			{
				std::vector<int> nodes(_nodes.size(),0);
				for(int j=0;j<_nodes.size();j++)
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)&& k<1;k++)
					{
						std::cout<<i<<" "<<j<<" "<<k<<std::endl;
						std::cout<<cp.domain(UtilNodes[i][j][k])<<std::endl;
					}
				}
			}
			std::cout<<"end print"<<std::endl;*/
	
				//getchar();
			
			//std::cout << "MODEL: " << model << std::endl;

			// USE A SINGLE PROCESSOR (DO NOT TOUCH THIS)
			cp.setParameter(IloCP::Workers, 4);
	
			// DEFINING A TIME LIMIT
			// other limit types are described the solver documentation
			timeLimit = _secondsOfExecution;
			cp.setParameter(IloCP::TimeLimit, timeLimit);
			
			// LOG EVERY N BRANCHES:
			// cp.setParameter(IloCP::LogPeriod, 100000);
	
			// DO NOT LOG AT ALL
			cp.setParameter(IloCP::LogVerbosity, IloCP::Quiet);

			// SET SEARCH TYPE TO DSF
			// keep commented to use te default search
			// other search modes are available: check the documentation
			cp.setParameter(IloCP::SearchType, IloCP::Auto);//IloCP::DepthFirst IloCP:Restart IloCP::MultiPoint IloCP::Auto 

			// SET FILTERING STRENGTH FOR "ALL" CONSTRAINTS
			// other strength values are available from the documentation
			cp.setParameter(IloCP::DefaultInferenceLevel, IloCP::Basic); //IloCP::Low, IloCP::Basic, IloCP::Medium, and IloCP::Extended
	
			// SET FILTERING STRENGTH FOR THE CUMULATIVE CONSTRAINTS ONLY
			// other strength values are available from the documentation
			cp.setParameter(IloCP::CumulFunctionInferenceLevel, IloCP::Extended);
	
			// =====================
			// = Solve the problem =
			// =====================
	
			// start to search
			cp.startNewSearch();
			/*for (int i = 0; i < _jobs.size(); ++i) 
				{
					//std::cout << _jobs[i].tostd::string(0,0)<<std::endl;
				std::cout << cp.domain(task[i])<<std::endl;
				}*/
			/*std::cout<<"Sono qui "<<_jobs.size() <<std::endl;
			cp.printInformation(std::cout);
			for(int i=0;i<_jobs.size();i++)
			{
				std::cout<<"Entro loop1"<<std::endl;
				std::vector<int> nodes(_nodes.size(),0);
				for(int j=0;j<_nodes.size();j++)
				{
				std::cout<<"    Entro loop2"<<std::endl;
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
				std::cout<<"        Entro loop3 "<<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)<<std::endl;
						std::cout<<cp.domain(UtilNodes[i][j][k])<<std::endl;
					}
				}
			}
			cp.printInformation(std::cout);*/
			// loops until there are solution
			while (cp.next()) 
			{
				
				// "remember" that at least a solution was found
				
				solved = true;
				retry=false;
				_secondsOfExecution=_minTime;
				std::cout << "***** SOLUTION FOUND ***** time: " << cp.getInfo(IloCP::SolveTime) <<" valore soluzione: " << cp.getObjValue() << std::endl;
				if(firstSolution!=-1)
				{
					firstSolution=	cp.getInfo(IloCP::SolveTime);
				}
				/* DE-COMMENT THIS TO SEE HOW THE SOLUTION LOOKS LIKE
				std::cout <<"QUI"<<std::endl;
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					//std::cout << _jobs[i].tostd::string(0,0)<<std::endl;
				std::cout << cp.domain(task[i])<<std::endl;
				}
				std::cout << "FINE"<<std::endl;
				std::cout <<"QUI Util"<<std::endl;
				for(int i=0;i<_jobs.size();i++)
			{
				//std::vector<int> nodes(_nodes.size(),0);
				for(int j=0;j<_nodes.size();j++)
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						std::cout<<cp.domain(UtilNodes[i][j][0])<<std::endl;
					}
				}
			}
				std::cout << "FINE"<<std::endl;*/
				//getchar();
				
		//std::cout<<"1"<<std::endl;
			}
		//std::cout << "1 e 1/2" << std::endl;
			if(solved)
			{
		//std::cout<<"2"<<std::endl;
				for(int i=0;i<_jobs.size();i++)
				{
		//std::cout<<"3"<<std::endl;
					if(cp.isPresent(task[i]))
					{
						std::vector<int> nodes(_nodes.size(),0);
						for(int j=0;j<_nodes.size();j++)
						{
		//std::cout<<"4"<<std::endl;
							for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
							{
		//std::cout<<"5"<<std::endl;
								if(cp.isPresent(UtilNodes[i][j][k]))
								{
		//std::cout<<"6"<<std::endl;
									nodes[j]++;
		//std::cout<<"7"<<std::endl;
								}
					
							}
						}
						_jobs[i].allocate(cp.getStart(task[i]),nodes);
						_jobs[i].setScheduled(true);
						//std::cout<<"Job Presente:"<<_jobs[i].tostd::string(0,0)<<std::endl;
					}
					else
					{
						//std::cout<<"Job NoN Presente:"<<_jobs[i].tostd::string(0,0)<<std::endl;
					}
				}
			}

			// terminate search
			cp.endSearch();
			//se la ricerca non ha prodotto risultati a causa di un timeLimit troppo std::stringente: aumento il time limit e ritento, 
			if (!solved && cp.getInfo(IloCP::SolveTime) >= timeLimit) 
			{
				retry=true;
				_secondsOfExecution*=5;
				//std::cout << "changed time limit: min " << _secondsOfExecution << std::endl;
				if(_secondsOfExecution>=1250)
				{
					return _jobs;
				}
				//getchar();
			}
			else if(!solved) //se la ricerca termina perché non sono presenti soluzioni ammissibili
			{
				std::cout << "THE PROBLEM IS INFEASIBLE, time "<<cp.getInfo(IloCP::SolveTime)<<">"<<timeLimit << std::endl;
				//getchar();
				return _jobs;
			}
	
			// DE-COMMENT THIS IF YOU ARE NOT USING LOGGING
			//cp.printInformation();
	
			env.end();
			iteratoreReservation++;
			precOptionals=numberOfOptionals;
			numberOfOptionals=0;
			for(int i=0;i<_jobs.size();i++)
				if(!_jobs[i].isScheduled())
					numberOfOptionals++;
//std::cout<<"opzionali "<<numberOfOptionals<<" | precedenti "<<precOptionals<<std::endl;
 //getchar();
		}while((numberOfOptionals>0) && numberOfOptionals!= precOptionals);
		//std::cout<<"Uscita"<<std::endl;
//getchar();
	}
	catch (IloException& ex) 
	{
		std::string m(ex.getMessage());
		if(m.find("Empty array in expression")==std::string::npos) //se l'eccezione è Empty array in expression allora è una eventualità normalissima (sono finiti i job in coda)
		{
			
			std::cout << "Error: " << m <<". Exception type " <<typeid(ex).name()<< std::endl;
		
			//getchar();
			exit(0);
		}
	}
	//std::cout<<"exit solve model"<<std::endl;
	//*secondiEsecuzione=firstSolution;
	return _jobs;
}
#endif
