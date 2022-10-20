/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                    File: Job.hpp                                                     *
*                                                                                                                      *
*                                                                                                                      *
 ***********************************************************************************************************************/

#ifndef Def_Job
#define Def_Job
#include <ctime>
#include <vector>
#include <string>
#include "Util.hpp"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <unistd.h>
#include "NodeArray.hpp"
#include <map>
#include <thread>

//using namespace std;
/*struct ByElements: public std::binary_function< std::vector <bool> , std::vector <bool> , bool>
	{
   		bool operator()(const std::vector <bool>& lhs, const std::vector <bool>& rhs) const
   	 	{
			if(lhs.size()!=rhs.size())
				return false;
        		for(unsigned int i=0;i<lhs.size();i++)
			{
				if(lhs[i]!=rhs[i])
					return false;
			}
			return true;
    		}
	};*/
class Job
{
private:
	
	std::string _id;
	std::string _name;
	std::string _userName;
	std::string _coda;
	int _enterQueueTime;
		 
	int _nodes;
	int _cores;
	int _gpus;
	int _mics;
	int _mem;
	int _ghz;
		 
	int _realDuration;
	int _estimatedDuration;

	int _start;
	double _lastMakespan;
	bool _makespanPresent;

	bool _schedulato;
	std::vector<int> _usedNodes;
	int _atId;
	int _terminationStatus;
	bool _confirmed;
	//int _feasibility;
	int min(int a,int b){return (a<b?a:b);}
	//int _lastFeasibility;
	bool _isHold;
	std::vector< std::vector<bool> > _usedMic;
	std::vector< std::vector<bool> > _resvJobUsedMic;	
	std::string _host; 
	bool _shortConfirmed;
	std::vector< std::vector<bool> > _statiNodi;
	std::vector<int> _feasible;
	bool _qrunError;
	std::string _lastConfirmResvString;
	

	void taskAt(int start,int duration)
	{
		std::stringstream command,command2,command3;
		//command<<"res=no;for i in `atq | awk '{ print $2$3}' | tr -d - | tr -d : ` ; do if test $i -eq "<<Util::timeToStrForAtNoSec(start)<<";then res=si; fi;done; if test $res != 'si';then at -f $MODEL26_HOME/util/startreservation.sh -t "<<Util::timeToStrForAt(start)<<" ;fi";
		command<<"at -f $MODEL26_HOME/util/startreservation.sh -t "<<Util::timeToStrForAt(start)<<" 2>/dev/null";
		system(command.str().c_str());
		//command2<<"res=no;for i in `atq | awk '{ print $2$3}' | tr -d - | tr -d : ` ; do if test $i -eq "<<Util::timeToStrForAtNoSec(start+duration)<<";then res=si; fi;done; if test $res != 'si';then at -f $MODEL26_HOME/util/endreservation.sh -t "<<Util::timeToStrForAt(start+duration)<<" ;fi";
		command2<<"at -f $MODEL26_HOME/util/endreservation.sh -t "<<Util::timeToStrForAt(start+duration)<<" 2>/dev/null";
		system(command2.str().c_str());
		//std::cout<<"AT RESERVATION  "<<command.str()<<" -&- "<<command2.str()<<std::endl;		
	}

	void runSystem(std::string cmd)
	{
		system(cmd.c_str());
	}
	
public:
	void setScheduled(bool s)
	{
		_schedulato=s;
		/*if(s)
		{
			_feasibility=1;	
		}*/
	}
	bool isScheduled(){return _schedulato;}
	double getLastMakespan(){return _lastMakespan;}
	bool isMakespanPresent(){return _makespanPresent;}
	void setLastMakespan(double lmks){_lastMakespan=lmks;_makespanPresent=true;}
	std::string getJobId(){return _id;}
	std::string getJobName(){return _name;}
	std::string getUserName(){return _userName;}
	int getEnterQueueTime(){return _enterQueueTime;}
	std::string getQueue(){return _coda;}
	int getNumberOfNodes(){return _nodes;}
	int getNumberOfCores(){return _cores;}
	int getNumberOfGPU(){return _gpus;}
	int getNumberOfMIC(){return _mics;}
	int getMemory(){return _mem;}
	int getEstimatedDuration(){return _estimatedDuration;}
	int getRealDuration(){return _realDuration;}
	int getStartTime(){return _start;}
	int getTerminationStatus(){return _terminationStatus;}
	std::vector<int> getUsedNodes(){return _usedNodes; }

	int getFeasibility(std::vector<bool> state)
	{
		for(unsigned int i=0;i<_statiNodi.size();i++)
		{
			bool trovato=true;
			for(unsigned int j=0;j<state.size();j++)
			{
				if(_statiNodi[i][j]!=state[j])
					trovato=false;
			}
			if(trovato)
			{
				return _feasible[i];
			}
		}
		return -1;
	}
	void setFeasibility(std::vector<bool> state,int f)
	{
		for(unsigned int i=0;i<_statiNodi.size();i++)
		{
			bool trovato=true;
			for(unsigned int j=0;j<state.size();j++)
			{
				if(_statiNodi[i][j]!=state[j])
					trovato=false;
			}
			if(trovato)
			{
				_feasible[i]=f;
				return;
			}
		}
		_statiNodi.push_back(state);
		_feasible.push_back(f);
	}

	std::string getHost(){return _host;}
	int isUsed()
	{
		for(unsigned int i=0;i<getUsedNodes().size();i++)
		{
			if(getUsedNodes()[i]>0)
				return 1;
		}
		return 0;
	}
	bool isReservation()
	{
		if((_coda[0]=='R' || _coda[0]=='S'))
			return true;
		return false;
	}
	bool isStandingReservation()
	{
		if(_coda[0]=='S')
			return true;
		return false;
	}
	bool isConfirmedReservation()
	{
		if(isReservation())
		{
			return _confirmed;
		}
		return false;
	}

	void setConfirmedReservation(bool c)
	{
		
		if(isReservation() && _confirmed==false && c==true)
		{
			//command<<"$PBS_EXEC/bin/qmgr -c \"set queue "<<getQueue()<<" enabled = "<<c<<"\"";
			//std::cout<<command.str()<<std::endl;
			//system(command.str().c_str());
			time_t now=0;
			time(&now);
			if(getStartTime()>now )
			{
				std::thread t1(&Job::taskAt, this,getStartTime(),getRealDuration());
				t1.detach();
				//taskAt(getStartTime(),getRealDuration());
				/*std::stringstream command2;
				command2<<"res=no;for i in `atq | awk '{ print $2$3}' | tr -d - | tr -d : ` ; do if test $i -eq "<<Util::timeToStrForAtNoSec(getStartTime())<<";then res=si; fi;done; if test $res != 'si';then at -f $MODEL26_HOME/util/startreservation.sh -t "<<Util::timeToStrForAt(getStartTime())<<" ;fi";
				//std::cout<<command2.str()<<std::endl;
				system(command2.str().c_str());*/
				//getchar();*/
			}
		}
		_confirmed=c;
	}
	bool isPrimeTime()
	{
		if(_coda[0]=='p' && _coda[1]=='_')
			return true;
		return false;
	}
	bool isNonPrimeTime()
	{
		if(_coda[0]=='n' && _coda[1]=='p' && _coda[2]=='_')
			return true;
		return false;
	}
	bool isAnytime()
	{
		if(_coda[0]=='p' && _coda[1]=='_')
			return false;
		if(_coda[0]=='n' && _coda[1]=='p' && _coda[2]=='_')
			return false;
		return true;
	}
	int getDuration(int atTime)
	{
		if(getStartTime()+min(getEstimatedDuration(),getRealDuration())<= atTime)
		{
			return min(getEstimatedDuration(),getRealDuration());
		}
		return getEstimatedDuration();
	}
		 
	/*bool isAlreadyStarted(int atTime){
	if(getStartTime()<atTime)
	return true;
	return false;
	}
	bool isAlreadyFinished(int atTime)
	{
	if(getStartTime()+min(getRealDuration(),getEstimatedDuration())<=atTime)
	return true;
	return false;
	}*/
		 
	int getGHz(){return _ghz;}
	void setGHz(int ghz)
	{
		_ghz=ghz;
	} 
		 
	void setJobId(std::string id){ _id=id;}
	void setJobName(std::string name){ _name=name;}
	void setUserName(std::string userName){_userName=userName;}
	void setQueue(std::string queue){ _coda=queue;}
	void setEnterQueueTime(int eqt){_enterQueueTime=eqt;}
		 
	void setNumberOfNodes(int n){ _nodes=n;}
	void setNumberOfCores(int n){ _cores=n;}
	void setNumberOfGPU(int n){ _gpus=n;}
	void setNumberOfMIC(int n){ _mics=n;}
	void setMemory(int m){ _mem=m;}
	void setEstimatedDuration(int d){_estimatedDuration=d;}
	void setRealDuration(int d){_realDuration=d;}
	void setStartTime(int start){_start=start;}
	void setTerminationStatus(int t){_terminationStatus=t;}
	void allocate(int s, std::vector<int> nodes){_start=s, _usedNodes=nodes;}
	void setQrunError(bool qe)
	{
		_qrunError=qe;
	}
	bool getQrunError()
	{
		return _qrunError;
	}
	/*void initFeasibility(std::vector<int> f)
	{
		
			_feasibility=feasible;
	}*/
	inline Job(std::string id,std::string name,std::string userName,std::string queue,int eqt, int nodes,int cores,int gpu,int mic,int mem,int ghz,int estDur,int realDur,int start,std::vector<int> used,std::string host){
			_atId=-1;
			setJobId(id);
			setJobName(name);
			setUserName(userName);
			setQueue(queue);
			setEnterQueueTime(eqt);
			setNumberOfNodes(nodes);
			setNumberOfCores(cores);
			setNumberOfGPU(gpu);
			setNumberOfMIC(mic);
			setMemory(mem);
			setEstimatedDuration(estDur);
			setRealDuration(realDur);
			setGHz(ghz);
			allocate(start,used);
			_makespanPresent=false;
			_lastMakespan=-1;
			_schedulato=false;
			_terminationStatus=0;
			_confirmed=false;
			//_feasibility=feasible;
			//for(unsigned int i=0;i<used.size();i++)
			//_feasibility=-1;
			//_lastFeasibility=-1;
			_isHold=0;
			_usedMic=std::vector< std::vector<bool> >(used.size(), std::vector<bool>(2,false));
			_resvJobUsedMic=std::vector< std::vector<bool> >(used.size(), std::vector<bool>(2,false));
			_host=host;
			_shortConfirmed=false;
			_qrunError=false;
			//_lastConfirmResvString();
		}
		void setShortConfirmed(bool c)
		{
			_shortConfirmed=c;
		}
		bool isShortConfirmed()
		{
			return _shortConfirmed;
		}
		bool getUsedMIC(int j,int n)
		{
			return _usedMic[j][n];
		}
		void setUsedMIC(int j,int n,bool val)
		{
			_usedMic[j][n]=val;
		}
		bool getResvJobUsedMIC(int j,int n)
		{
			return _resvJobUsedMic[j][n];
		}
		void setResvJobUsedMIC(int j,int n,bool val)
		{
			_resvJobUsedMic[j][n]=val;
		}
		void setHolding(int h)
		{
			_isHold=h;
		}
		 
		int isHolding()
		{
			return _isHold;
		}


		std::string toString(int now,int initialTime)
		{
			std::stringstream s("");
			if(isHolding())
				s<<"H ";
			else if(isScheduled()==false)
				s<<"N ";
			else
				s<<"R ";
			s<<getJobId()<<" "<< Util::timeToStr(getEnterQueueTime())<<" ["<<Util::timeToStr(getStartTime())<<"->"<<getRealDuration()<<"->"<<Util::timeToStr(getRealDuration()+getStartTime())<<"] [";
			for(unsigned int j=0;j<getUsedNodes().size();j++)
			{
				s<<getUsedNodes()[j];
				if(j==31 || j==63)
					s<<"|";
			}
			s<<"] "<<_schedulato<<" nodi:"<<getNumberOfNodes()<<" cores:"<<getNumberOfCores()<<" GHz:"<<_ghz<<" gpu:"<<getNumberOfGPU()<<" mic:"<<getNumberOfMIC()<<" mem:"<<getMemory()<<" coda:"<<getQueue()<<" host:"<<_host<<" feasibility: ";//<<_feasibility;
			
    			for(unsigned int i=0;i<_statiNodi.size();i++)
			{
				s<<"[";
				for(unsigned int j=0;j<_statiNodi[i].size();j++)
				{
					s<<_statiNodi[i][j];
				}
				s<<"]"<<_feasible[i]<<", ";
			}
			
			if(getTerminationStatus()==1)
			{		
				setTerminationStatus(2);
			}
			//s<<std::endl;
			return s.str();
		}
		bool isReservationJob()
		{
			if(isReservation())
			{
				unsigned int pos=-1;
				if((pos=getJobId().find(getQueue()))<getJobId().size())
				{
					return false;
				}
				else
				{
					return true;
				}
			}
			return false;
		}
		
		std::string printNodes(NodeArray nodes,std::vector<Job> * jobsArray)
		{	
			std::stringstream cmd;
			std::vector<int> ud=getUsedNodes();
			bool giaInserito=false;
			int n=0;
			for(unsigned int j=0;j<ud.size();j++)
			{
				while(ud[j]>0)
				{
					if(giaInserito)
						cmd<<"+";
					giaInserito=true;
					cmd<<nodes[j].getNodeName()<<":ncpus="<<getNumberOfCores()/getNumberOfNodes()<<":ngpus="<<getNumberOfGPU()/getNumberOfNodes()<<":mem="<<getMemory()/getNumberOfNodes();
					n++;
					ud[j]--;
					if(getNumberOfMIC()/getNumberOfNodes()==2)
					{
						cmd<<"+"<<nodes[j].getNodeName()<<"-mic0:nmics=1"<<"+"<<nodes[j].getNodeName()<<"-mic1:nmics=1";
						setUsedMIC(j,0,true);
						setUsedMIC(j,1,true);
						//std::cout<<getJobId()<<" Utilizza Mic 0 nodo "<<j<<std::endl;
						//std::cout<<getJobId()<<" Utilizza Mic 1 nodo "<<j<<std::endl;
					}
					else if(getNumberOfMIC()/getNumberOfNodes()==1)
					{
						bool mic0Usabile=true, mic1Usabile=true;
						for(unsigned int i=0;i<jobsArray->size();i++)
						{
							if((*jobsArray)[i].isScheduled() && (*jobsArray)[i].getNumberOfMIC()/(*jobsArray)[i].getNumberOfNodes()==1 &&  (*jobsArray)[i].getUsedNodes()[j]>0)
							{
								if((*jobsArray)[i].getUsedMIC(j,0)==true)
								{	
									mic0Usabile=false;
									
								}
								if((*jobsArray)[i].getUsedMIC(j,1)==true)
								{
									mic1Usabile=false;
								}
							}
						}
						int freemic=-1;
						
						if(mic1Usabile)
							freemic=1;
						if(mic0Usabile)
							freemic=0;
						if(freemic==-1)
						{
							std::cout<<"Errore, mic inutilizzabili"<<std::endl;
							//erroreMic=true;
							//getchar();
						}
						setUsedMIC(j,freemic,true);
						cmd<<"+"<<nodes[j].getNodeName()<<"-mic"<<freemic<<":nmics=1";
						//std::cout<<getJobId()<<" Utilizza Mic "<<freemic<<" nodo "<<j<<std::endl;
					}		
				}
			}
			return cmd.str();
		}
		
		bool changedLocation(std::string command)
		{
			if(_lastConfirmResvString.compare(command)==0)
				return false;
			return true;
		}
		 
		int execute(NodeArray nodes,std::vector<Job> * jobsArray, int initialTime,int istant)
		{	
			int eseguito=0;
			setQrunError(false);
			int erroreMic=false;
			if(isReservation() && isUsed() && !isReservationJob() && !isScheduled() &&!isShortConfirmed() && (!isConfirmedReservation() || ( istant > (getStartTime()-3600))))
			{
				char hostname[1024];
				eseguito=1;
				gethostname(hostname,1024);
				std::stringstream cmd;
				cmd<<"$PBS_EXEC/bin/pbs_rconf --server "; 
				cmd<<hostname<<" --resv-name "<<getJobName()<<" --start-time 0 --resources " ;
				std::vector<int> starts;
				std::vector<std::string> strNodes;
				if(isStandingReservation())
				{				
					std::vector<std::string> strs=Util::split(getJobId(),"/");
					for(unsigned int i=0;i<jobsArray->size();i++)
					{
						unsigned int pos;
						if((pos=(*jobsArray)[i].getJobId().find(strs[0]))<(*jobsArray)[i].getJobId().size())
						{
							bool trovato=false;
							for(unsigned int j=0;j<starts.size();j++)
							{
								if(starts[j]==(*jobsArray)[i].getStartTime())
								{

									trovato=true;
									std::stringstream temporanea;
									temporanea<<strNodes[j];
									if(strNodes[j].size()>0)
									{
										temporanea<<"+";
									}
									temporanea<<(*jobsArray)[i].printNodes(nodes,jobsArray);
									strNodes[j]=temporanea.str();
								}
							}
							if(!trovato)
							{
								strNodes.push_back( (*jobsArray)[i].printNodes(nodes,jobsArray));
								starts.push_back((*jobsArray)[i].getStartTime());
							}
							(*jobsArray)[i].setShortConfirmed(true);
						}
					}
					cmd<<"'"<<starts.size()<<"#";
					for(unsigned int j=0;j<starts.size();j++)
					{

						//std::cout<<getJobId()<<" "<<starts[j]<<" "<<strNodes[j]<<std::endl;
						cmd<<"(";
						cmd<<strNodes[j];
						cmd<<"){"<<j<<"}";
					}
					cmd<<"'";
				}
				else
				{
					std::vector<std::string> nome=Util::split(getJobId(),"(");
					std::stringstream altriNodi;
					if(nome.size()>1)
					{
						for(unsigned int i=0;i<jobsArray->size();i++)
						{
							unsigned int pos=20000;
							if((pos=(*jobsArray)[i].getJobId().find(nome[0]))<(*jobsArray)[i].getJobId().size() && getJobId().compare((*jobsArray)[i].getJobId())!=0)
							{
								(*jobsArray)[i].setConfirmedReservation(true);
								int n=0;
								std::vector<int> tmp=(*jobsArray)[i].getUsedNodes();
								for(unsigned int j=0;j<tmp.size();j++)
								{
									while(tmp[j]>0)
									{
										/*if(n>0)
											altriNodi<<"+";*/
										(*jobsArray)[i].setShortConfirmed(true);
										altriNodi<< "+"<<nodes[j].getNodeName()<<":ncpus="<<(*jobsArray)[i].getNumberOfCores()/(*jobsArray)[i].getNumberOfNodes()<<":ngpus="<<(*jobsArray)[i].getNumberOfGPU()/(*jobsArray)[i].getNumberOfNodes()<<":mem="<<(*jobsArray)[i].getMemory()/(*jobsArray)[i].getNumberOfNodes()<<"kb";
										n++;	
										tmp[j]--;
										if((*jobsArray)[i].getNumberOfMIC()/(*jobsArray)[i].getNumberOfNodes()==2)
										{
											altriNodi<<"+"<<nodes[j].getNodeName()<<"-mic0:nmics=1"<<"+"<<nodes[j].getNodeName()<<"-mic1:nmics=1";
											(*jobsArray)[i].setUsedMIC(j,0,true);
											(*jobsArray)[i].setUsedMIC(j,1,true);
											//std::cout<<(*jobsArray)[i].getJobId()<<" Utilizza Mic 0 nodo "<<j<<std::endl;
											//std::cout<<(*jobsArray)[i].getJobId()<<" Utilizza Mic 1 nodo "<<j<<std::endl;
											
										}
										else if((*jobsArray)[i].getNumberOfMIC()/(*jobsArray)[i].getNumberOfNodes()==1)
										{
											bool mic0Usabile=true, mic1Usabile=true;
											for(unsigned int k=0;k<jobsArray->size();k++)
											{
												if((*jobsArray)[k].isScheduled() && (*jobsArray)[k].getNumberOfMIC()/(*jobsArray)[k].getNumberOfNodes()==1 &&  (*jobsArray)[k].getUsedNodes()[j]>0)
												{
													if((*jobsArray)[k].getUsedMIC(j,0)==true)
													{	
														mic0Usabile=false;
														
														//exec_str<<"+"<<nodes[j].getNodeName()<<"-mic0:nmics=1";
													}
													if((*jobsArray)[k].getUsedMIC(j,1)==true)
													{
														mic1Usabile=false;
														//setUsedMIC(j,1,true);
														//exec_str<<"+"<<nodes[j].getNodeName()<<"-mic1:nmics=1";
													}
												}
											}
											int freemic=-1;
											
											if(mic1Usabile)
												freemic=1;
											if(mic0Usabile)
												freemic=0;
											if(freemic==-1)
											{
												std::cout<<"Errore, mic inutilizzabili"<<std::endl;
												erroreMic=true;
												//getchar();
											}
											(*jobsArray)[i].setUsedMIC(j,freemic,true);
											altriNodi<<"+"<<nodes[j].getNodeName()<<"-mic"<<freemic<<":nmics=1";
											//std::cout<<(*jobsArray)[i].getJobId()<<" Utilizza Mic "<<freemic<<" nodo "<<j<<std::endl;
										}
									}
								}
								//altriNodi<<"+";
							}
						}
					}
					std::vector<int> ud=getUsedNodes();
					bool giaInserito=false;
					//cmd<<altriNodi.str();
					setConfirmedReservation(true);
					int n=0;
					for(unsigned int j=0;j<ud.size();j++)
					{
						while(ud[j]>0)
						{
							if(giaInserito)
								cmd<<"+";
							giaInserito=true;
							cmd<<nodes[j].getNodeName()<<":ncpus="<<getNumberOfCores()/getNumberOfNodes()<<":ngpus="<<getNumberOfGPU()/getNumberOfNodes()<<":mem="<<getMemory()/getNumberOfNodes();
							n++;
							ud[j]--;
							if(getNumberOfMIC()/getNumberOfNodes()==2)
							{
								cmd<<"+"<<nodes[j].getNodeName()<<"-mic0:nmics=1"<<"+"<<nodes[j].getNodeName()<<"-mic1:nmics=1";
								setUsedMIC(j,0,true);
								setUsedMIC(j,1,true);
								//std::cout<<getJobId()<<" Utilizza Mic 0 nodo "<<j<<std::endl;
								//std::cout<<getJobId()<<" Utilizza Mic 1 nodo "<<j<<std::endl;
							}
							else if(getNumberOfMIC()/getNumberOfNodes()==1)
							{
								bool mic0Usabile=true, mic1Usabile=true;
								for(unsigned int i=0;i<jobsArray->size();i++)
								{
									if((*jobsArray)[i].isScheduled() && (*jobsArray)[i].getNumberOfMIC()/(*jobsArray)[i].getNumberOfNodes()==1 &&  (*jobsArray)[i].getUsedNodes()[j]>0)
									{
										if((*jobsArray)[i].getUsedMIC(j,0)==true)
										{	
											mic0Usabile=false;
											
											//exec_str<<"+"<<nodes[j].getNodeName()<<"-mic0:nmics=1";
										}
										else if((*jobsArray)[i].getUsedMIC(j,1)==true)
										{
											mic1Usabile=false;
											//setUsedMIC(j,1,true);
											//exec_str<<"+"<<nodes[j].getNodeName()<<"-mic1:nmics=1";
										}
									}
								}
								int freemic=-1;
								
								if(mic1Usabile)
									freemic=1;
								if(mic0Usabile)
									freemic=0;
								if(freemic==-1)
								{
									std::cout<<"Errore, mic inutilizzabili"<<std::endl;
									erroreMic=true;
									//getchar();
								}
								setUsedMIC(j,freemic,true);
								cmd<<"+"<<nodes[j].getNodeName()<<"-mic"<<freemic<<":nmics=1";
								//std::cout<<getJobId()<<" Utilizza Mic "<<freemic<<" nodo "<<j<<std::endl;
							}		
						}
					}	
					cmd<<altriNodi.str();
				}			
				std::cout<<cmd.str()<<std::endl;
				if(changedLocation(cmd.str()))
				{				
					_lastConfirmResvString=cmd.str();
					std::thread t1(&Job::runSystem, this,cmd.str());
					t1.detach();
				}
				//system(cmd.str().c_str());
				//getchar();
			}
			//std::cout<<"Esecuzione "<<isUsed()<<" "<<getStartTime()<<" =?= "<<istant<<" !"<<isScheduled()<<std::endl;
			//getchar();
			if(isUsed() && getStartTime() == istant && !isScheduled())
			{
				if(! isReservation() || isReservationJob())
				{
					std::vector<std::string> nome=Util::split(getJobId(),"(");
					std::stringstream altriNodi;
					if(nome.size()>1)
					{
						for(unsigned int i=0;i<jobsArray->size();i++)
						{
							unsigned int pos=20000;
							if((pos=(*jobsArray)[i].getJobId().find(nome[0]))<(*jobsArray)[i].getJobId().size() && getJobId().compare((*jobsArray)[i].getJobId())!=0)
							{
								(*jobsArray)[i].setScheduled(true);
								int n=0;
								std::vector<int> tmp=(*jobsArray)[i].getUsedNodes();
								for(unsigned int j=0;j<tmp.size();j++)
								{
									while(tmp[j]>0)
									{
										if(n>0)
											altriNodi<<"+";
										altriNodi<< nodes[j].getNodeName()<<":ncpus="<<(*jobsArray)[i].getNumberOfCores()/(*jobsArray)[i].getNumberOfNodes()<<":ngpus="<<(*jobsArray)[i].getNumberOfGPU()/(*jobsArray)[i].getNumberOfNodes()<<":mem="<<(*jobsArray)[i].getMemory()/(*jobsArray)[i].getNumberOfNodes()<<"kb";
										n++;	
										tmp[j]--;
										if(isReservationJob() && (*jobsArray)[i].getNumberOfMIC()/(*jobsArray)[i].getNumberOfNodes()==2)
										{
											altriNodi<<"+"<<nodes[j].getNodeName()<<"-mic0:nmics=1"<<"+"<<nodes[j].getNodeName()<<"-mic1:nmics=1";
											(*jobsArray)[i].setResvJobUsedMIC(j,0,true);
											(*jobsArray)[i].setResvJobUsedMIC(j,1,true);
											//std::cout<<(*jobsArray)[i].getJobId()<<" Utilizza Mic 0 nodo "<<j<<std::endl;
											//std::cout<<(*jobsArray)[i].getJobId()<<" Utilizza Mic 1 nodo "<<j<<std::endl;
											
										}
										else if(isReservationJob() && (*jobsArray)[i].getNumberOfMIC()/(*jobsArray)[i].getNumberOfNodes()==1)
										{
											bool mic0Usabile=true, mic1Usabile=true;
											for(unsigned int k=0;k<jobsArray->size();k++)
											{
												if((*jobsArray)[k].isScheduled() && (*jobsArray)[k].getNumberOfMIC()/(*jobsArray)[k].getNumberOfNodes()==1 &&  (*jobsArray)[k].getUsedNodes()[j]>0 && !(*jobsArray)[k].isReservation())
												{
													if((*jobsArray)[k].getUsedMIC(j,0)==true)
													{	
														//std::cout<<(*jobsArray)[i].getJobId()<<" Mic 0 nodo "<<j<<" utilizzato da "<<(*jobsArray)[k].getJobId()<<std::endl;
														mic0Usabile=false;
													}
													if((*jobsArray)[k].getUsedMIC(j,1)==true)
													{
														//std::cout<<(*jobsArray)[i].getJobId()<<" Mic 1 nodo "<<j<<" utilizzato da "<<(*jobsArray)[k].getJobId()<<std::endl;
														mic1Usabile=false;
													}
												}
												if((*jobsArray)[k].isScheduled() && (*jobsArray)[k].isReservation()&& (*jobsArray)[k].isScheduled() && !(*jobsArray)[k].isReservationJob()  && (*jobsArray)[k].getQueue().compare((*jobsArray)[i].getQueue())==0)
												{
													if((*jobsArray)[k].getUsedMIC(j,0)==true)
													{	
														
														//std::cout<<(*jobsArray)[i].getJobId()<<" Mic 0 nodo "<<j<<" utilizzato da Reservation "<<(*jobsArray)[k].getJobId()<<std::endl;
														mic0Usabile=true;
													}
													if((*jobsArray)[k].getUsedMIC(j,1)==true)
													{
														//std::cout<<(*jobsArray)[i].getJobId()<<" Mic 1 nodo "<<j<<" utilizzato da Reservation "<<(*jobsArray)[k].getJobId()<<std::endl;
														mic1Usabile=true;
													}
												}
												if((*jobsArray)[k].isScheduled() && (*jobsArray)[k].isReservationJob()  && (*jobsArray)[k].getQueue().compare((*jobsArray)[i].getQueue())==0)
												{
													if((*jobsArray)[k].getResvJobUsedMIC(j,0)==true)
													{	
														//std::cout<<(*jobsArray)[i].getJobId()<<" Mic 0 nodo "<<j<<" utilizzato da ReservationJob "<<(*jobsArray)[k].getJobId()<<std::endl;
														mic0Usabile=false;
													}
													if((*jobsArray)[k].getResvJobUsedMIC(j,1)==true)
													{
														//std::cout<<(*jobsArray)[i].getJobId()<<" Mic 1 nodo "<<j<<" utilizzato da ReservationJob "<<(*jobsArray)[k].getJobId()<<std::endl;
														mic1Usabile=false;
													}
												}
											}
											int freemic=-1;
											
											if(mic1Usabile)
												freemic=1;
											if(mic0Usabile)
												freemic=0;
											if(freemic==-1)
											{
												std::cout<<"Errore, mic inutilizzabili "<<toString(0,0)<<std::endl;
												//getchar();
											}
											(*jobsArray)[i].setResvJobUsedMIC(j,freemic,true);
											altriNodi<<"+"<<nodes[j].getNodeName()<<"-mic"<<freemic<<":nmics=1";
											//std::cout<<(*jobsArray)[i].getJobId()<<" Utilizza Mic "<<freemic<<" nodo "<<j<<std::endl;

										}
										else if((*jobsArray)[i].getNumberOfMIC()/(*jobsArray)[i].getNumberOfNodes()==2)
										{
											altriNodi<<"+"<<nodes[j].getNodeName()<<"-mic0:nmics=1"<<"+"<<nodes[j].getNodeName()<<"-mic1:nmics=1";
											(*jobsArray)[i].setUsedMIC(j,0,true);
											(*jobsArray)[i].setUsedMIC(j,1,true);
										}
										else if((*jobsArray)[i].getNumberOfMIC()/(*jobsArray)[i].getNumberOfNodes()==1)
										{
											bool mic0Usabile=true, mic1Usabile=true;
											for(unsigned int k=0;k<jobsArray->size();k++)
											{
												if((*jobsArray)[k].isScheduled() && (*jobsArray)[k].getNumberOfMIC()/(*jobsArray)[k].getNumberOfNodes()==1 &&  (*jobsArray)[k].getUsedNodes()[j]>0)
												{
													if((*jobsArray)[k].getUsedMIC(j,0)==true)
													{	
														mic0Usabile=false;
														
														//exec_str<<"+"<<nodes[j].getNodeName()<<"-mic0:nmics=1";
													}
													if((*jobsArray)[k].getUsedMIC(j,1)==true)
													{
														mic1Usabile=false;
														//setUsedMIC(j,1,true);
														//exec_str<<"+"<<nodes[j].getNodeName()<<"-mic1:nmics=1";
													}
												}
											}
											int freemic=-1;
											
											if(mic1Usabile)
												freemic=1;
											if(mic0Usabile)
												freemic=0;
											if(freemic==-1)
											{
												std::cout<<"Errore, mic inutilizzabili"<<std::endl;
												erroreMic=true;
												//getchar();
											}
											(*jobsArray)[i].setUsedMIC(j,freemic,true);
											altriNodi<<"+"<<nodes[j].getNodeName()<<"-mic"<<freemic<<":nmics=1";
										}
									}
								}
								altriNodi<<"+";
							}
						}
					}
					
					setScheduled(true);
					eseguito=1;
					std::stringstream exec_str;
					exec_str<<"$PBS_EXEC/bin/qrun -H "<<altriNodi.str();	
					int n=0;
					std::vector<int> tmp=getUsedNodes();
					for(unsigned int j=0;j<tmp.size();j++)
					{
						while(tmp[j]>0)
						{
							if(n>0)
								exec_str <<"+";
							exec_str  << nodes[j].getNodeName()<<":ncpus="<<getNumberOfCores()/getNumberOfNodes()<<":ngpus="<<getNumberOfGPU()/getNumberOfNodes()<<":mem="<<getMemory()/getNumberOfNodes()<<"kb";
							n++;	
							tmp[j]--;
							if(isReservationJob() && getNumberOfMIC()/getNumberOfNodes()==2)
							{
								exec_str<<"+"<<nodes[j].getNodeName()<<"-mic0:nmics=1"<<"+"<<nodes[j].getNodeName()<<"-mic1:nmics=1";									
								setResvJobUsedMIC(j,0,true);
								setResvJobUsedMIC(j,1,true);
								//std::cout<<getJobId()<<" RJ Utilizza Mic 0 nodo "<<j<<std::endl;
								//std::cout<<getJobId()<<" RJ Utilizza Mic 1 nodo "<<j<<std::endl;
								
							}
							else if(isReservationJob() && getNumberOfMIC()/getNumberOfNodes()==1)
							{
								bool mic0Usabile=true, mic1Usabile=true;
								for(unsigned int k=0;k<jobsArray->size();k++)
								{
									if((*jobsArray)[k].isScheduled() && (*jobsArray)[k].getNumberOfMIC()/(*jobsArray)[k].getNumberOfNodes()==1 &&  (*jobsArray)[k].getUsedNodes()[j]>0 && !(*jobsArray)[k].isReservation())
									{
										if((*jobsArray)[k].getUsedMIC(j,0)==true)
										{	
											//std::cout<<getJobId()<<" Mic 0 nodo "<<j<<"  utilizzato da  "<<(*jobsArray)[k].getJobId()<<std::endl;
											mic0Usabile=false;
										}
										if((*jobsArray)[k].getUsedMIC(j,1)==true)
										{
											mic1Usabile=false;
											//std::cout<<getJobId()<<" Mic 1 nodo "<<j<<"  utilizzato da  "<<(*jobsArray)[k].getJobId()<<std::endl;
										}
									}
									if((*jobsArray)[k].isScheduled() && (*jobsArray)[k].isReservation() && !(*jobsArray)[k].isReservationJob()  && (*jobsArray)[k].getQueue().compare(getQueue())==0)
									{
										if((*jobsArray)[k].getUsedMIC(j,0)==true)
										{	
											
											//std::cout<<getJobId()<<" Mic 0 nodo "<<j<<" utilizzato da Reservation "<<(*jobsArray)[k].getJobId()<<std::endl;
											mic0Usabile=true;
										}
										if((*jobsArray)[k].getUsedMIC(j,1)==true)
										{
											//std::cout<<getJobId()<<" Mic 1 nodo "<<j<<" utilizzato da Reservation "<<(*jobsArray)[k].getJobId()<<std::endl;
											mic1Usabile=true;
										}
									}
									if((*jobsArray)[k].isScheduled() && (*jobsArray)[k].isReservationJob()  && (*jobsArray)[k].getQueue().compare(getQueue())==0)
									{
										if((*jobsArray)[k].getResvJobUsedMIC(j,0)==true)
										{	
											//std::cout<<getJobId()<<" Mic 0 nodo "<<j<<"  utilizzato da ReservationJob "<<(*jobsArray)[k].getJobId()<<std::endl;
											mic0Usabile=false;
										}
										if((*jobsArray)[k].getResvJobUsedMIC(j,1)==true)
										{
											//std::cout<<getJobId()<<" Mic 1 nodo "<<j<<"  utilizzato da ReservationJob "<<(*jobsArray)[k].getJobId()<<std::endl;
											mic1Usabile=false;
										}
									}
								}
								int freemic=-1;
									
								if(mic1Usabile)
									freemic=1;
								if(mic0Usabile)
									freemic=0;
								if(freemic==-1)
								{
									std::cout<<"Errore, mic inutilizzabili"<<std::endl;
									erroreMic=true;
									//getchar();
								}
								setResvJobUsedMIC(j,freemic,true);
								exec_str<<"+"<<nodes[j].getNodeName()<<"-mic"<<freemic<<":nmics=1";
								//std::cout<<getJobId()<<" RJ Utilizza Mic"<<freemic<<" nodo "<<j<<std::endl;
							}
							else if(getNumberOfMIC()/getNumberOfNodes()==2)
							{
								exec_str<<"+"<<nodes[j].getNodeName()<<"-mic0:nmics=1"<<"+"<<nodes[j].getNodeName()<<"-mic1:nmics=1";
								setUsedMIC(j,0,true);
								setUsedMIC(j,1,true);
							}
							else if(getNumberOfMIC()/getNumberOfNodes()==1)
							{
								bool mic0Usabile=true, mic1Usabile=true;
								for(unsigned int i=0;i<jobsArray->size();i++)
								{
									if((*jobsArray)[i].isScheduled() && (*jobsArray)[i].getNumberOfMIC()/(*jobsArray)[i].getNumberOfNodes()==1 &&  (*jobsArray)[i].getUsedNodes()[j]>0)
									{
										if((*jobsArray)[i].getUsedMIC(j,0)==true)
										{	
											mic0Usabile=false;
											
											//exec_str<<"+"<<nodes[j].getNodeName()<<"-mic0:nmics=1";
										}
										if((*jobsArray)[i].getUsedMIC(j,1)==true)
										{
											mic1Usabile=false;
											//setUsedMIC(j,1,true);
											//exec_str<<"+"<<nodes[j].getNodeName()<<"-mic1:nmics=1";
										}
									}
								}
								int freemic=-1;
								
								if(mic1Usabile)
									freemic=1;
								if(mic0Usabile)
									freemic=0;
								if(freemic==-1)
								{
									std::cout<<"Errore, mic inutilizzabili"<<std::endl;
									erroreMic=true;
									//getchar();
								}
								setUsedMIC(j,freemic,true);
								exec_str<<"+"<<nodes[j].getNodeName()<<"-mic"<<freemic<<":nmics=1";
							}
						}
					}
					
					exec_str <<" "<<nome[0]<<" 2>&1 >/dev/null";
					std::cout<<"Executing -"<<exec_str.str()<<"- now: "<<getStartTime()<<" == "<< Util::timeToStr(istant)<<std::endl;
					//system(exec_str.str().c_str());
					FILE * fp=popen(exec_str.str().c_str(),"r");
					char buff[1024];
					while(fgets(buff,1024,fp))
					{
						std::string s(buff);
						std::cout<<"ERRORE -->"<<s<<std::endl;
						setScheduled(false);
						for(unsigned int i=0;i<getUsedNodes().size();i++)
						{
							setUsedMIC(i,0,false);
							setUsedMIC(i,1,false);
							setQrunError(true);
						}
						//getchar();
					}
					pclose(fp);
				}
			
				
			}
			if(isUsed() && isReservation() && getStartTime() <= istant && !isScheduled() && !isReservationJob())
			{
				setScheduled(true);
				eseguito=1;
				/*std::stringstream command;
				command<<"$PBS_EXEC/bin/qmgr -c \"set queue "<<getQueue()<<" started = true\"";
				std::cout<<"Starting reservation"<<command.str()<<std::endl;
				std::thread t1(&Job::runSystem, this,command.str());
				t1.detach();*/
				//system(command.str().c_str());
			}
			if(erroreMic)
			{
				setScheduled(false);
				for(unsigned int i=0;i<getUsedNodes().size();i++)
				{
					setUsedMIC(i,0,false);
					setUsedMIC(i,1,false);
					setQrunError(true);
				}
			}
			/*else if (isReservation() && getStartTime() <= istant && getFeasibility(nodes.getStates())!=1 )
			{
				std::stringstream cmd;
				eseguito=1;
				cmd<<"$PBS_EXEC/bin/pbs_rdel ";
				cmd<<getQueue();
				system(cmd.str().c_str());
				std::cout<<cmd.str()<<std::endl;
			}*/
			return eseguito;
		}
		
		
	};
#endif
