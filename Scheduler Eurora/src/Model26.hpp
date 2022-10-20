/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                   File: Model26.hpp                                                  *
*                                                                                                                      *
*                                                                                                                      *
 ***********************************************************************************************************************/

#ifndef Def_Model26
#define Def_Model26
#include "QueueArray.hpp"
#include "NodeArray.hpp"
#include "JobArray.hpp"
#include "IModel.hpp"
#include "IModelAdvanced.hpp"
#include "IModelComplete.hpp"
#include "Instant.hpp"
#include <ilcp/cp.h>
#include <vector>
#include <stdio.h>
#include <typeinfo>

class Model26: public IModelComplete
{
private:
	double _secondsOfExecution;
	double _minTime;
	Instant _intervals;
	
	int numberOfJointNodes(Job j,Node n,Queue * coda,NodeArray _nodes)
	{
		//cout<<"QUI1"<<endl;
		if(!n.isUsable() || (!n.isWorking() && !j.isScheduled()))
		{
			cout<<"Job "<<j.getJobId()<<" cannot run on node "<<n.getNodeName()<<" node not working "<<endl;
			return 0;
		}
		//cout<<"QUI2"<<endl;
		if(j.getGHz()>0 && n.getGHz()>0 && j.getGHz()!=n.getGHz())
		{
			cout<<"Job "<<j.getJobId()<<" ghz "<<j.getGHz()<<" cannot run on node "<<n.getNodeName()<<" ghz "<<n.getGHz()<<endl;
			//getchar();
			return 0;
		}
		//cout<<"QUI3"<<endl;
		if(j.getHost().compare("")!=0 && j.getHost().compare(n.getNodeName())!=0)
		{
			cout<<"Job "<<j.getJobId()<<" cannot run on node "<<n.getNodeName()<<" Different Host selection "<<endl;
			return 0;
		}
		//cout<<"QUI4"<<endl;
		int r=j.getNumberOfNodes();
		//cout<<"r1"<<j.getNumberOfNodes()<<endl;
		if(j.getNumberOfCores()!=0)
			r=Util::min(r,n.getCoresNumber()/(j.getNumberOfCores()/j.getNumberOfNodes()));
		//cout<<"r2"<<r<<endl;
		if(j.getNumberOfGPU()!=0)
			r=Util::min(r,n.getGPUsNumber()/(j.getNumberOfGPU()/j.getNumberOfNodes()));
		//cout<<"r3"<<r<<endl;
		if(j.getNumberOfMIC()!=0)
			r=Util::min(r,n.getMICsNumber()/(j.getNumberOfMIC()/j.getNumberOfNodes()));
		//cout<<"r4"<<r<<endl;
		if(j.getMemory()!=0)
			r=Util::min(r,n.getTotalMemory()/(j.getMemory()/j.getNumberOfNodes()));
		//cout<<"r5"<<r<<endl;

		//cout<<"QUI5"<<endl;
		if(!n.canAllocateToQueue(*coda))
		{
			r=0;
			cout<<"Non posso allocare il job "<<j.getJobId()<<" con coda "<<j.getQueue()<<" sul nodo "<<n.getNodeName()<<endl;
		}
		else
		{
			cout<<"Posso allocare il job "<<j.getJobId()<<" con coda "<<j.getQueue()<<" chunk ";
			for(unsigned int i=0;i<coda->getDefaultChunk().size();i++)
			{
				cout<<coda->getDefaultChunk()[i]<<", ";
			}
			cout<<" sul nodo "<<n.getNodeName()<<endl;
		}
		//cout<<"QUI6"<<endl;
		//getchar();
		return r;
	}
	
	int makespanUpperBound(JobArray _jobs)
	{
		int totale=_initialTime+4*7*24*60*60;
		for(int i=0;i<_jobs.size();i++)
		{
			totale+=_jobs[i].getRealDuration();
		}
		return totale;
	}
	
	
	void setPrimeTimeIntervals(JobArray _jobs,IloEnv env,IloIntervalVarArray primeTimeIntervals)
	{
		int totale=makespanUpperBound(_jobs);
		vector< vector<int> > intervalli=_intervals.getIntervals(_initialTime,totale,true);
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
		int totale=makespanUpperBound(_jobs);
		vector< vector<int> > intervalli=_intervals.getIntervals(_initialTime,totale,false);
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
	Model26(){_minTime=2;_os=-1;_wt=-1;_wtDelta=-1;_nl=-1;_nlDelta=-1;_mk=-1;_mkDelta=-1;}
	/*Model26(QueueArray queue,NodeArray nodes,JobArray jobs){_queue=queue,_nodes=nodes,_jobs=jobs;_minTime=1;_os=-1;_wt=-1;_wtDelta=-1;_nl=-1;_nlDelta=-1;_mk=-1;_mkDelta=-1;}*/
	JobArray solve(JobArray _jobs, QueueArray _queue, NodeArray _nodes,int refTime,int objectiveFunction,int * secondiEsecuzione,bool * risolto,int * totaleEsecuzione,bool * riprova,bool print);
	JobArray solveReservation(JobArray _jobs,JobArray _reservations, QueueArray _queue, NodeArray _nodes,int refTime,int objectiveFunction,int secondiEsecuzione);
	//double getOptimalSolution(){return _os;}
	
	void setIntervals(Instant intervalli)
	{
		/*primeTimeIntervals=intervalli[0];
		nonPrimeTimeIntervals=intervalli[1];*/
		_intervals=intervalli;
	}
};

inline JobArray Model26::solve(JobArray _jobs, QueueArray _queue, NodeArray _nodes,int refTime,int objectiveFunction,int * secondiEsecuzione,bool * risolto,int * totaleEsecuzione,bool * riprova,bool print)
{
	bool solved = false;
	//bool retry=false;
	_initialTime=refTime;
	//_secondsOfExecution=_minTime;
	_secondsOfExecution=*secondiEsecuzione;
	if(print)
		cout<<"secondi di esecuzione "<<*secondiEsecuzione<<endl;
	IloInt timeLimit = _secondsOfExecution;
	int firstSolution=-1;
	try	
	{
		//do{
			IloEnv env; 
			
			IloModel model(env);
			IloIntervalVarArray task(env);

			//settaggio start time passati, start time reservation, min start time futuri e vincolo su modello makespan
			
			cout<<"MODEL26 IL NUMERO di JOBS E': "<<_jobs.size()<<"------------------------------------------"<<endl;
			for (int i = 0; i < _jobs.size(); i++) 
			{
				task.add(IloIntervalVar(env, _jobs[i].getRealDuration()));
				Queue * coda=_queue.getQueue(_jobs[i]);
				
				if(_jobs[i].isScheduled() && !_jobs[i].isReservation())// && _jobs[i].getStartTime()<refTime)
				{
					cout<<_jobs[i].getJobId()<<"JOB GIA SCHEDULATO---------------------------------------"<<endl;
					task[i].setStartMin(_jobs[i].getStartTime());
					task[i].setStartMax(_jobs[i].getStartTime());
				}
				else
				{
					cout<<_jobs[i].getJobId()<<"Job non ancora schedulato"<<endl;
					
					cout<<"Coda "<<(_queue.getQueue(_jobs[i]))->getId()<<endl;
					if(_jobs[i].isReservation())
					{
						cout<<"Reservation "<<_jobs[i].getJobId()<<Util::timeToStr(_jobs[i].getStartTime())<<endl;
						task[i].setStartMin(_jobs[i].getStartTime());
						task[i].setStartMax(_jobs[i].getStartTime());
						//getchar();
					}
					else if(!coda->isEnabled())
					{
						std::cout<<"Coda non abilitata "<<coda->getId()<<std::endl;
						task[i].setOptional();
						task[i].setAbsent();
					}
					else
					{
						
						if(_jobs[i].isNonPrimeTime())
						{
							std::cout<<"JNP "<<refTime<<std::endl;
							task[i].setStartMin(refTime);
							IloIntervalVarArray noOverlapIntervals(env);
							setPrimeTimeIntervals(_jobs,env,noOverlapIntervals);
							noOverlapIntervals.add(task[i]);
							IloNoOverlap no(env,noOverlapIntervals);
							model.add(no);
							
						}
						else if(_jobs[i].isPrimeTime())
						{
							std::cout<<"JP "<<refTime<<std::endl;
							task[i].setStartMin(refTime);
							IloIntervalVarArray noOverlapIntervals(env);
							setNonPrimeTimeIntervals(_jobs,env,noOverlapIntervals);
							noOverlapIntervals.add(task[i]);
							IloNoOverlap no(env,noOverlapIntervals);
							model.add(no);
						}
						else
						{
							cout<<_jobs[i].getJobId()<<" Setto min ref time = "<<refTime<<endl;
							task[i].setStartMin(refTime);
						}
						//cout<<"Fine settaggio job con coda abilitata"<<endl;
					}
					
					//cout<<"Fine settaggio job non in esecuzione "<<this<<" "<<&env<<&model<<&task<<&task[0]<<endl;
				}
				//cout<<"if(_mk>0 && _mkDelta>0)"<<endl;
				//cout<<_mk<<" "<<_mkDelta<<endl;
				if(_mk>0 && _mkDelta>0 && _jobs[i].isAnytime())
				{
					std::cout<<"Add vincolo makespan start max "<<_mk*_mkDelta-_jobs[i].getRealDuration()<<std::endl;
					task[i].setStartMax(_mk*_mkDelta-_jobs[i].getRealDuration());
				}
				//cout<<"Fine settaggio job"<<endl;
			} 
			//std::cout<<"FINITO TASK"<<std::endl;

			//vincolo sincronizzazione job eterogenei
			for (int i = 0; i < _jobs.size(); i++) 
			{
				unsigned int pos=INT_MAX;
				if((pos=_jobs[i].getJobId().find("("))<_jobs[i].getJobId().size())
				{
					
					string first=_jobs[i].getJobId().substr(0,pos);
					IloIntervalVarArray synchronizeEterogenei(env);
					for(int j=i+1; j<_jobs.size(); j++)
					{
						string second=_jobs[j].getJobId().substr(0,_jobs[j].getJobId().find("("));
						cout<<"Comparing eterogenei "<<first<<" =?= "<<second;
						if(first.compare(second)==0)
						{
							cout<<" YES!!!!!!!!!!";
							synchronizeEterogenei.add(task[j]);
						}
						cout<<endl;
					}
					IloSynchronize sync(env,task[i],synchronizeEterogenei);
					model.add(sync);
				}
			}

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
								cout<<"Vincolo start before start "<<_jobs[i].getJobId()<<" "<<_jobs[j].getJobId()<<endl;
								model.add(IloStartBeforeStart(env,task[i],task[j]));
							}
						}
					}
				}
			}
			//vincolo su modello num in late
			if(_nl>0 && _nlDelta>0)
			{
				//std::cout << "AAAARRRGHHHH!!!" << endl;
				IloNumExprArray nlSum(env);
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					if( _jobs[i].isAnytime())
					{
						nlSum.add(IloStartOf(task[i])-_jobs[i].getEnterQueueTime()>_queue.getMaxMinutesToWait(_jobs[i].getQueue()));
						std::cout<<"Add vincolo num late SUM start-EQT("<<_jobs[i].getEnterQueueTime()<<") > maximumWait("<<_queue.getMaxMinutesToWait(_jobs[i].getQueue())<<") <= nl*delta("<<_nl<<"*"<<_nlDelta<<")"<<std::endl;
					}
				}
				model.add(IloSum(nlSum) <= _nl*_nlDelta);
			}

			// vincolo su modello weighted tardiness
			if(_wt>0 && _wtDelta>0)
			{
				//std::cout << "AAAARRRGHHHH!!!" << endl;
				IloNumExprArray wtSum(env);
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					
					if( _jobs[i].isAnytime())
					{
						IloNum wait=_queue.getMaxMinutesToWait(_jobs[i].getQueue());
						wtSum.add(IloMax((IloStartOf(task[i])-_jobs[i].getEnterQueueTime()-wait)/wait,0));
						std::cout<<"Add vincolo weight t MAX start-EQT("<<_jobs[i].getEnterQueueTime()<<") -wait("<<_queue.getMaxMinutesToWait(_jobs[i].getQueue())<<") / wait("<<_queue.getMaxMinutesToWait(_jobs[i].getQueue())<<") <= wt*delta("<<_wt*_wtDelta<<")"<<std::endl;
					}
				}
				model.add(IloSum(wtSum) <= _wt*_wtDelta);
			}

			//creazione matrice utilizzazione e settaggio start time passato e min start time futuro
			std::vector< std::vector<IloIntervalVarArray> > UtilNodes(_jobs.size());
			cout<<"UtilNodes creation per "<<_jobs.size()<<" jobs"<<endl;
			for (int i = 0; i < _jobs.size(); i++) 
			{
				UtilNodes[i]=std::vector<IloIntervalVarArray>(_nodes.size());
				std::vector<int> usedNodes=_jobs[i].getUsedNodes();
				
				cout<<"numero nodi "<<_nodes.size()<<endl;
				for(int j=0;j<_nodes.size();j++)
				{
					//if(_nodes[j].isUsable())
					//{
					UtilNodes[i][j]=IloIntervalVarArray(env);
					cout<<"Inizio loop"<<endl;
					cout<<"numero virtual job usabili sul nodo "<<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)<<endl;
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						UtilNodes[i][j].add(IloIntervalVar(env, _jobs[i].getRealDuration()));
						/*if(!_nodes[j].isUsable() || !_nodes[j].isWorking())
						{
							cout<<"NODO ASSENTE!!!!!!!!!!!! "<<_jobs[i].getJobId()<<" "<<_nodes[j].getNodeName()<<endl;
							UtilNodes[i][j][k].setAbsent();
						}
						else*/ if(_jobs[i].isScheduled())// && !_jobs[i].isReservation())// && _jobs[i].getStartTime()<refTime) 
						{
							cout<<"E' già stato schedulato "<<_jobs[i].getJobId()<<" "<<_jobs[i].isScheduled()<<endl;

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
							cout<<"NON è già stato schedulato "<<_jobs[i].getJobId()<<" "<<_jobs[i].isScheduled()<<endl;
							UtilNodes[i][j][k].setOptional();
							if(!_jobs[i].isReservation())
								UtilNodes[i][j][k].setStartMin(refTime);
						}
					}
					cout<<"Fine loop1"<<endl;
					
				}
				cout<<"Fine loop2"<<endl;	
			}
			cout<<"Inizio altervnative"<<endl;
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
				model.add(a1);
			}
			//cout<<"Fine altervnative"<<endl;

		
			//cout<<"Inizio Cumul core"<<endl;
			//cumulfunction cores
			IloCumulFunctionExprArray usgCores(env); 
			for (int j = 0; j < _nodes.size(); j++) 
			{
				//cout<<_nodes[j].getNodeName()<<" limit "<<_nodes[j].getCoresNumber()<<" ";
				usgCores.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						usgCores[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfCores()/_jobs[i].getNumberOfNodes());
						//cout<<"util "<<_jobs[i].getNumberOfCores()/_jobs[i].getNumberOfNodes()<<" "; 
					}
					/*if(_jobs[i].isScheduled() && _jobs[i].getUsedNodes()[j]>0)
					{
						cout<<_jobs[i].getJobId()<<" "<<_jobs[i].getUsedNodes()[j]*_jobs[i].getNumberOfCores()/_jobs[i].getNumberOfNodes();
					}*/
					cout<<"Limite core "<<j<<" "<<_jobs[i].getNumberOfCores()<<"/"<<_jobs[i].getNumberOfNodes()<<" <= "<<_nodes[j].getCoresNumber()<<endl;
				}
				model.add(usgCores[j] <= _nodes[j].getCoresNumber());
				//cout<<endl;
 
				
			}
			//cout<<"Fine Cumul core"<<endl;

			
			//cout<<"Inizio Cumul gpu"<<endl;
			//cumulfunction gpu
			IloCumulFunctionExprArray usgGPU(env); 
			for (int j = 0; j < _nodes.size(); j++) 
			{
				usgGPU.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						usgGPU[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfGPU()/_jobs[i].getNumberOfNodes());
					}
					cout<<"Limite gpu "<<j<<" "<<_jobs[i].getNumberOfGPU()<<"/"<<_jobs[i].getNumberOfNodes()<<" <= "<<_nodes[j].getGPUsNumber()<<endl;
				}
				model.add(usgGPU[j] <= _nodes[j].getGPUsNumber());
				
			}
			//cout<<"Fine Cumul gpu"<<endl;

			//cout<<"Inizio Cumul mic"<<endl;
			//cumulfunction mic
			IloCumulFunctionExprArray usgMIC(env); 
			for (int j = 0; j < _nodes.size(); ++j) 
			{
				usgMIC.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						usgMIC[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfMIC()/_jobs[i].getNumberOfNodes());
					}
					cout<<"Limite mic "<<j<<" "<<_jobs[i].getNumberOfMIC()/_jobs[i].getNumberOfNodes()<<" <= "<<_nodes[j].getMICsNumber()<<endl;
				}
				model.add(usgMIC[j] <= _nodes[j].getMICsNumber());
				
			}
			//cout<<"Fine Cumul mic"<<endl;
			//cout<<"Inizio Cumul mem"<<endl;
		
			//cumulfunction mem
			IloCumulFunctionExprArray usgMem(env); 
			for (int j = 0; j < _nodes.size(); ++j) 
			{
				
				usgMem.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						usgMem[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getMemory()/_jobs[i].getNumberOfNodes());
					
					}
					cout<<"Limite mem "<<j<<" "<<_jobs[i].getMemory()/_jobs[i].getNumberOfNodes()<<" <= "<<_nodes[j].getTotalMemory()<<endl;
				}
				model.add(usgMem[j] <= _nodes[j].getTotalMemory());
				
			}
			//cout<<"Fine Cumul mem"<<endl;

			//inserimento funzioni obiettivo

			//cout<<"Inizio inserimento funzioni obiettivo"<<endl;
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
					cout<<"QT (S-"<<qt<<")*"<<priority<<" ";
				}
				//cout<<endl;
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
			//cout<<"Inizio inserimento funzioni obiettivo"<<endl;
			// ================================
			// = Bulid and configure a solver =
			// ================================
		
			// build a solver
			IloCP cp(model);
			//cout<<"IloCP isntaziato"<<endl;
			if(cp.refineConflict())
			{
			//cout<<"refining conflict"<<endl;
				cp.writeConflict(cp.out());
				cp.printInformation(std::cout);
			}
			//cout<<"end refining conflict"<<endl;
			/*
			for(int i=0;i<_jobs.size();i++)
			{
				cout<<_jobs[i].getJobId()<<" "<<cp.domain(task[i])<<endl;
			}
			for(int i=0;i<_jobs.size();i++)
			{
				std::vector<int> nodes(_nodes.size(),0);
				for(int j=0;j<_nodes.size();j++)
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)&& k<1;k++)
					{
						cout<<cp.domain(UtilNodes[i][j][0])<<endl;
					}
				}
			}
				cout<<model<<endl;
	
				//getchar();
			}*/
			//cout << "MODEL: " << model << endl;

			// USE A SINGLE PROCESSOR (DO NOT TOUCH THIS)
			cp.setParameter(IloCP::Workers, 4);
	
			// DEFINING A TIME LIMIT
			// other limit types are described the solver documentation
			timeLimit = _secondsOfExecution;
			cout<<"eseguo con time limit a "<<timeLimit<<endl;
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
	
			// start to search
			cp.startNewSearch();
			
			/*cout<<"Sono qui "<<_jobs.size() <<endl;
			cp.printInformation(std::cout);
			for(int i=0;i<_jobs.size();i++)
			{
				cout<<"Entro loop1"<<endl;
				std::vector<int> nodes(_nodes.size(),0);
				for(int j=0;j<_nodes.size();j++)
				{
				cout<<"    Entro loop2"<<endl;
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
				cout<<"        Entro loop3 "<<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)<<endl;
						cout<<cp.domain(UtilNodes[i][j][k])<<endl;
					}
				}
			}
			cp.printInformation(std::cout);*/
			// loops until there are solution
			while (cp.next()) 
			{
				
				// "remember" that at least a solution was found
				*riprova=false;
				solved = true;
				//retry=false;
				_secondsOfExecution=_minTime;
				
				if(print)
					std::cout << "***** SOLUTION FOUND ***** time: " << cp.getInfo(IloCP::SolveTime) <<" valore soluzione: " << cp.getObjValue() << std::endl;
				if(firstSolution!=-1)
				{
					firstSolution=	cp.getInfo(IloCP::SolveTime);
				}
				/*DE-COMMENT THIS TO SEE HOW THE SOLUTION LOOKS LIKE
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					std::cout << _jobs[i].toString(0,0)<<std::endl;
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
				
		//cout<<"1"<<endl;
			}
		//cout << "1 e 1/2" << endl;
			if(solved)
			{
		//cout<<"2"<<endl;
				//cout<<"SOLVED"<<endl;
				//getchar();
				*totaleEsecuzione=cp.getInfo(IloCP::SolveTime);
				for(int i=0;i<_jobs.size();i++)
				{
		//cout<<"3"<<endl;
					std::vector<int> nodes(_nodes.size(),0);
					for(int j=0;j<_nodes.size();j++)
					{
		//cout<<"4"<<endl;
						for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
						{
		//cout<<"5"<<endl;
							if(cp.isPresent(UtilNodes[i][j][k]))
							{
		//cout<<"6"<<endl;
								nodes[j]++;
		//cout<<"7"<<endl;
							}
					
						}
					}
					_jobs[i].allocate(cp.getStart(task[i]),nodes);
				}
			}

			// terminate search
			cp.endSearch();
			
			//se la ricerca non ha prodotto risultati a causa di un timeLimit troppo stringente: aumento il time limit e ritento, 
			if (!solved && cp.getInfo(IloCP::SolveTime) >= timeLimit) 
			{
				*riprova=true;
				*totaleEsecuzione=timeLimit;
				//_secondsOfExecution*=5;
				//std::cout << "changed time limit: min " << _secondsOfExecution << std::endl;
				//se il time limit eccede i 125 minuti ritorno comunque la soluzione precedente (in futuro avrò meno jobs e riuscirò a finire uno scheduling)
				/*std::cout <<"Task:"<<endl;
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					//std::cout << _jobs[i].toString(0,0)<<std::endl;
					std::cout << cp.domain(task[i])<<std::endl;
				}
				std::cout <<"Util:"<<endl;
				for(int i=0;i<_jobs.size();i++)
				{
					std::vector<int> nodes(_nodes.size(),0);
					for(int j=0;j<_nodes.size();j++)
					{
						for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)&& k<1;k++)	
						{
							cout<<cp.domain(UtilNodes[i][j][0])<<endl;
						}
					}
				}*/
				//getchar();
				if(_secondsOfExecution>=1250)
				{

					*riprova=false;
					//*totaleEsecuzione=timeLimit;
				/*std::cout <<"Task:"<<endl;
					for (int i = 0; i < _jobs.size(); ++i) 
					{
						//std::cout << _jobs[i].toString(0,0)<<std::endl;
						std::cout << cp.domain(task[i])<<std::endl;
					}
				std::cout <<"Util:"<<endl;
					for(int i=0;i<_jobs.size();i++)
					{
						std::vector<int> nodes(_nodes.size(),0);
						for(int j=0;j<_nodes.size();j++)
						{
							for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)&& k<1;k++)
							{
								cout<<cp.domain(UtilNodes[i][j][0])<<endl;
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
				std::cout <<"Task:"<<endl;
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					std::cout << _jobs[i].toString(0,0)<<std::endl;
					std::cout << cp.domain(task[i])<<std::endl;
				}
				/*std::cout <<"Util:"<<endl;
				for(int i=0;i<_jobs.size();i++)
				{
					std::vector<int> nodes(_nodes.size(),0);
					for(int j=0;j<_nodes.size();j++)
					{
						for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)&& k<1;k++)
						{
							cout<<cp.domain(UtilNodes[i][j][0])<<endl;
						}
					}
				}*/
				//getchar();
				//break;
			}
	
			// DE-COMMENT THIS IF YOU ARE NOT USING LOGGING
			//cp.printInformation();
			
			
			*risolto=solved;
			env.end();
			
		//}while(retry);
		
	}
	catch (IloException& ex) 
	{
		std::string m(ex.getMessage());
		if(m.find("Empty array in expression")==string::npos) //se l'eccezione è Empty array in expression allora è una eventualità normalissima (sono finiti i job in coda)
		{
			
			std::cout << "Error: " << m <<". Exception type " <<typeid(ex).name()<< std::endl;
		
			//getchar();
			exit(0);
		}
	}
	//cout<<"exit solve model"<<endl;
	*secondiEsecuzione=firstSolution;
	return _jobs;
}

inline JobArray Model26::solveReservation(JobArray _jobs,JobArray _reservations, QueueArray _queue, NodeArray _nodes,int refTime,int objectiveFunction,int secondiEsecuzione)
{
	bool solved = false;
	bool retry=false;

	_initialTime=refTime;
	_secondsOfExecution=secondiEsecuzione;
	cout<<"solveReservation secondi di esecuzione "<<secondiEsecuzione<<endl;
	IloInt timeLimit = _secondsOfExecution;
	int firstSolution=-1;
	int iteratoreReservation=0;
	while(refTime>_reservations[iteratoreReservation].getStartTime()+_reservations[iteratoreReservation].getRealDuration())
	{
		//cout<<refTime<<" MAGGIORE DI "<<_reservations[iteratoreReservation].getStartTime()+_reservations[iteratoreReservation].getRealDuration()<<endl;
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
			IloEnv env; 
			
			IloModel model(env);
			IloIntervalVarArray task(env);

			//settaggio start time passati, start time reservation, min start time futuri e vincolo su modello makespan
			
			//cout<<"IL NUMERO di JOBS E': "<<_jobs.size()<<" iterazione"<<iteratoreReservation<<"------------------------------------------"<<endl;
			for (int i = 0; i < _jobs.size(); i++) 
			{
				task.add(IloIntervalVar(env, _jobs[i].getRealDuration()));
				Queue * coda=_queue.getQueue(_jobs[i]);
				
				if(_jobs[i].isScheduled())// && _jobs[i].getStartTime()<refTime)
				{
					//cout<<"JOB GIA SCHEDULATO---------------------------------------"<<endl;
					task[i].setStartMin(_jobs[i].getStartTime());
					task[i].setStartMax(_jobs[i].getStartTime());
					if(_jobs[i].getStartTime()+_jobs[i].getRealDuration()<_reservations[iteratoreReservation].getStartTime())
					{
						task[i].setAbsent();
						//cout<<"----->Job "<<i<<" setto absent perchè in reservation precedente"<<endl;
					}
				}
				else
				{
					//cout<<"Job non ancora schedulato"<<endl;
					task[i].setOptional();
					if(!coda->isEnabled())
					{
						//std::cout<<"Coda non abilitata "<<coda->getId()<<std::endl;
						task[i].setAbsent();
					}
					else
					{
						
						//task[i].setStartMin(refTime);
						int minTime=refTime>_reservations[iteratoreReservation].getStartTime()?refTime:_reservations[iteratoreReservation].getStartTime();
						//cout<<"Setto start min = "<<minTime<<" end max = "<<_reservations[iteratoreReservation].getStartTime()+_reservations[iteratoreReservation].getRealDuration()<<" ID "<<_jobs[i].getJobId()<<endl;
						task[i].setStartMin(minTime);
						task[i].setEndMax(_reservations[iteratoreReservation].getStartTime()+_reservations[iteratoreReservation].getRealDuration());
						//cout<<"Fine settaggio job con coda abilitata"<<endl;
					}
					
					//cout<<"Fine settaggio job non in esecuzione "<<this<<" "<<&env<<&model<<&task<<&task[0]<<endl;
				}
				//cout<<"Fine settaggio job"<<endl;
			} 
			//std::cout<<"FINITO TASK"<<std::endl;

			//creazione matrice utilizzazione e settaggio start time passato e min start time futuro
			std::vector< std::vector<IloIntervalVarArray> > UtilNodes(_jobs.size());
			for (int i = 0; i < _jobs.size(); i++) 
			{
				UtilNodes[i]=std::vector<IloIntervalVarArray>(_nodes.size());
				std::vector<int> usedNodes=_jobs[i].getUsedNodes();
				for(int j=0;j<_nodes.size();j++)
				{
					//if(_nodes[j].isUsable())
					//{
					UtilNodes[i][j]=IloIntervalVarArray(env);
					
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						UtilNodes[i][j].add(IloIntervalVar(env, _jobs[i].getRealDuration()));
						if(!_nodes[j].isUsable() || !_nodes[j].isWorking())
						{
							UtilNodes[i][j][k].setAbsent();
						}
						else if(_jobs[i].isScheduled())// && _jobs[i].getStartTime()<refTime) 
						{
							//cout<<"E' già stato schedulato"<<endl;
							if(_jobs[i].getStartTime()+_jobs[i].getRealDuration()<_reservations[iteratoreReservation].getStartTime())
							{
								UtilNodes[i][j][k].setAbsent();	
							}
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
							//cout<<"NON è già stato schedulato"<<endl;
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
				//cout<<i<<" "<<a1<<endl;
				model.add(a1);
			}

		
			//cumulfunction cores
			IloCumulFunctionExprArray usgCores(env); 
			for (int j = 0; j < _nodes.size(); j++) 
			{
				//cout<<_nodes[j].getNodeName()<<" limit "<<_nodes[j].getCoresNumber()<<" ";
				usgCores.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						usgCores[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfCores()/_jobs[i].getNumberOfNodes());
						//
					}
					//cout<<"util "<<_jobs[i].getNumberOfCores()/_jobs[i].getNumberOfNodes()<<" "; 
					/*if(_jobs[i].isScheduled() && _jobs[i].getUsedNodes()[j]>0)
					{
						cout<<_jobs[i].getJobId()<<" "<<_jobs[i].getUsedNodes()[j]*_jobs[i].getNumberOfCores()/_jobs[i].getNumberOfNodes();
					}*/
				}
				//cout<<"Limite core "<<j<<" "<<" <= "<<_reservations[iteratoreReservation].getUsedNodes()[j]<<"*"<<_reservations[iteratoreReservation].getNumberOfCores()/_reservations[iteratoreReservation].getNumberOfNodes()<<endl;
				model.add(usgCores[j] <= _reservations[iteratoreReservation].getUsedNodes()[j]*_reservations[iteratoreReservation].getNumberOfCores()/_reservations[iteratoreReservation].getNumberOfNodes());
				//model.add(usgCores[j] <= _nodes[j].getCoresNumber());
				//cout<<endl;
				
			}

			//cumulfunction gpu
			IloCumulFunctionExprArray usgGPU(env); 
			for (int j = 0; j < _nodes.size(); j++) 
			{
				usgGPU.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						usgGPU[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfGPU()/_jobs[i].getNumberOfNodes());
					}	
					//cout<<"util "<<_jobs[i].getNumberOfGPU()/_jobs[i].getNumberOfNodes()<<" "; 
				}
				//model.add(usgGPU[j] <= _nodes[j].getGPUsNumber());
				//cout<<"Limite GPU "<<j<<" "<<" <= "<<_reservations[iteratoreReservation].getUsedNodes()[j]<<"*"<<_reservations[iteratoreReservation].getNumberOfGPU()/_reservations[iteratoreReservation].getNumberOfNodes()<<endl;
				model.add(usgGPU[j] <= _reservations[iteratoreReservation].getUsedNodes()[j]*_reservations[iteratoreReservation].getNumberOfGPU()/_reservations[iteratoreReservation].getNumberOfNodes());
			}

			//cumulfunction mic
			IloCumulFunctionExprArray usgMIC(env); 
			for (int j = 0; j < _nodes.size(); ++j) 
			{
				usgMIC.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						usgMIC[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfMIC()/_jobs[i].getNumberOfNodes());
					}
				}
				//model.add(usgMIC[j] <= _nodes[j].getMICsNumber());
				//cout<<"Limite MIC "<<j<<" <= "<<_reservations[iteratoreReservation].getUsedNodes()[j]<<"*"<<_reservations[iteratoreReservation].getNumberOfMIC()/_reservations[iteratoreReservation].getNumberOfNodes()<<endl;
				model.add(usgMIC[j] <= _reservations[iteratoreReservation].getUsedNodes()[j]*_reservations[iteratoreReservation].getNumberOfMIC()/_reservations[iteratoreReservation].getNumberOfNodes());
				
			}
		
			//cumulfunction mem
			IloCumulFunctionExprArray usgMem(env); 
			for (int j = 0; j < _nodes.size(); ++j) 
			{
				
				usgMem.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
						usgMem[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getMemory()/_jobs[i].getNumberOfNodes());
					}
				}
				//model.add(usgMem[j] <= _nodes[j].getTotalMemory());
				//cout<<"Limite MEM "<<j<<" <= "<<_reservations[iteratoreReservation].getUsedNodes()[j]<<"*"<<_reservations[iteratoreReservation].getMemory()/_reservations[iteratoreReservation].getNumberOfNodes()<<endl;
				model.add(usgMem[j] <= _reservations[iteratoreReservation].getUsedNodes()[j]*_reservations[iteratoreReservation].getMemory()/_reservations[iteratoreReservation].getNumberOfNodes());
				
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
					//cout<<"QT (S-"<<qt<<")*"<<priority<<" ";
				}
				//cout<<endl;
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
			}*/
			/*for(int i=0;i<_jobs.size();i++)
			{
cout<<cp.domain(task[i])<<endl;
			}
			
			for(int i=0;i<_jobs.size();i++)
			{
				std::vector<int> nodes(_nodes.size(),0);
				for(int j=0;j<_nodes.size();j++)
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)&& k<1;k++)
					{
						cout<<cp.domain(UtilNodes[i][j][0])<<endl;
					}
				}
			}
				cout<<model<<endl;
	
				//getchar();
			}*/
			//cout << "MODEL: " << model << endl;

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
					//std::cout << _jobs[i].toString(0,0)<<std::endl;
				std::cout << cp.domain(task[i])<<std::endl;
				}*/
			/*cout<<"Sono qui "<<_jobs.size() <<endl;
			cp.printInformation(std::cout);
			for(int i=0;i<_jobs.size();i++)
			{
				cout<<"Entro loop1"<<endl;
				std::vector<int> nodes(_nodes.size(),0);
				for(int j=0;j<_nodes.size();j++)
				{
				cout<<"    Entro loop2"<<endl;
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
					{
				cout<<"        Entro loop3 "<<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes)<<endl;
						cout<<cp.domain(UtilNodes[i][j][k])<<endl;
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
				// DE-COMMENT THIS TO SEE HOW THE SOLUTION LOOKS LIKE
				/*std::cout <<"QUI"<<std::endl;
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					//std::cout << _jobs[i].toString(0,0)<<std::endl;
				std::cout << cp.domain(task[i])<<std::endl;
				}
				std::cout << "FINE"<<std::endl;*/
				//getchar();
				if(cp.getObjValue()!=0)
				{
					_os=cp.getObjValue();
					if(objectiveFunction==MAKESPAN_OBJF)
						_os+=minEqt;
				}
		//cout<<"1"<<endl;
			}
		//cout << "1 e 1/2" << endl;
			if(solved)
			{
		//cout<<"2"<<endl;
				for(int i=0;i<_jobs.size();i++)
				{
		//cout<<"3"<<endl;
					if(cp.isPresent(task[i]))
					{
						std::vector<int> nodes(_nodes.size(),0);
						for(int j=0;j<_nodes.size();j++)
						{
		//cout<<"4"<<endl;
							for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j],_queue.getQueue(_jobs[i]),_nodes);k++)
							{
		//cout<<"5"<<endl;
								if(cp.isPresent(UtilNodes[i][j][k]))
								{
		//cout<<"6"<<endl;
									nodes[j]++;
		//cout<<"7"<<endl;
								}
					
							}
						}
						_jobs[i].allocate(cp.getStart(task[i]),nodes);
						_jobs[i].setScheduled(true);
						//cout<<"Job Presente:"<<_jobs[i].toString(0,0)<<endl;
					}
					else
					{
						//cout<<"Job NoN Presente:"<<_jobs[i].toString(0,0)<<endl;
					}
				}
			}

			// terminate search
			cp.endSearch();
			//se la ricerca non ha prodotto risultati a causa di un timeLimit troppo stringente: aumento il time limit e ritento, 
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
				break;
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
	 //getchar();
		}while((numberOfOptionals>0) && iteratoreReservation<_reservations.size());
		
	}
	catch (IloException& ex) 
	{
		std::string m(ex.getMessage());
		if(m.find("Empty array in expression")==string::npos) //se l'eccezione è Empty array in expression allora è una eventualità normalissima (sono finiti i job in coda)
		{
			
			std::cout << "Error: " << m <<". Exception type " <<typeid(ex).name()<< std::endl;
		
			//getchar();
			exit(0);
		}
	}
	//cout<<"exit solve model"<<endl;
	//*secondiEsecuzione=firstSolution;
	return _jobs;
}
#endif
