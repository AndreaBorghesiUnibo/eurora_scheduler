/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                 File: JobReader.hpp                                                  *
*                                                                                                                      *
*                                                                                                                      *
 ***********************************************************************************************************************/

#ifndef Def_JobReader
#define Def_JobReader
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "Job.hpp"
#include "Util.hpp"
#include "Model26Util.hpp"
#include <sstream>
#include <climits>

class JobReader
{
private:
	std::string _file;
	
public:
	JobReader(){}
	JobReader(std::string file);
	std::vector<Job> read();
	std::vector<Job> readFromPBS(std::vector<Job> old,NodeArray nodes);
	std::vector<Job> readJob(std::string name,NodeArray nodesArray);
	std::vector<Job> readReservations(std::vector<Job> old,NodeArray nodesArray);
	std::vector<Job> readReservation(std::string id,NodeArray nodesArray);
};

inline std::vector<Job> JobReader::readReservation(std::string id,NodeArray nodesArray)
{
	//std::cout<<"StartRead res "<<id<<std::endl;
	std::stringstream job_cmd;
	job_cmd<<"$PBS_EXEC/bin/pbs_rstat  -f "<<id<<" &";
	//std::cout<<"Before "<<id<<std::endl;
	FILE * fp=popen(job_cmd.str().c_str(),"r");
	//std::cout<<"After "<<id<<std::endl;
	if(!fp)
	{
		std::cout<<"cant read command output readJob qstat "<<job_cmd.str()<<std::endl;
		//exit(-1);
		//getchar();
	}
	char buff[1024];
	std::string nome,coda,user,host="",exec_vnodePrec;
	int holding =0;
	std::stringstream prec;
	int eqt=1,nodes=1,cores=1,gpu=0,mic=0,mem=0,estDur=60,realDur=-1,start=INT_MAX,ripetizioni=-1,passate=1,ghz=-1;
	unsigned int pos=0;
	char freq=' ';
	bool confermata=true;
	std::vector<std::string> giorni;
	std::vector<int> u(Model26Util::getNumberOfNodes(),0);
	bool running=false;
	int stato=0;
	bool eterogenee=false;
	std::vector<Job> ja;
	while(fgets(buff,1024,fp))
	{
		std::string s(buff);
		if(stato==1 && s[0]=='\t' && s.size()>2)
		{
			s.erase(s.end()-1);
			s.erase(s.begin());
			prec<<s;
			//std::cout<<"0--->"<<prec.str()<<"<---"<<std::endl;
	
			
		}
		else if(stato==1)
		{
			//std::cout<<"1FINITO --->"<<prec.str()<<"<---"<<std::endl;
			stato=0;
			std::vector<std::string> vettSelect=Util::split(prec.str(), "+");
			if(vettSelect.size()>1)
			{
				eterogenee=true;
				for(unsigned int i=0;i<vettSelect.size();i++)
				{
					nodes=1,cores=1,gpu=0,mic=0,mem=0,ghz=-1,host="";
					nodes=atoi(vettSelect[i].c_str());
					if((pos=vettSelect[i].find("host="))<vettSelect[i].size())
					{
						std::string sel=vettSelect[i];
						sel.erase(0,sizeof(char)*(strlen("host=")+pos));
						host=sel;
					}
					if((pos=vettSelect[i].find("ncpus="))<vettSelect[i].size())
					{
						std::string sel=vettSelect[i];
						sel.erase(0,sizeof(char)*(strlen("ncpus=")+pos));
						cores=atoi(sel.c_str());
					}
					if((pos=vettSelect[i].find("ngpus="))<vettSelect[i].size())
					{
						std::string sel=vettSelect[i];
						sel.erase(0,sizeof(char)*(strlen("ngpus=")+pos));
						gpu=atoi(sel.c_str());
					}
					if((pos=vettSelect[i].find("nmics="))<vettSelect[i].size())
					{
						std::string sel=vettSelect[i];
						sel.erase(0,sizeof(char)*(strlen("nmics=")+pos));
						mic=atoi(sel.c_str());
					}
					if((pos=vettSelect[i].find("mem="))<vettSelect[i].size())
					{
						std::string sel=vettSelect[i];
						sel.erase(0,sizeof(char)*(strlen("mem=")+pos));
						int mul=1;
						if((pos=sel.find("kb"))<sel.size() || (pos=sel.find("Kb"))<sel.size() || (pos=sel.find("KB"))<sel.size() )
						{
							mul=1;
						}
						else if((pos=sel.find("mb"))<sel.size() || (pos=sel.find("Mb"))<sel.size() || (pos=sel.find("MB"))<sel.size() )
						{
							mul=1024;
						}
						else if((pos=sel.find("gb"))<sel.size() || (pos=sel.find("Gb"))<sel.size() || (pos=sel.find("GB"))<sel.size() )
						{
							mul=1024*1024;
						}
						mem=atoi(sel.c_str())*mul;
					}
					if((pos=vettSelect[i].find("cpuspeed="))<vettSelect[i].size())
					{
						std::string sel=vettSelect[i];
						sel.erase(0,sizeof(char)*(strlen("cpuspeed=")+pos));
						if(sel.compare("any")==0)
							ghz=-1;
						else
							ghz=atoi(sel.c_str());
					}
					std::stringstream newId;
					newId<<id<<"("<<i<<")";
					Job j(newId.str(),nome,user,coda,eqt,nodes,cores*nodes,gpu*nodes,mic*nodes,mem*nodes,ghz,estDur,realDur,start,u,host);
					//j.initFeasibility(feasible);
					j.setHolding(holding);
					ja.push_back(j);
				}
				//creo array di jobs
			}
			else
			{
				//std::cout<<"2 else"<<std::endl;
				if((pos=prec.str().find("host="))<prec.str().size())
				{
					std::string sel=prec.str();
					sel.erase(0,sizeof(char)*(strlen("host=")+pos));
					host=sel;
					//std::cout<<"HOST index="<<host<<std::endl;
					
					//getchar();	

				}
				if((pos=prec.str().find("cpuspeed="))<prec.str().size())
				{
					std::string sel=prec.str();
					sel.erase(0,sizeof(char)*(strlen("cpuspeed=")+pos));
					if(sel.compare("any")==0)
						ghz=-1;
					else
						ghz=atoi(sel.c_str());
					//std::cout<<"ghz=ghz "<<ghz<<std::endl;
				}
				//getchar();
				Job j(id,nome,user,coda,eqt,nodes,cores,gpu,mic,mem,ghz,estDur,realDur,start,u,host);
				//j.initFeasibility(feasible);
				j.setHolding(holding);
				ja.push_back(j);
			}		
			Util::trim(s);	
			if(s.length()>1)	
				s.erase(s.end()-1);
			//std::cout<<"1--"<<s<<"--"<<std::endl;
		}
		else
		{
				//std::cout<<"3 else"<<std::endl;
			Util::trim(s);	
			if(s.length()>1)	
				s.erase(s.end()-1);
			//std::cout<<"2--"<<s<<"--"<<std::endl;
		}
		
		//std::cout<<"4 read"<<std::endl;
		if((pos=s.find("Name: "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Name: "));
			nome=s;
		}
		else if((pos=s.find("reserve_state = "))<s.size())
		{
			//std::cout<<"--"<<s<<"--"<<std::endl;
			if((pos=s.find("RESV_UNCONFIRMED"))<s.size())
			{
				confermata=false;
				std::cout<<"Resv non confermata"<<std::endl;
			}
			if((pos=s.find("RESV_RUNNING"))<s.size())
			{
				running=true;
				std::cout<<"Resv RUNNING"<<std::endl;
			}
			//std::cout<<confermata<<std::endl;
			//getchar();
		}
		else if((pos=s.find("Reserve_Owner = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Reserve_Owner = "));
			user=s;
		}
		else if((pos=s.find("queue = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("queue = "));
			coda=s;
		}
		else if((pos=s.find("ctime = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("ctime = "));
			eqt=Util::str2ToTime(s.c_str());
		}
		else if((pos=s.find("Resource_List.nodect = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Resource_List.nodect = "));
			nodes=atoi(s.c_str());
		}
		else if((pos=s.find("reserve_index = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("reserve_index = "));
			passate=atoi(s.c_str());
		}
		else if((pos=s.find("Resource_List.ncpus = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Resource_List.ncpus = "));
			cores=atoi(s.c_str());
		}
		else if((pos=s.find("Resource_List.ngpus = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Resource_List.ngpus = "));
			gpu=atoi(s.c_str());
		}
		else if((pos=s.find("Resource_List.nmics = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Resource_List.nmics = "));
			mic=atoi(s.c_str());
		}
		else if((pos=s.find("Resource_List.cpuspeed = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Resource_List.cpuspeed = "));
			if(s.compare("any")==0)
				ghz=-1;
			else
				ghz=atoi(s.c_str());
		}
		else if((pos=s.find("Resource_List.mem = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Resource_List.mem = "));
			int mul=1;
			if((pos=s.find("kb"))<s.size() || (pos=s.find("Kb"))<s.size() || (pos=s.find("KB"))<s.size() )
			{
				mul=1;
			}
			else if((pos=s.find("mb"))<s.size() || (pos=s.find("Mb"))<s.size() || (pos=s.find("MB"))<s.size() )
			{
				mul=1024;
			}
			else if((pos=s.find("gb"))<s.size() || (pos=s.find("Gb"))<s.size() || (pos=s.find("GB"))<s.size() )
			{
				mul=1024*1024;
			}
			mem=atoi(s.c_str())*mul;
		}
		
		else if((pos=s.find("reserve_duration = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("reserve_duration = "));
			estDur = atoi(s.c_str());
			realDur= atoi(s.c_str());
		}
		else if((pos=s.find("reserve_start = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("reserve_start = "));
			start= Util::str2ToTime(s.c_str());
		}
		else if((pos=s.find("Resource_List.select = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Resource_List.select = "));
			stato=1;
			//std::cout<<"Stato = 1"<<std::endl;
			//std::cout<<"select=s "<<s<<std::endl;
			//getchar();
			prec<<s;
		}
		else if((pos=s.find("reserve_count = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("reserve_count = "));
			ripetizioni=atoi(s.c_str());
		}
		else if((pos=s.find("reserve_rrule = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("reserve_rrule = "));
			std::vector<std::string> tmp=Util::split( s, ";");
			for(unsigned int i=0;i<tmp.size();i++)
			{
				if((pos=tmp[i].find("FREQ="))<tmp[i].size())
				{
					tmp[i].erase(0,sizeof(char)*strlen("FREQ="));
					freq=tmp[i][0];
				}
				else if((pos=tmp[i].find("BYDAY="))<tmp[i].size())
				{
					tmp[i].erase(0,sizeof(char)*strlen("BYDAY="));
					giorni=Util::split(tmp[i],",");
				}
				else if((pos=tmp[i].find("COUNT="))<tmp[i].size() && ripetizioni==-1)
				{
					tmp[i].erase(0,sizeof(char)*strlen("COUNT="));
					ripetizioni=atoi(tmp[i].c_str());
				}
			}
		}
		else if((pos=s.find("resv_nodes = "))<s.size())
		{
			/*s.erase(0,sizeof(char)*strlen("resv_nodes = "));
			std::vector<std::string> vettNodi=Util::split( s, "(");
			if(vettNodi.size()==1)
			{
				vettNodi=Util::split(s, "+");
			}
			for(unsigned int i=0;i<vettNodi.size();i++)
			{
				for(int j=0;j<nodesArray.size();j++)
				{	
					if((pos=vettNodi[i].find(nodesArray[j].getNodeName()))<s.size())
					{
						u[j]++;
						std::cout<<"settato nodo reservation################################# "<<j<<std::endl;
					}
				}
			}*/
			s.erase(0,sizeof(char)*strlen("resv_nodes = "));
			std::stringstream tot;
			//std::cout<<"exec =s "<<tot<<std::endl;
			tot<<s;
			do
			{
				fgets(buff,1024,fp);
				std::string tmp(buff);
				Util::trim(tmp);
				if(tmp.size()>1)		
					tmp.erase(tmp.end()-1);
				
				//std::cout<<"tmp "<<tmp<<std::endl;
				if(tmp[0]==' ')
					tot<<tmp;
			}while(buff[0]==' ');
			//std::cout<<"Tot ->"<<tot.str()<<"<-"<<std::endl;
			exec_vnodePrec=tot.str();
			std::vector<std::string> vettNodi=Util::split(tot.str(), "(");
			if(vettNodi.size()==1 || vettNodi[0].empty())
			{
				std::string t=tot.str().substr(0,tot.str().size()-1);
				vettNodi=Util::split(t, "+");
			}
			//std::cout<<s<<std::endl<<"trovati num nodi "<<vettNodi.size()<<vettNodi[0]<<std::endl;
			//getchar();
			for(unsigned int i=0;i<vettNodi.size();i++)
			{
				if(vettNodi[i].find("-mic")>vettNodi[i].size())
				{
					//std::cout<<"_"<<vettNodi[i]<<"_"<<std::endl;
					for(int j=0;j<nodesArray.size();j++)
					{	
						if((pos=vettNodi[i].find(nodesArray[j].getNodeName()))<vettNodi[i].size() && !vettNodi[i].empty())
						{
							u[j]++;
						}
					}
				}
			}
			//std::cout<<"fine for"<<std::endl;
		}
	
	}
	pclose(fp);

	if(!eterogenee)
	{
		
		//std::cout<<"Letto !E "<<std::endl;
		for(unsigned int i=0;i<ja.size();i++)
		{
			ja[i].allocate(start,u);
			ja[i].setEstimatedDuration(estDur);
			ja[i].setRealDuration(realDur);
			ja[i].setEnterQueueTime(eqt);
			ja[i].setScheduled(running);
			ja[i].setConfirmedReservation(confermata);
		}
	}
	//else
	if(eterogenee)
	{
		if(!exec_vnodePrec.empty())
		{
			//std::cout<<"entro E "<<exec_vnodePrec<<std::endl;
			std::vector<std::string> vettNodi=Util::split(exec_vnodePrec, "(");
			if(vettNodi.size()==1 || vettNodi[0].empty())
			{
				int a=0;
				if(exec_vnodePrec[0]=='(')
					a++;
				std::string t=exec_vnodePrec.substr(a,exec_vnodePrec.size()-1);
				vettNodi=Util::split(t, "+");
			}
			/*if(vettNodi.size()==1)
			{
				vettNodi=Util::split(exec_vnodePrec, "+");
			}*/
			//std::cout<<"passed"<<vettNodi.size()<<vettNodi[0]<<std::endl;
			for(unsigned int i=0;i<vettNodi.size();i++)
			{
				if(vettNodi[i].size()>1)
					vettNodi[i].erase(vettNodi[i].find(":"),vettNodi[i].size());
			}
			//std::cout<<"ho "<<vettNodi.size()<<" da settare "<<exec_vnodePrec<<std::endl;
			unsigned int indiceJob=0;
			unsigned int indiceVettNodi=0;
			do //Può sembrare brutto codice ma, FIDATI, va fatto così 
			{
				std::vector<int> utilizzazione(nodesArray.size(),0);
				int countNodiInseriti=0;
				for(;indiceVettNodi<vettNodi.size() && countNodiInseriti<ja[indiceJob].getNumberOfNodes();indiceVettNodi++)
				{
					for(int j=0;j<nodesArray.size();j++)
					{
						//std::cout<<vettNodi[indiceVettNodi]<<" =?= "<<nodesArray[j].getNodeName();
						if(vettNodi[indiceVettNodi].compare(nodesArray[j].getNodeName())==0)
						{	
							countNodiInseriti++;
							utilizzazione[j]++;
							//std::cout<<" YES "<<countNodiInseriti<< " "<<utilizzazione[j];
						}
				//std::cout<<std::endl;
					}
				}
				ja[indiceJob].allocate(start,utilizzazione);
				/*for(unsigned int i=0;i<utilizzazione.size();i++)
				{
					std::cout<<utilizzazione[i];
				}
	
				std::cout<<std::endl;*/
				//getchar();				
				indiceJob++;
				if(indiceJob>=ja.size())
				{
					indiceJob=0;	
				}
			}while(indiceJob!=0);
		}
		for(unsigned int i=0;i<ja.size();i++)
		{
			ja[i].setEstimatedDuration(estDur);
			ja[i].setRealDuration(realDur);
			ja[i].setEnterQueueTime(eqt);
			ja[i].setScheduled(running);
			ja[i].setConfirmedReservation(confermata);
			std::cout<<"Letto eterogeneo "<<ja[i].toString(0,0)<<std::endl;
			//getchar();
		}	
	}
	std::vector<Job> ja2;
	//std::cout<<"Reservation letta:"<<id<<" "<<nome<<" "<<user<<" "<<coda<<" "<<eqt<<" "<<nodes<<" "<<cores<<" "<<gpu<<" "<<mic<<" "<<mem<<" "<<estDur<<" "<<realDur<<" "<<start<<std::endl;
	if(id[0]=='R')
	{
		/*Job j(id,nome,user,coda,eqt,nodes,cores,gpu,mic,mem,ghz,estDur,realDur,start,u,host);
		j.setConfirmedReservation(confermata);
		j.setScheduled(running);
		ja.push_back(j);*/
		for(unsigned int k=0;k<ja.size();k++)
		{
			std::cout<<"Letto "<<ja[k].toString(0,0)<<std::endl;		
		}
	//std::cout<<"FINISHRead res "<<id<<std::endl;
		return ja;
	}
	else if(id[0]=='S')
	{
		std::cout<<"Standing eservation letta freq:"<<ripetizioni<<" " <<passate<<std::endl;
		for(unsigned int k=0;k<ja.size();k++)
		{
			if(freq=='D')
			{
				for(int i=0;i<=ripetizioni-passate;i++)
				{
					std::stringstream newId;
					unsigned int pos=20000;
					//std::cout<<"A"<<std::endl;
					pos=ja[k].getJobId().find("(");
					if(pos>ja[k].getJobId().size())
					{ 
						newId<<ja[k].getJobId()<<"/"<<i;
					}
					else
					{
						newId<<ja[k].getJobId().substr(0,pos)<<"/"<<i<<ja[k].getJobId().substr(pos,ja[k].getJobId().size());
					}
					//std::cout<<"B"<<std::endl;
					//newId<<ja[k].getJobId()<<"/"<<i;
					std::cout<<"Reservation letta:"<<newId.str()<<" "<<nome<<" "<<user<<" "<<coda<<" "<<eqt<<" "<<nodes<<" "<<cores<<" "<<gpu<<" "<<mic<<" "<<mem<<" "<<estDur<<" "<<realDur<<" "<<start+i*86400<<std::endl;

					Job j(newId.str(),ja[k].getJobName(),ja[k].getUserName(),ja[k].getQueue(),ja[k].getEnterQueueTime(),ja[k].getNumberOfNodes(),ja[k].getNumberOfCores(),ja[k].getNumberOfGPU(),ja[k].getNumberOfMIC(),ja[k].getMemory(),ja[k].getGHz(),ja[k].getEstimatedDuration(),ja[k].getRealDuration(),ja[k].getStartTime()+i*86400,ja[k].getUsedNodes(),ja[k].getHost());
					j.setConfirmedReservation(confermata);
					if(i==0)
					{
						j.setScheduled(running);
					}
					else
					{
						j.setScheduled(false);
					}
					/*for(unsigned int i=0;i<u.size();i++)
					{
						if(u[i]>0)
							j.setScheduled(true);
					}*/
					ja2.push_back(j);
				}
			}
			else if(freq=='W')
			{
				//std::cout<<"Weeklyyy"<<std::endl;
				int count=0;
				for(int i=0;count<=ripetizioni-passate;i++)
				{
					
				//std::cout<<"ripetizione"<<count<<" "<<i<<std::endl;
					for(unsigned int j=0;j<giorni.size();j++)
					{
						
				//std::cout<<Util::timeToDayofWeek(start+i*86400)<<" ==? "<<giorni[j]<<std::endl;
						if((pos=Util::timeToDayofWeek(start+i*86400).find(giorni[j]))<giorni.size())
						{
							std::stringstream newId;
							pos=ja[k].getJobId().find("(");
							if(pos>ja[k].getJobId().size())
							{ 
								newId<<ja[k].getJobId()<<"/"<<i;
							}
							else
							{
								newId<<ja[k].getJobId().substr(0,pos)<<"/"<<i<<ja[k].getJobId().substr(pos,ja[k].getJobId().size());
							}
							//std::cout<<"Reservation letta:"<<newId.str()<<" "<<nome<<" "<<user<<" "<<coda<<" "<<eqt<<" "<<nodes<<" "<<cores<<" "<<gpu<<" "<<mic<<" "<<mem<<" "<<estDur<<" "<<realDur<<" "<<start+i*86400<<std::endl;
	
							Job j(newId.str(),ja[k].getJobName(),ja[k].getUserName(),ja[k].getQueue(),ja[k].getEnterQueueTime(),ja[k].getNumberOfNodes(),ja[k].getNumberOfCores(),ja[k].getNumberOfGPU(),ja[k].getNumberOfMIC(),ja[k].getMemory(),ja[k].getGHz(),ja[k].getEstimatedDuration(),ja[k].getRealDuration(),ja[k].getStartTime()+i*86400,ja[k].getUsedNodes(),ja[k].getHost());
							j.setConfirmedReservation(confermata);
							
							if(i==0)
							{
								j.setScheduled(running);
							}
							else
							{
								j.setScheduled(false);
							}
							ja2.push_back(j);
							count++;
						}
					}			
				}
			}
		}
	}
	for(unsigned int i=0;i<ja2.size();i++)
	{
		
		std::cout<<"Letto "<<ja2[i].toString(0,0)<<std::endl;
	}
	//std::cout<<"Return"<<std::endl;
	//std::cout<<"FINISHRead res "<<id<<std::endl;
	return ja2;
}
inline std::vector<Job> JobReader::readReservations(std::vector<Job> old,NodeArray nodesArray)
{
	std::stringstream exec_str;
	exec_str<<"$PBS_EXEC/bin/pbs_rstat";
	FILE * fp=popen(exec_str.str().c_str(),"r");
	if(!fp)
	{
		std::cout<<"cant read command output readReservations "<< exec_str.str()<<std::endl;
		//getchar();
	}
	std::vector<Job> ja;
	char buff[1024];
	while(fgets(buff,1024,fp))
	{	
		std::string s(buff);
		std::vector<std::string>tmp=Util::split(s,".");
		if(tmp[0][0]=='S' || tmp[0][0]=='R')
		{
			//WUWWW
			//std::cout<<"Letto id Reservation"<<tmp[0]<<std::endl;
			int index=-1;
			for(unsigned int i=0;i<old.size();i++)
			{
				unsigned int pos;
				if((pos=old[i].getJobId().find(tmp[0]))<old[i].getJobId().size() || (pos=tmp[0].find(old[i].getJobId()))<tmp[0].size())
				{
					//std::cout<<"found old resv "<<old[i].getJobId()<<" at index "<<i<<std::endl;
					index=i;
					ja.push_back(old[index]);
				}
			}
			if(index==-1)
			{
				std::vector<Job> r=readReservation(tmp[0],nodesArray);
				//std::cout<<r.size()<<std::endl;
				//getchar();
				for(unsigned int i=0;i<r.size();i++)
					if( !r[i].getJobId().empty())
					{
						ja.push_back(r[i]);
						
					}
				std::stringstream nomeFile;
				time_t today;
				time(&today);
				nomeFile<<"JobsLetti_"<<Util::dateToStrNoDel(today);
				std::ofstream output(nomeFile.str().c_str(), std::ofstream::app);
				if (!output) 
				{
					std::cout << "ERROR: Could not open file \"" << nomeFile.str() << "\"" << std::endl;
					getchar();
				}
				output<<tmp[0]<<std::endl;
				output.close();
			}		
		}		
	}
	pclose(fp);
	return ja;
}
inline JobReader::JobReader(std::string file)
{
	_file=file;
}

inline std::vector<Job> JobReader::readJob(std::string id,NodeArray nodesArray)
{
	std::stringstream job_cmd;
	std::vector<Job> ja;
	job_cmd<<"$PBS_EXEC/bin/qstat -x -f "<<id;
	FILE * fp=popen(job_cmd.str().c_str(),"r");
	if(!fp)
	{
		std::cout<<"cant read command output readJob qstat "<<job_cmd.str()<<std::endl;
		//exit(-1);
		//getchar();
	}
	char buff[1024];
	std::string nome,coda,user,host="",exec_vnodePrec;
	int holding =0;
	int eqt=1,nodes=1,cores=1,gpu=0,mic=0,mem=0,estDur=60,realDur=-1,start=INT_MAX,ghz=-1;
	unsigned int pos=0;
	std::vector<int> u(Model26Util::getNumberOfNodes(),0);
	std::vector<int> feasible(Model26Util::getNumberOfNodes(),-1);
	/*for(int i=0;i<Model26Util::getNumberOfNodes();i++)
		feasible.push_back(-1);*/
	int stato=0;
	bool eterogenee=false,insert=true;
	std::stringstream prec;
	std::string exec_host;
	while(fgets(buff,1024,fp))
	{
		std::string s(buff);
		if(stato==1 && s[0]=='\t' && s.size()>2)
		{
			s.erase(s.end()-1);
			s.erase(s.begin());
			prec<<s;
			//std::cout<<"0--->"<<prec.str()<<"<---"<<std::endl;
			
		}
		else if(stato==1)
		{
			//std::cout<<"FINITO --->"<<prec.str()<<"<---"<<std::endl;
			stato=0;
			std::vector<std::string> vettSelect=Util::split(prec.str(), "+");
			if(vettSelect.size()>1)
			{
				eterogenee=true;
				for(unsigned int i=0;i<vettSelect.size();i++)
				{
					nodes=1,cores=1,gpu=0,mic=0,mem=0,ghz=-1,host="";
					nodes=atoi(vettSelect[i].c_str());
					if((pos=vettSelect[i].find("host="))<vettSelect[i].size())
					{
						std::string sel=vettSelect[i];
						sel.erase(0,sizeof(char)*(strlen("host=")+pos));
						host=sel;
					}
					if((pos=vettSelect[i].find("ncpus="))<vettSelect[i].size())
					{
						std::string sel=vettSelect[i];
						sel.erase(0,sizeof(char)*(strlen("ncpus=")+pos));
						cores=atoi(sel.c_str());
					}
					if((pos=vettSelect[i].find("ngpus="))<vettSelect[i].size())
					{
						std::string sel=vettSelect[i];
						sel.erase(0,sizeof(char)*(strlen("ngpus=")+pos));
						gpu=atoi(sel.c_str());
					}
					if((pos=vettSelect[i].find("nmics="))<vettSelect[i].size())
					{
						std::string sel=vettSelect[i];
						sel.erase(0,sizeof(char)*(strlen("nmics=")+pos));
						mic=atoi(sel.c_str());
					}
					if((pos=vettSelect[i].find("mem="))<vettSelect[i].size())
					{
						std::string sel=vettSelect[i];
						sel.erase(0,sizeof(char)*(strlen("mem=")+pos));
						int mul=1;
						if((pos=sel.find("kb"))<sel.size() || (pos=sel.find("Kb"))<sel.size() || (pos=sel.find("KB"))<sel.size() )
						{
							mul=1;
						}
						else if((pos=sel.find("mb"))<sel.size() || (pos=sel.find("Mb"))<sel.size() || (pos=sel.find("MB"))<sel.size() )
						{
							mul=1024;
						}
						else if((pos=sel.find("gb"))<sel.size() || (pos=sel.find("Gb"))<sel.size() || (pos=sel.find("GB"))<sel.size() )
						{
							mul=1024*1024;
						}
						mem=atoi(sel.c_str())*mul;
					}
					if((pos=vettSelect[i].find("cpuspeed="))<vettSelect[i].size())
					{
						std::string sel=vettSelect[i];
						sel.erase(0,sizeof(char)*(strlen("cpuspeed=")+pos));
						if(sel.compare("any")==0)
							ghz=-1;
						else
							ghz=atoi(sel.c_str());
					}
					std::stringstream newId;
					newId<<id<<"("<<i<<")";
					Job j(newId.str(),nome,user,coda,eqt,nodes,cores*nodes,gpu*nodes,mic*nodes,mem*nodes,ghz,estDur,realDur,start,u,host);
					//j.initFeasibility(feasible);
					j.setHolding(holding);
					if(insert)
						ja.push_back(j);
				}
				//creo array di jobs
			}
			else
			{
				//nodes=1,cores=1,gpu=0,mic=0,mem=0,ghz=-1,host="";
				nodes=atoi(prec.str().c_str());
				if((pos=prec.str().find("host="))<prec.str().size())
				{
					std::string sel=prec.str();
					sel.erase(0,sizeof(char)*(strlen("host=")+pos));
					host=sel;
				}
				if((pos=prec.str().find("ncpus="))<prec.str().size())
				{
					std::string sel=prec.str();
					sel.erase(0,sizeof(char)*(strlen("ncpus=")+pos));
					cores=atoi(sel.c_str())*nodes;
				}
				if((pos=prec.str().find("ngpus="))<prec.str().size())
				{
					std::string sel=prec.str();
					sel.erase(0,sizeof(char)*(strlen("ngpus=")+pos));
					gpu=atoi(sel.c_str())*nodes;
				}
				if((pos=prec.str().find("nmics="))<prec.str().size())
				{
					std::string sel=prec.str();
					sel.erase(0,sizeof(char)*(strlen("nmics=")+pos));
					mic=atoi(sel.c_str())*nodes;
				}
				if((pos=prec.str().find("mem="))<prec.str().size() )
				{
					
					std::string sel=prec.str();
					sel.erase(0,sizeof(char)*(strlen("mem=")+pos));
					int mul=1;
					if((pos=sel.find("kb"))<sel.size() || (pos=sel.find("Kb"))<sel.size() || (pos=sel.find("KB"))<sel.size() )
					{
						mul=1;
					}
					else if((pos=sel.find("mb"))<sel.size() || (pos=sel.find("Mb"))<sel.size() || (pos=sel.find("MB"))<sel.size() )
					{
						mul=1024;
					}
					else if((pos=sel.find("gb"))<sel.size() || (pos=sel.find("Gb"))<sel.size() || (pos=sel.find("GB"))<sel.size() )
					{
						mul=1024*1024;
					}
					mem=atoi(sel.c_str())*mul*nodes;
				}
				if((pos=prec.str().find("cpuspeed="))<prec.str().size())
				{
					std::string sel=prec.str();
					sel.erase(0,sizeof(char)*(strlen("cpuspeed=")+pos));
					if(sel.compare("any")==0)
						ghz=-1;
					else
						ghz=atoi(sel.c_str());
				}
				//getchar();
			}		
			Util::trim(s);		
			if(s.size()>1)
				s.erase(s.end()-1);
			//std::cout<<"1--"<<s<<"--"<<std::endl;
		}
		else
		{
			Util::trim(s);	
			if(s.size()>1)		
				s.erase(s.end()-1);
			//std::cout<<"2--"<<s<<"--"<<std::endl;
		}	
		//std::cout<<"3--"<<s<<"--"<<std::endl;	
		if((pos=s.find("Job_Name = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Job_Name = "));
			nome=s;
			//std::cout<<"nome=s "<<s<<std::endl;
		}
		else if((pos=s.find("Job_Owner = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Job_Owner = "));
			user=s;
			//std::cout<<"user=s "<<s<<std::endl;
		}
		else if((pos=s.find("queue = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("queue = "));
			coda=s;
			//std::cout<<"coda=s "<<s<<std::endl;
		}
		else if((pos=s.find("job_state = H"))<s.size())
		{
			holding=1;
			//std::cout<<"holding=1 "<<s<<std::endl;
		}
		else if((pos=s.find("job_state = F"))<s.size())
		{
			insert=false;
			std::cout<<"Finished Job "<<id<<std::endl;
		}
		else if((pos=s.find("etime = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("etime = "));
			eqt=Util::str2ToTime(s.c_str());
			//std::cout<<"eqt=s "<<s<<std::endl;
		}
		else if((pos=s.find("qtime = "))<s.size() && eqt<=1)
                {
                        s.erase(0,sizeof(char)*strlen("etime = "));
                        eqt=Util::str2ToTime(s.c_str());
                        //std::cout<<"eqt=s "<<s<<std::endl;
                }
		else if((pos=s.find("Resource_List.nodect = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Resource_List.nodect = "));
			nodes=atoi(s.c_str());
			//std::cout<<"nodes=s "<<s<<std::endl;
		}
		else if((pos=s.find("Resource_List.ncpus = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Resource_List.ncpus = "));
			cores=atoi(s.c_str());
			//std::cout<<"cores=s "<<s<<std::endl;
		}
		else if((pos=s.find("Resource_List.ngpus = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Resource_List.ngpus = "));
			gpu=atoi(s.c_str());
			//std::cout<<"gpu=s "<<s<<std::endl;
		}
		else if((pos=s.find("Resource_List.nmics = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Resource_List.nmics = "));
			mic=atoi(s.c_str());
			//std::cout<<"mic=s "<<s<<std::endl;
		}
		else if((pos=s.find("Resource_List.mem = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Resource_List.mem = "));
			int mul=1;
			if((pos=s.find("kb"))<s.size() || (pos=s.find("Kb"))<s.size() || (pos=s.find("KB"))<s.size() )
			{
				mul=1;
			}
			else if((pos=s.find("mb"))<s.size() || (pos=s.find("Mb"))<s.size() || (pos=s.find("MB"))<s.size() )
			{
				mul=1024;
			}
			else if((pos=s.find("gb"))<s.size() || (pos=s.find("Gb"))<s.size() || (pos=s.find("GB"))<s.size() )
			{
				mul=1024*1024;
			}
			mem=atoi(s.c_str())*mul;
			//std::cout<<"mem=s*mul "<<s<<" * "<<mul<<std::endl;
		}
		else if((pos=s.find("Resource_List.select = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Resource_List.select = "));
			stato=1;
			//std::cout<<"Stato = 1"<<std::endl;
			//std::cout<<"select=s "<<s<<std::endl;
			//getchar();
			prec<<s;
		}
		
		else if((pos=s.find("Resource_List.walltime = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("Resource_List.walltime = "));
			estDur = Util::strHHMMSSToTime(s.c_str());
			if(realDur==-1)
				realDur= Util::strHHMMSSToTime(s.c_str());
			//std::cout<<"estDur=s "<<s<<std::endl;
			//std::cout<<"Letta durata stimata"<<std::endl;
		}
		/*else if((pos=s.find("resources_used.walltime = "))<s.size())
		{
			realDur= Util::strHHMMSSToTime(s.c_str());
		}*/
		else if((pos=s.find("stime = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("stime = "));
			start= Util::str2ToTime(s.c_str());
			//std::cout<<"start=s "<<s<<std::endl;
		}
		else if((pos=s.find("exec_host = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("exec_host = "));
			exec_host=s;
		}
		else if((pos=s.find("exec_vnode = "))<s.size())
		{
			s.erase(0,sizeof(char)*strlen("exec_vnode = "));
			std::stringstream tot;
			//std::cout<<"exec =s "<<s<<std::endl;
			tot<<s;
			do
			{
				fgets(buff,1024,fp);
				std::string tmp(buff);
				Util::trim(tmp);	
					
				if(tmp.size()>1)	
					tmp.erase(tmp.end()-1);
				
				//std::cout<<"tmp "<<tmp<<std::endl;
				if(buff[0]!=' ')
					tot<<tmp;
			}while(buff[0]!=' ');
			//std::cout<<"Tot ->"<<tot.str()<<"<-"<<std::endl;
			exec_vnodePrec=tot.str();
			std::vector<std::string> vettNodi=Util::split(tot.str(), "(");
			if(vettNodi.size()==1)
			{
				vettNodi=Util::split(tot.str(), "+");
			}
			//std::cout<<vettNodi[0]<<std::endl<<"trovati num nodi "<<vettNodi.size()<<std::endl;
			//getchar();
			for(unsigned int i=0;i<vettNodi.size();i++)
			{
				//std::cout<<vettNodi[i]<<std::endl;
				for(int j=0;j<nodesArray.size();j++)
				{	
					unsigned int p2=0;
					if((pos=vettNodi[i].find(nodesArray[j].getNodeName()))<vettNodi[i].size() && (p2=vettNodi[i].find("-mic"))>vettNodi[i].size())
					{
						u[j]++;
					}
				}
			}
			//getchar();
		}

	}
	pclose(fp);
	if(!eterogenee)
	{
		Job j(id,nome,user,coda,eqt,nodes,cores,gpu,mic,mem,ghz,estDur,realDur,start,u,host);
		//j.initFeasibility(feasible);
		j.setHolding(holding);
		
		if(start==INT_MAX)
		{
			j.setScheduled(false);
		}
		else if(!holding)
		{
			int totUsed=0;
			for(unsigned int i=0;i<u.size();i++)
			{
				totUsed+=u[i];
			}
			if(totUsed!=nodes)
			{
				std::vector<std::string> vettNodi=Util::split(exec_host, "+");
				
				for(unsigned int i=0;i<vettNodi.size();i++)
				{
					for(int j=0;j<nodesArray.size();j++)
					{	
						unsigned int pos=0;
						if((pos=vettNodi[i].find(nodesArray[j].getNodeName()))<vettNodi[i].size())
						{
							u[j]++;
						}
					}
				}
				j.allocate(start,u);
			}
			j.setScheduled(true);
		}
		std::cout<<"Letto "<<j.toString(0,0)<<std::endl;
		
		if(insert)
			ja.push_back(j);
	}
	else
	{
		std::vector<std::string> vettNodi=Util::split(exec_vnodePrec, "(");
		if(vettNodi.size()==1)
		{
			vettNodi=Util::split(exec_vnodePrec, "+");
		}
		for(unsigned int i=0;i<vettNodi.size();i++)
		{
			if(vettNodi[i].size()>1)
				vettNodi[i].erase(vettNodi[i].find(":"),vettNodi[i].size());
		}
		unsigned int indiceJob=1;
		unsigned int indiceVettNodi=0;
		do //Può sembrare brutto codice ma, FIDATI, va fatto così 
		{
			std::vector<int> utilizzazione(nodesArray.size(),0);
			int countNodiInseriti=0;
			for(;indiceVettNodi<vettNodi.size() && countNodiInseriti<ja[indiceJob].getNumberOfNodes();indiceVettNodi++)
			{
				for(int j=0;j<nodesArray.size();j++)
				{
					if(vettNodi[indiceVettNodi].compare(nodesArray[j].getNodeName())==0)
					{	
						countNodiInseriti++;
						utilizzazione[j]++;
					}
				}
			}
			ja[indiceJob].allocate(start,utilizzazione);
				
			indiceJob++;
			if(indiceJob>=ja.size())
			{
				indiceJob=0;	
			}
		}while(indiceJob!=1);
		
		for(unsigned int i=0;i<ja.size();i++)
		{
			ja[i].setEstimatedDuration(estDur);
			ja[i].setRealDuration(realDur);
			ja[i].setEnterQueueTime(eqt);
			
			if(start==INT_MAX)
			{
				ja[i].setScheduled(false);
			}
			else
			{
				ja[i].setScheduled(true);
			}
			std::cout<<"Letto eterogeneo "<<ja[i].toString(0,0)<<std::endl;
			//getchar();
		}	
	}
	//getchar();
	
	return ja;
	
	
	/*std::cout<<"RILETTO "<<id<<j.isScheduled()<<" "<<nome<<" "<<user<<" "<<coda<<" "<<eqt<<" "<<nodes<<" "<<cores<<" "<<gpu<<" "<<mic<<" "<<mem<<" "<<estDur<<" "<<realDur<<" "<<start<<"[";
	for(unsigned int i=0;i<u.size();i++)
	{
		std::cout<<u[i];
	}
	std::cout<<"]"<<std::endl;*/
}

inline std::vector<Job> JobReader::readFromPBS(std::vector<Job> old,NodeArray nodes)
{
	std::stringstream exec_str;
	exec_str<<"$PBS_EXEC/bin/qselect";
	FILE * fp=popen(exec_str.str().c_str(),"r");
	if(!fp)
	{
		std::cout<<"cant read command output readFromPBS "<< exec_str.str()<<std::endl;
		//getchar();
	}
	std::vector<Job> ja;
	char buff[1024];
	while(fgets(buff,1024,fp))
	{	
		std::string s(buff);
		if(s.size()>1)
		{
			s.erase(s.end()-1);
		
			int index=-1;
			for(unsigned int i=0;i<old.size();i++)
			{
				unsigned int pos;
				if((pos=old[i].getJobId().find(s))<old[i].getJobId().size() || (pos=s.find(old[i].getJobId()))<s.size())
				{
					//std::cout<<"found1 old job "<<old[i].getJobId()<<" at index "<<i<<std::endl;
					index=i;
					ja.push_back(old[index]);
				}
			}
			if(index<0)//else
			{
				unsigned int pos;
				if((pos=s.find("[]"))<s.size())
				{
					std::stringstream job_cmd;
					std::string shortS=s;
					std::string startStringId=shortS.erase(shortS.find("["),shortS.size());
					job_cmd<<"$PBS_EXEC/bin/qstat -xt "<<s<<" | grep "<<startStringId<<" | grep -v \"\\[\\]\" | grep -v \" X \"";
					//std::cout<<job_cmd.str()<<std::endl;
					FILE * fp2=popen(job_cmd.str().c_str(),"r");
					if(!fp2)
					{
						std::cout<<"cant read command output readJob qstat [] "<<job_cmd.str()<<std::endl;
						//exit(-1);
						//getchar();
					}
					char buff2[1024];
					while(fgets(buff2,1024,fp2))
					{
						std::string line(buff2);
						std::string subId=Util::split(line,".")[0];
						//std::cout<<"JOB ARRAY letto [] "<<subId<<std::endl;
						std::vector<Job> r=readJob(subId,nodes);
						for(unsigned int i=0;i<r.size();i++)
						{
							if( !r[i].getJobId().empty())
								ja.push_back(r[i]);
						}
						std::stringstream nomeFile;
						time_t today;
						time(&today);
						nomeFile<<"JobsLetti_"<<Util::dateToStrNoDel(today);
						std::ofstream output(nomeFile.str().c_str(), std::ofstream::app);
						if (!output) 
						{
							std::cout << "ERROR: Could not open file \"" << nomeFile.str() << "\"" << std::endl;
							getchar();
						}
						output<<subId<<std::endl;
						output.close();
					}
					fclose(fp2);
					//getchar();
				}
				else
				{
					std::vector<Job> r=readJob(s,nodes);
					for(unsigned int i=0;i<r.size();i++)
					{
						if( !r[i].getJobId().empty())
						{
							ja.push_back(r[i]);
							
						}
					}
					std::stringstream nomeFile;
					time_t today;
					time(&today);
					nomeFile<<"JobsLetti_"<<Util::dateToStrNoDel(today);
					std::ofstream output(nomeFile.str().c_str(), std::ofstream::app);
					if (!output) 
					{
						std::cout << "ERROR: Could not open file \"" << nomeFile.str() << "\"" << std::endl;
						getchar();
					}
					output<<s<<std::endl;
					output.close();
				}
			}
		}
	}
	pclose(fp);
	return ja;
}


inline std::vector<Job> JobReader::read()
{
	std::ifstream input(_file.c_str());
	
	std::vector<Job> job;
	if (!input) 
	{
		std::cout << "ERROR: Could not open file \"" << _file << "\"" << std::endl;
		
	}
	else
	{
		std::string line;
		int letti=0;
		while ( getline (input,line) )
		{
			std::string id,name,userName,coda;
			/*std::cout<<line<<std::endl;	
			std::cout<<"qui0"<<std::endl;*/
			int enterQueueTime,start,nodes=0,cores=0,gpu=0,mic=0,mem=0,estimatedDuration,realDuration,ghz=-1;
			std::vector<int> usedNodes(Model26Util::getNumberOfNodes(),0);	
			bool ready=true;
			std::vector<std::string> terzi=Util::split(line,"__");
			std::vector<std::string> data1=Util::split(terzi[0],";");

		
			//std::cout<<"qui01"<<std::endl;
			enterQueueTime = Util::strToTime(data1[4]);
			//for(int i=0;i<terzi.size();i++)
			//	std::cout<<terzi[i]<<std::endl;
			std::vector<std::string> data3=Util::split(terzi[2],";");

			//std::cout<<"qui012"<<std::endl;
			bool gpuMicPresenti=false;
	
			//std::cout<<"qui013"<<std::endl;
			start = Util::strToTime(data3[0]);
	
			//std::cout<<"qui014"<<std::endl;
			realDuration = Util::strToTime(data3[1])-start;
			//std::cout<<"qui02"<<std::endl;
			if(data3[2].compare("--")!=0)
				nodes=atoi(data3[2].c_str());
			else
				nodes=-1;
			if(data3[3].compare("--")!=0)
				cores=atoi(data3[3].c_str());
			else
			{
				ready=false;
			}
			//std::cout<<"qui1"<<std::endl;
			/*if(data3.size()==9)
			{
			if(data3[4].compare("--")!=0)
			gpu=atoi(data3[4].c_str());
			else
			{
			ready=false;
			}
				if(data3[5].compare("--")!=0)
			mic=atoi(data3[5].c_str());
			else 
			{
			ready=false;
			}
			if(data3[6].compare("--")!=0)
			{
			int mul=1;
			if(data3[6][data3[6].size()-2]=='g')
			mul=1024*1024;
			else if (data3[6][data3[6].size()-2]=='m')
			mul=1024;
			mem=atoi(data3[6].c_str())*mul;
			}
			else
			{
			ready=false;
			}
			estimatedDuration = Util::strTimeToTime(data3[7]);
			gpuMicPresenti=true;
			}*/
			//else
			//{
			if(data3[4].compare("--")!=0)
			{
				int mul=1;
				if(data3[4][data3[4].size()-2]=='g')
					mul=1024*1024;
				else if (data3[4][data3[4].size()-2]=='m')
					mul=1024;
				mem=atoi(data3[4].c_str())*mul;
			}
			else 
			{
				ready=false;
			}
			estimatedDuration = Util::strHHMMToTime(data3[5]);
			//std::cout<<data3[5]<<" "<<estimatedDuration<<std::endl;
			//getchar();
			//}
			//std::cout<<"qui2"<<std::endl;
			std::vector<std::string> data2=Util::split(terzi[1],"#");
			if(nodes==-1)
				nodes=data2.size();
	
			if(!ready || !gpuMicPresenti)
			{
				nodes=data2.size(),cores=0,gpu=0,mic=0,mem=0;
				for(unsigned int i=0;i<data2.size();i++)
				{
					if(data2[i].size()>0)
					{
						std::vector<std::string> sottoInfo=Util::split(data2[i],";");
						//int index=atoi(sottoInfo[0].c_str())-1;
						//usedNodes[index]=1;
				
						//if(cores==-1)
						cores+=atoi(sottoInfo[1].c_str());
						//if(!gpuMicPresenti || mic==-1 || gpu==-1)
						//{
						gpu+=atoi(sottoInfo[2].c_str());
						mic+=atoi(sottoInfo[3].c_str());
						//}
						//if(mem==-1)
						mem+=atoi(sottoInfo[4].c_str());
					}
				}
			}
			//std::cout<<"qui3"<<std::endl;
			if(estimatedDuration==0)
				estimatedDuration=((realDuration/60)+1)*60;
			if(realDuration==0)
				realDuration=1;
			id=data1[0];
			name=data1[1];
			userName=data1[2];
			coda=data1[3];
			if(nodes==0)
				nodes=1;
			if(cores==0)
				cores=4;
			if(mem==0)
				mem=200*1024;
			//std::cout<<"qui4"<<std::endl;
			/*if(cores/nodes>16)
			{
				std::cout<<"TROPPI CORE"<<std::endl;
				getchar();
			}
			if(gpu/nodes>2)
			{
				std::cout<<"TROPPI GPU"<<std::endl;
				getchar();
			}
			if(mic/nodes>2)
			{
				std::cout<<"TROPPI MIC"<<std::endl;
				getchar();
			}
			if(gpu/nodes>0 && mic>0)
			{
				std::cout<<"GPU con MIC mischiati"<<std::endl;
				getchar();
			}
			if(mem/nodes>33554432)
			{
				std::cout<<"TROPPI CORE"<<std::endl;
				getchar();
			}*/
			//std::cout<<id<<" "<<name<<" "<<userName<<" "<<coda<<" "<<enterQueueTime<<" "<<nodes<<" "<<cores<<" "<<gpu<<" "<<mic<<" "<<mem<<" "<<estimatedDuration<<" "<<realDuration<<" "<<start<<std::endl;
			job.push_back(Job(data1[0],data1[1],data1[2],data1[3],enterQueueTime,nodes,cores,gpu,mic,mem,ghz,estimatedDuration,realDuration,start,usedNodes,""));
			letti++;
		}
		//std::cout<<"Letti "<<letti<<" Jobs"<<std::endl;
	}
	input.close();
	return job;
}
#endif
