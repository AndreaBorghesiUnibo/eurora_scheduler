/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                 File: Scheduler4.hpp                                                 *
*                                                                                                                      *
*                                                                                                                      *
 **********************************************************************************************************************/

#ifndef Def_Scheduler4
#define Def_Scheduler4
//#include "IModelComplete.hpp"
#include "Model27.hpp"
#include "ObjFDefine.hpp"
#include <typeinfo>
#include "QueueArray.hpp"
#include "NodeArray.hpp"
#include "JobArray.hpp"
#include "Checker.hpp"
//#include "Util.hpp"
#include "Instant.hpp"
#include <string>    
#include <stdlib.h>
#include <sstream>
#include <sys/stat.h>




class Scheduler4
{
private:
	Instant _instant;
	QueueArray _queues;
	NodeArray _nodes;
	JobArray _jobs;
	//JobArray _original;
	Model27 _model;
	//std::string _fJob;
	int _numIteration;
	std::string _directory;
	int _initialTime;
	int _tempoEsecuzione;
	bool _lastSolutionFound;

	/*JobArray getOriginal(JobArray res)
	{
		std::vector<Job> sel;
		for(int i=0;i<res.size();i++)
		{
			for(int j=0;j<_original.size();j++)
			{
				if(res[i].getJobId().compare(_original[j].getJobId()))
				{
					sel.push_back(_original[j]);
					break;
				}
			}
		}
		return JobArray(sel);
	}*/

	JobArray solveModel(int refTime, int objectiveFunction,time_t * endFeasibility1Check,time_t * endFeasibility2Check,time_t * endJobsSubdivision,time_t* modelCreationEnd,time_t * endModelSolve,time_t * endReservationSubdiviosn,time_t * endReservationSolve,time_t * endJobsStatusWriteBack)
	{
		
		std::cout<<"Ho n jobs pre selezione: "<<_jobs.size()<<std::endl;
		checkFeasibility(refTime,endFeasibility1Check,endFeasibility2Check);
		
		JobArray sel;
		JobArray reservation;
		JobArray resvJobSel;
		for(int i=0;i<_jobs.size();i++)
		{
			if(!_jobs[i].isHolding() && _jobs[i].getFeasibility(_nodes.getStates())==1 && !_jobs[i].isReservationJob())
			{
				sel.Add(_jobs[i]);
			}
			if(_jobs[i].isReservation() && !_jobs[i].isReservationJob())
			{
				reservation.Add(_jobs[i]);
			}
			else if(_jobs[i].isReservationJob())
			{
				resvJobSel.Add(_jobs[i]);
			}
		}
		std::cout<<"Entro solve 27 con jobs "<<sel.size()<<std::endl;
		bool risolto=false,riprova=false;
		int totaleEsecuzione=0;
		int count=0;
		int err;
		JobArray res;
		bool e=false;
		std::vector<bool> errorNodes(_nodes.size(),false);
		if((err=Checker::CheckJobArray(sel,_nodes))<=0)
		{
			e=true;
			std::cout<<"LA SITUAZIONE DI PARTENZA E' CRITICA: Errore "<<err<<" #########################################################################################################################"<<std::endl;
			std::vector< std::vector<int> > risorse(_nodes.size());
			for(int i=0;i<_nodes.size();i++)
			{
				
				risorse[i].push_back(_nodes[i].getCoresNumber());
				risorse[i].push_back(_nodes[i].getGPUsNumber());
				risorse[i].push_back(_nodes[i].getMICsNumber());
				risorse[i].push_back(_nodes[i].getTotalMemory());
			}
			for(int i=0;i<_jobs.size();i++)
			{
				if(_jobs[i].isScheduled() && !_jobs[i].isReservation())
				{
					std::vector<int> usedNodes=_jobs[i].getUsedNodes();
					for(unsigned int j=0;j<usedNodes.size();j++)
					{
						risorse[j][0]-=usedNodes[j]*_jobs[i].getNumberOfCores()/_jobs[i].getNumberOfNodes();
						risorse[j][1]-=usedNodes[j]*_jobs[i].getNumberOfGPU()/_jobs[i].getNumberOfNodes();
						risorse[j][2]-=usedNodes[j]*_jobs[i].getNumberOfMIC()/_jobs[i].getNumberOfNodes();
						risorse[j][3]-=usedNodes[j]*_jobs[i].getMemory()/_jobs[i].getNumberOfNodes();
					}
				}
			}
			for(int i=0;i<_nodes.size();i++)
			{
				for(int j=0;j<_nodes.getNumberOfResources();j++)
				{
					if(risorse[i][j]<0)
					{
						errorNodes[i]=true;
						std::cout<<"Error nodes "<<i<<std::endl;
					}
				}
			}
			getchar();
		}
		time(endJobsSubdivision);
		time_t prec=*endJobsSubdivision;
		*modelCreationEnd=0;
				time_t temp;
		if(sel.size()>0)
		{
			bool startingFromSubProblem=false;	
			do{
				_tempoEsecuzione+=3;
				if(_tempoEsecuzione<3)
					_tempoEsecuzione=3;
				if(!_lastSolutionFound && count<=1 && !e)
				{
					startingFromSubProblem=true;
					int removed=0;
					int size=sel.size();
					int toRemove=size/2;
					for(int i=size-1;i>=0&&removed<toRemove;i--)
					{
						if(!sel[i].isReservation() && !sel[i].isScheduled())
						{
							sel.remove(i);
							removed++;
						}
					}
				}
				else
				{
					startingFromSubProblem=false;
				}
				 res=_model.solve(sel,_queues,_nodes,refTime,objectiveFunction,&_tempoEsecuzione,&risolto,&totaleEsecuzione,&riprova,false,false,true,&temp,errorNodes);
				//_tempoEsecuzione=totaleEsecuzione;
				*modelCreationEnd+=temp-prec;
				if(!risolto && riprova)
				{
					if(totaleEsecuzione<1)
						totaleEsecuzione=1;
					_tempoEsecuzione=totaleEsecuzione*2;
					if(count>0 || _tempoEsecuzione>16)
					{
						int removed=0;
						int size=sel.size();
						int toRemove=size/2;
						for(int i=size-1;i>=0&&removed<toRemove;i--)
						{
							if(!sel[i].isReservation() && !sel[i].isScheduled())
							{
								sel.remove(i);
								removed++;
							}
						}
					}
				}
				std::cout<<"Tentativo "<<count<<" risolto:"<<risolto<<" riprovo:"<<riprova<<" tempoTot:"<<totaleEsecuzione<<" T1Sol:"<<_tempoEsecuzione<<" getchar"<<std::endl;
				if(!risolto && riprova==false && count==0)
				{
					riprova=true;
					_lastSolutionFound=risolto;
				}
				count++;
				time(&prec);
			}while(!risolto && riprova && _tempoEsecuzione<320 );
			if(!startingFromSubProblem)
			{
				_lastSolutionFound=risolto;
			}
			else
			{
				_lastSolutionFound=false;
			}
			if(_tempoEsecuzione>=320)
			{
				_tempoEsecuzione=60;
			}
		}
		
		*modelCreationEnd+=*endJobsSubdivision;
		//std::cout<<*endJobsSubdivision<<" "<<temp<<" "<<prec<<" "<<*modelCreationEnd<<std::endl;
		//getchar(); 
		if((err=Checker::CheckJobArray(res,_nodes))<=0)
		{
			for(int i=0; i<res.size();i++)
			{
				std::cout<<res[i].toString(refTime,_initialTime)<<std::endl;
			}
			std::cout<<"Solution Error: Not respected constraints in model result - "<<err<<std::endl;
		}

		for(int i=0;i<reservation.size();i++)
		{
			for(int j=0;j<res.size();j++)
			{
				if(res[j].getJobId().compare(reservation[i].getJobId())==0)
				{
					reservation[i].allocate(res[j].getStartTime(),res[j].getUsedNodes());
					reservation[i].setRealDuration(res[j].getRealDuration());
					reservation[i].setLastMakespan(res.getMakespan());
				}
			}
		}
		time(endModelSolve);
	
		std::vector<JobArray> reservationSubdivision;
		for(int i=0;i<reservation.size();i++)
		{
			bool trovato=false;
			for(unsigned int j=0;j<reservationSubdivision.size();j++)
			{
				if(reservationSubdivision[j][0].getQueue().compare(reservation[i].getQueue())==0 && ((reservation[i].getStartTime()<= refTime && reservation[i].getFeasibility(_nodes.getStates())!=0) ||  reservation[i].isScheduled()))
				{	
					reservationSubdivision[j].Add(reservation[i]);
					trovato=true;
				}
			}
			if(!trovato && ((reservation[i].getStartTime()<= refTime && reservation[i].getFeasibility(_nodes.getStates())!=0) ||  reservation[i].isScheduled()))
			{	
				JobArray t;
				t.Add(reservation[i]);
				reservationSubdivision.push_back(t);
			}
		}
		std::cout<<"Reservation size "<<reservation.size()<<std::endl;
		std::cout<<"Reservation subdivizion size "<<reservationSubdivision.size()<<std::endl;
		for(unsigned int i=0;i<reservationSubdivision.size();i++)
		{
			for(int j=0;j<reservationSubdivision[i].size();j++)
				std::cout<<i<<" "<<reservationSubdivision[i][j].toString(0,0)<<std::endl;
			//std::cout<<std::endl;
		}
		time(endReservationSubdiviosn);
		for(unsigned int i=0;i<reservationSubdivision.size();i++)
		{
			JobArray temp;
			for(int j=0;j<resvJobSel.size();j++)
			{
				if(resvJobSel[j].getQueue().compare(reservationSubdivision[i][0].getQueue())==0)
				{
					temp.Add(resvJobSel[j]);	
				}
			}
			int tempo=300;
			JobArray iesimaResv=reservationSubdivision[i];	
			std::cout<<"Job in Reservation "<<reservationSubdivision[i][0].getQueue()<<": "<<temp.size()<<std::endl;
			if(temp.size()>0)
			{
				JobArray resResv=_model.solveReservation(temp,iesimaResv,_queues,_nodes,refTime,MAX_JOBS,tempo);
				std::cout<<"Uscito solveReservation"<<std::endl;
				std::cout<<"resResv size "<< resResv.size()<<std::endl;
				int err;
				if((err=Checker::CheckJobArray(resResv,_nodes))<=0)
				{
					for(int i=0; i<resResv.size();i++)
					{
						std::cout<<res[i].toString(refTime,_initialTime)<<std::endl;
					}
					std::cout<<"Solution Error: Not respected constraints in model result - "<<err<<std::endl;
				}
				//std::cout<<"1"<<std::endl;
				for(int i=0;i<_jobs.size();i++)
				{
					for(int j=0;j<resResv.size();j++)
					{
						if(resResv[j].getJobId().compare(_jobs[i].getJobId())==0)
						{
							_jobs[i].allocate(resResv[j].getStartTime(),resResv[j].getUsedNodes());
							_jobs[i].setRealDuration(resResv[j].getRealDuration());
							_jobs[i].setLastMakespan(resResv.getMakespan());
						}
					}
				}
				//std::cout<<"2"<<std::endl;
			}
		}
		time(endReservationSolve);
				//std::cout<<"3"<<std::endl;
		for(int i=0;i<_jobs.size();i++)
		{
			for(int j=0;j<res.size();j++)
			{
				if(res[j].getJobId().compare(_jobs[i].getJobId())==0)
				{
					_jobs[i].allocate(res[j].getStartTime(),res[j].getUsedNodes());
					_jobs[i].setRealDuration(res[j].getRealDuration());
					_jobs[i].setLastMakespan(res.getMakespan());
				}
			}
		}
		
				//std::cout<<"4"<<std::endl;
		_numIteration++;
		time(endJobsStatusWriteBack);
		std::cout<<"Uscito solve"<<std::endl;
		return _jobs;
	}

	void init(std::string directory)
	{
		_tempoEsecuzione=0;
		_numIteration=0;
		_directory=directory;
	}
	
	
public:
	void checkFeasibility(int refTime,time_t * endFeasibility1Check,time_t * endFeasibility2Check)
	{
		
		std::cout<<"START checkFeasibility()"<<std::endl;
		std::cout<<"checkFeasibility() reservations"<<std::endl;
		int totaleEsecuzione=0,te=0;
		std::vector<bool> errorNodes(_nodes.size(),false);
		JobArray sel1;
		bool risolto=false,riprova=false,count=0;
		std::vector<bool> nodesState=_nodes.getStates();
		JobArray res1;
		bool esegui=false;
		
		for(int i=0;i<_jobs.size();i++)
		{
			if(!_jobs[i].isHolding() && (_jobs[i].isScheduled() || _jobs[i].isReservation()) && !_jobs[i].isReservationJob())
			{
				sel1.Add(_jobs[i]);
				std::cout<<"Checking Feasibility "<<_jobs[i].getJobId()<<std::endl;
				if( _jobs[i].isReservation())
					esegui=true;
			}
		}
		
		if(sel1.size()>0 && esegui)
		{
			int err;
			
			std::vector<bool> errorNodes(_nodes.size(),false);
                	if((err=Checker::CheckJobArray(sel1,_nodes))<=0)
                	{
            
                       	 	std::vector< std::vector<int> > risorse(_nodes.size());
                       	 	for(int i=0;i<_nodes.size();i++)
                       	 	{
                                	risorse[i].push_back(_nodes[i].getCoresNumber());
                                	risorse[i].push_back(_nodes[i].getGPUsNumber());
                                	risorse[i].push_back(_nodes[i].getMICsNumber());
                                	risorse[i].push_back(_nodes[i].getTotalMemory());
                        	}
                        	for(int i=0;i<_jobs.size();i++)
                        	{
                               	 	if(_jobs[i].isScheduled() && !_jobs[i].isReservation())
                                	{
                                	        std::vector<int> usedNodes=_jobs[i].getUsedNodes();
                                	        for(unsigned int j=0;j<usedNodes.size();j++)
                                	        {
                                	                risorse[j][0]-=usedNodes[j]*_jobs[i].getNumberOfCores()/_jobs[i].getNumberOfNodes();
                                       		         risorse[j][1]-=usedNodes[j]*_jobs[i].getNumberOfGPU()/_jobs[i].getNumberOfNodes();
                                        	        risorse[j][2]-=usedNodes[j]*_jobs[i].getNumberOfMIC()/_jobs[i].getNumberOfNodes();
                                        	        risorse[j][3]-=usedNodes[j]*_jobs[i].getMemory()/_jobs[i].getNumberOfNodes();
                                        	}
                                	}
                        	}
				for(int i=0;i<_nodes.size();i++)
				{
	                                for(int j=0;j<_nodes.getNumberOfResources();j++)
	                                {
	                                        if(risorse[i][j]<0)
	                                        {
	                                                errorNodes[i]=true;
	                                                std::cout<<"Error nodes "<<i<<std::endl;
	                                        }
	                                }
	                        }
	                }

			do{
				te+=2;
				time_t unused;
				if(te<2)
					te=2;
				//std::cout<<"A"<<std::endl;
				
				res1=_model.solve(sel1,_queues,_nodes,refTime,MAX_JOBS,&te,&risolto,&totaleEsecuzione,&riprova,true,true,false,&unused,errorNodes);
				if(!risolto && riprova)
				{
					if(totaleEsecuzione<1)
						totaleEsecuzione=1;
					te=totaleEsecuzione*5;
				}
				//std::cout<<"Tentativo "<<count<<" risolto:"<<risolto<<" riprovo:"<<riprova<<" tempoTot:"<<totaleEsecuzione<<" T1Sol:"<<te<<std::endl;
				count++;
				//getchar();
			}while(!risolto && riprova && te<1250 );
		}
		else
		{
			std::cout<<"Nessun job in checkFeasibility()"<<std::endl;
		}
		
		std::vector<std::string> ids;
		std::vector<int> feas;
		for(int i=0;i<res1.size();i++)
		{
			bool insert=false;
			std::vector<std::string> sottonome1=Util::split(res1[i].getJobId(),"/");
			std::vector<std::string> sottonome=Util::split(sottonome1[0],"(");
			std::cout<<"Sottonome "<<sottonome[0]<<std::endl;
			for(unsigned int j=0;j<ids.size();j++)
			{
				if(ids[j].compare(sottonome[0])==0)
				{	
					insert=true;
					if(feas[j]>0)
					{
						feas[j]=res1[i].getFeasibility(nodesState);
						std::cout<<"Job "<<sottonome[0]<<" feasibility "<<res1[i].getFeasibility(nodesState)<<std::endl;
					}				
				}
			}
			if(insert==false)
			{
				ids.push_back(sottonome[0]);
				feas.push_back(res1[i].getFeasibility(nodesState));
				std::cout<<"IJob "<<sottonome[0]<<" feasibility "<<res1[i].getFeasibility(nodesState)<<std::endl;
			}
		}
		for(unsigned int i=0;i<ids.size();i++)
		{
			for(int j=0;j<_jobs.size();j++)
			{
				if(_jobs[j].getJobId().find(ids[i])==0)
				{
					_jobs[j].setFeasibility(nodesState,feas[i]);
					_jobs[j].toString(0,0);
				}
			}
		}
		
		time(endFeasibility1Check);
		std::cout<<"checkFeasibility() jobs"<<std::endl;
		
		for(int i=0;i<_jobs.size();i++)
		{
			JobArray sel2;
			te=0;
			if(!_jobs[i].isHolding() && !_jobs[i].isReservation() && _jobs[i].getFeasibility(nodesState)<=0)
			{
				sel2.Add(_jobs[i]);
				std::cout<<"Checking Feasibility "<<_jobs[i].getJobId()<<std::endl;
				std::vector<std::string> sottonome1=Util::split(_jobs[i].getJobId(),"/");
				std::vector<std::string> sottonome=Util::split(sottonome1[0],"(");
				for(int j=0;j<_jobs.size();j++)
				{
					if(_jobs[j].getJobId().find(sottonome[0])<_jobs[j].getJobId().size() && i!=j)
					{	
						std::cout<<"Checking Feasibility "<<_jobs[j].getJobId()<<std::endl;
						sel2.Add(_jobs[j]);
					}
				}
				//getchar();
				do{
					te+=1;
					time_t unused;
					if(te<1)
						te=1;
					//std::cout<<"B"<<std::endl;
					res1=_model.solve(sel2,_queues,_nodes,refTime,MAX_JOBS,&te,&risolto,&totaleEsecuzione,&riprova,false,true,false,&unused,errorNodes);
					if(!risolto && riprova)
					{
						if(totaleEsecuzione<1)
							totaleEsecuzione=1;
						te=totaleEsecuzione*5;
					}
					//std::cout<<"Tentativo "<<count<<" risolto:"<<risolto<<" riprovo:"<<riprova<<" tempoTot:"<<totaleEsecuzione<<" T1Sol:"<<te<<std::endl;
					count++;
					//getchar();
				}while(!risolto && riprova && te<1250 );
				for(int i=0;i<res1.size();i++)
				{
					bool insert=false;
					std::vector<std::string> sottonome1=Util::split(res1[i].getJobId(),"/");
					std::vector<std::string> sottonome=Util::split(sottonome1[0],"(");
					for(unsigned int j=0;j<ids.size();j++)
					{
						if(ids[j].compare(sottonome[0])==0 && feas[j]!=0)
						{	
							insert=true;
							feas[j]=res1[i].getFeasibility(nodesState);
							//std::cout<<"Job "<<sottonome[0]<<" feasibility "<<res1[i].getFeasibility(nodesState)<<std::endl;
						}
					}
					if(insert==false)
					{
						ids.push_back(sottonome[0]);
						feas.push_back(res1[i].getFeasibility(nodesState));
						//std::cout<<"Job "<<sottonome[0]<<" feasibility "<<res1[i].getFeasibility(nodesState)<<std::endl;
					}
				}
				for(unsigned int i=0;i<ids.size();i++)
				{
					for(int j=0;j<_jobs.size();j++)
					{
						if(_jobs[j].getJobId().find(ids[i])==0)
						{
							_jobs[j].setFeasibility(nodesState,feas[i]);
							_jobs[j].toString(0,0);
						}
					}
				}
			}
		}
		std::cout<<"END checkFeasibility()"<<std::endl;
		time(endFeasibility2Check);
	}

	/*std::vector<Job> readReservations()
	{
		JobReader r;	
		
		return r.readReservations(_nodes);
	}*/
	
	bool terminateJob(std::string jobId)
	{
		for( int i=0;i<_jobs.size();i++)
		{
			unsigned int pos;
			//std::cout<<"Checking "<<_jobs[i].getJobId()<<" =?= "<<jobId;
			if((pos=_jobs[i].getJobId().find(jobId))<_jobs[i].getJobId().size())
			{
				//std::cout<<" YES ";
				_jobs.remove(i);
				return true;
			}
			if((pos=jobId.find(_jobs[i].getJobId()))<jobId.size())
			{
				//std::cout<<" YES ";
				_jobs.remove(i);
				return true;
			}
			//std::cout<<std::endl;
		}
		if(_lastSolutionFound==false)	
			return true;
		return false;
	}

	std::string getDirectory() 
	{
		return _directory;
	}
	
	void updateJob(std::string id)
	{
		terminateJob(id);
		JobReader r;
		std::vector<Job> j=r.readJob(id,_nodes);
		for(unsigned int i=0;i<j.size();i++)
		{
			_jobs.Add(j[i]);
		}
	}

	bool updateJobsList()
	{
		checkNodeState();
		
		JobArray updated(_jobs.toArray(),_nodes);
		//std::vector<Job> resv=readReservations();
		/*for(unsigned int i=0;i<resv.size();i++)
		{
			updated.push_back(resv[i]);
		}*/
		if(_lastSolutionFound==false || _jobs.size()!=updated.size() )	
		{
			_jobs=updated;
			return true;
		}
		for(int i=0;i<_jobs.size();i++)
		{
			bool found=false;
			if(_jobs[i].getQrunError())
			{
				return true;
			}
			for(int j=0;j<updated.size();j++)
			{

				unsigned int pos;
				if((pos=_jobs[i].getJobId().find(updated[j].getJobId()))<_jobs[i].getJobId().size())
				{
					found=true;
				}
			}
			if(!found)
			{
				_jobs=updated;
				return true;
			}
		}
		
		for(int j=0;j<updated.size();j++)
		{
			bool found=false;
			for(int i=0;i<_jobs.size();i++)
			{
				unsigned int pos;
				if((pos=_jobs[i].getJobId().find(updated[j].getJobId()))<_jobs[i].getJobId().size())
				{
					found=true;
				}
			}
			if(!found)
			{
				_jobs=updated;
				return true;
			}
		}
		return false;
	}

	void execute(int initialTime,int istant)
	{
		int utile=0;
		for(int i=0;i<_jobs.size();i++)
		{
			_jobs[i].setShortConfirmed(false);
		}
		for(int i=0;i<_jobs.size();i++)
		{
			if(!_jobs[i].isReservationJob())
			{
				int temp=_jobs[i].execute(_nodes,_jobs.toArrayPointer(),initialTime,istant);
				if(temp>0)
					utile=1;
			}		
		}	
		for(int i=0;i<_jobs.size();i++)
		{
			if(_jobs[i].isReservationJob())
			{
				int temp=_jobs[i].execute(_nodes,_jobs.toArrayPointer(),initialTime,istant);
				if(temp>0)
					utile=1;
			}		
		}		 
		std::stringstream fileUtile;
		fileUtile<<"SchedulingUtili";
		std::ofstream output(fileUtile.str().c_str(), std::ofstream::app);
		if (!output) 
		{
			std::cout << "ERROR: Could not open file \"" << fileUtile.str() << "\"" << std::endl;
			getchar();
		}
		output<<utile<<";"<<std::endl;
		output.close();
	}

	void checkNodeState()
	{
		for(int i=0;i<_nodes.size();i++)
		{
			_nodes[i].checkState();
			_nodes[i].toString();
		}
		std::vector<bool> stati=_nodes.getStates();
		std::cout<<"Stato nodi: ";
		for(unsigned int i=0;i<stati.size();i++)
		{
			std::cout<<stati[i];
		}
		std::cout<<std::endl;	
	}
	int getInitialTime()
	{
		return _initialTime;
	}

	JobArray getResult()
	{
		return _jobs;
	}
	Model27 getModel()
	{
		return _model;
	}
	Scheduler4(std::string fNode,std::string directory)
	{
		_lastSolutionFound=true;
		std::cout<<"Reading Queues"<<std::endl;
		_queues=QueueArray();
		std::cout<<"Reading Nodes"<<std::endl;
		_nodes=NodeArray(fNode);
		std::cout<<"Reading Intervals"<<std::endl;	
		_instant=Instant();
		std::cout<<"Init"<<std::endl;
		init(directory);
	}
	Scheduler4(std::string directory)
	{
		_lastSolutionFound=true;
		std::cout<<"Reading Queues"<<std::endl;
		_queues=QueueArray();
		std::cout<<"Reading Nodes"<<std::endl;
		_nodes=NodeArray();
		std::cout<<"Reading Intervals"<<std::endl;	
		_instant=Instant();
		std::cout<<"Init"<<std::endl;
		init(directory);
	}
	
	int findNextIstant(int fromTime)
	{
		return _jobs.findNextIstant(fromTime);
	}

	JobArray Solve(int refTime,int numModelli,int orderWT,double wtDelta, int orderNL, double numLateDelta,int orderMK,double makespanDelta, std::string * stringaTempi)
	{
		time_t start,endWalltimeCheck,endQueueCheck,endNodesCheck,endIntervalsCheck;
		time_t endFeasibility1Check, endFeasibility2Check, endJobsSubdivision,modelCreationEnd, endModelSolve, endReservationSubdiviosn,endReservationSolve, endJobsStatusWriteBack;
		time(&start);
		for(int i=0;i<_jobs.size();i++)
		{
			if(_jobs[i].isScheduled() && !_jobs[i].isHolding() && _jobs[i].getStartTime()+_jobs[i].getEstimatedDuration()<=refTime)
			{
				_jobs[i].setEstimatedDuration(refTime+1-_jobs[i].getStartTime());
				_jobs[i].setRealDuration(refTime+1-_jobs[i].getStartTime());
				std::cout<<"Walltime sforato dal job "<< _jobs[i].getJobId()<<std::endl;
			}
			//std::cout<<_jobs[i].getJobId()<<" real dur "<<_jobs[i].getRealDuration()<<std::endl;
		}
		
		time(&endWalltimeCheck);
		double solutionWT=0,solutionNL=0,solutionMK=0;
		//_queues=QueueArray(); //CHECK
		time(&endQueueCheck);
		_nodes.updateState();
		time(&endNodesCheck);
		_model.setIntervals( &_instant);
		time(&endIntervalsCheck);
		JobArray res;
		for(int i=0;i<numModelli;i++)
		{
			if(solutionNL!=0 && numLateDelta!=0)
			{
				_model.setNumLate(solutionNL,numLateDelta);
			}
			else
			{
				_model.setNumLate(-1,numLateDelta);
			}
			if(solutionMK!=0 && makespanDelta!=0)
			{
				_model.setMK(solutionMK,makespanDelta);
			}
			else
			{
				_model.setMK(-1,makespanDelta);
			}
			if(solutionWT!=0 && wtDelta!=0)
			{
				_model.setWT(solutionWT,wtDelta);
			}
			else
			{
				_model.setWT(-1,wtDelta);
			}
			if(i==orderWT)
			{
				std::cout<<"Eseguo Weighted time in queue "<<std::endl;
				res=solveModel(refTime,WEIGHTED_QUEUE_TIME_OBJF,&endFeasibility1Check, &endFeasibility2Check, &endJobsSubdivision,&modelCreationEnd, &endModelSolve,&endReservationSubdiviosn,&endReservationSolve, &endJobsStatusWriteBack);		std::cout<<"Uscito solveModel"<<std::endl;
				solutionWT=_model.getOptimalSolution();
			}
			if(i==orderNL)
			{
				std::cout<<"Eseguo NUM JOB in Late"<<std::endl;
				res=solveModel(refTime,NUM_JOB_IN_LATE_OBJF,&endFeasibility1Check, &endFeasibility2Check, &endJobsSubdivision, &modelCreationEnd,&endModelSolve,&endReservationSubdiviosn,&endReservationSolve, &endJobsStatusWriteBack);
				std::cout<<"Uscito solveModel"<<std::endl;
				solutionNL=_model.getOptimalSolution();
			}
			if(i==orderMK)
			{
				std::cout<<"Eseguo Makespan"<<std::endl;
				res=solveModel(refTime,MAKESPAN_OBJF,&endFeasibility1Check, &endFeasibility2Check, &endJobsSubdivision,&modelCreationEnd, &endModelSolve,&endReservationSubdiviosn,&endReservationSolve, &endJobsStatusWriteBack);
				std::cout<<"Uscito solveModel"<<std::endl;
				solutionMK=_model.getOptimalSolution();
			}
		}
		std::stringstream strTempi;
		std::cout<<"Calcolo tempi"<<std::endl;
		//std::cout<<"Wallcheck"<<endWalltimeCheck-start<<" Queue"<<endQueueCheck-endWalltimeCheck<<" Node"<<endNodesCheck-endQueueCheck<<" Intervals"<<endIntervalsCheck-endNodesCheck<<" ResFeas"<<endFeasibility1Check-endIntervalsCheck<<" JobsFeas"<<endFeasibility2Check-endFeasibility1Check<<" JobSubd"<<endJobsSubdivision-endFeasibility2Check<<" ModelCreation"<<modelCreationEnd-endJobsSubdivision<<" SolveJob"<<endModelSolve-modelCreationEnd<<" ResvSubd"<<endReservationSubdiviosn-endModelSolve<<" ResvSolve"<<endReservationSolve-endReservationSubdiviosn<<" JobsWriteBack"<<endJobsStatusWriteBack-endReservationSolve<<std::endl;
		strTempi<<endWalltimeCheck-start<<"+"<<endQueueCheck-endWalltimeCheck<<"+"<<endNodesCheck-endQueueCheck<<"+"<<endIntervalsCheck-endNodesCheck<<"+"<<endFeasibility1Check-endIntervalsCheck<<"+"<<endFeasibility2Check-endFeasibility1Check<<"+"<<endJobsSubdivision-endFeasibility2Check<<"+"<<modelCreationEnd-endJobsSubdivision<<"+"<<endModelSolve-modelCreationEnd<<"+"<<endReservationSubdiviosn-endModelSolve<<"+"<<endReservationSolve-endReservationSubdiviosn<<"+"<<endJobsStatusWriteBack-endReservationSolve;
		*stringaTempi=strTempi.str();
		//getchar();
		std::cout<<"uscita model"<<std::endl;
		return res;	
	}

	void saveJobArray(std::string file)
	{
		_jobs.write(file);
	}

};


#endif
