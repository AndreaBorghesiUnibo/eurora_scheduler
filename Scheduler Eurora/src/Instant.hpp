/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                  File: Instant.hpp                                                   *
*                                                                                                                      *
*                                                                                                                      *
 ***********************************************************************************************************************/

#ifndef Def_Instant
#define Def_Instant

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <climits>
#include <stdio.h>
#include <stdlib.h>

#include <ilcp/cp.h>
#include <iostream>
#include <ctime>
#include <math.h>
#include <sstream>
#include <time.h> 
#include <algorithm>
#include <thread>





class Instant 
{
	private:
		
		int wS,wE,satS,satE,sunS,sunE,maxPrime,maxNonPrime;
		/*void taskAt(std::vector< std::vector<int> > primeIntervals,std::vector< std::vector<int> > nonPrimeIntervals,time_t now)
		{
			for(unsigned int i=0;i<primeIntervals[0].size();i++)
			{
				
				if(primeIntervals[0][i]>now && primeIntervals[0][i]<(now+2*86400))
				{
					std::stringstream command;
					command<<"res=no;for i in `atq | awk '{ print $2$3}' | tr -d - | tr -d : ` ; do if test $i -eq "<<Util::timeToStrForAtNoSec(primeIntervals[0][i])<<";then res=si; fi;done; if test $res != 'si';then at -f $MODEL26_HOME/util/changeinterval.sh -t "<<Util::timeToStrForAt(primeIntervals[0][i])<<" 2>/dev/null ;fi &";
					system(command.str().c_str());
				}
			}
			
			for(unsigned int i=0;i<nonPrimeIntervals[0].size();i++)
			{
				if(nonPrimeIntervals[0][i]>now && nonPrimeIntervals[0][i]<(now+2*86400))
				{
					
					std::stringstream command2;
					command2<<"res=no;for i in `atq | awk '{ print $2$3}' | tr -d - | tr -d : ` ; do if test $i -eq "<<Util::timeToStrForAtNoSec(nonPrimeIntervals[0][i])<<";then res=si; fi;done; if test $res != 'si';then at -f $MODEL26_HOME/util/changeinterval.sh -t "<<Util::timeToStrForAt(nonPrimeIntervals[0][i])<<" 2>/dev/null ;fi &";
					system(command2.str().c_str());
				}
			}
		}*/
	public:
		

		std::vector< std::vector<int> > getIntervals(time_t start,time_t end,bool prime)
		{
			std::vector< std::vector<int> > primeIntervals(2,std::vector<int>());
			std::vector< std::vector<int> > nonPrimeIntervals(2,std::vector<int>());
			time_t prev=start;

			for(time_t i=start;i<=end;i+=60*60*24)
			{
				std::string res=Util::timeToDayofWeek(i);

				time_t intStart,intEnd;
				unsigned int pos=-1;
				std::tm * ptm = std::localtime(&i);
				char buffer[32];		
				std::strftime(buffer, 32, "%Y-%m-%d 00:00:00", ptm);  
				std::string data(buffer);

				time_t base=Util::strToTime(data);

				intStart=intEnd=base;
				if((pos=res.find("SAT"))<res.size())
				{
					intStart+=satS;
					intEnd+=satE;
				}
				else if((pos=res.find("SUN"))<res.size())
				{
					intStart+=sunS;
					intEnd+=sunE;
				}
				else
				{
					intStart+=wS;		
					intEnd+=wE;
				}
				if(intStart!=intEnd )
				{
		
					ptm = std::localtime(&intStart);
					std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);  
					std::string res1(buffer);
					ptm = std::localtime(&intEnd);
					std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);  
					std::string res2(buffer);
					ptm = std::localtime(&prev);
					std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);  
					std::string res3(buffer);		
					if(intEnd-intStart>0)
					{
						primeIntervals[0].push_back(intStart);
						primeIntervals[1].push_back(intEnd-intStart);
					}
					if(intStart-prev>0)
					{					
						nonPrimeIntervals[0].push_back(prev);
						nonPrimeIntervals[1].push_back(intStart-prev);
					}					
					prev=intEnd;
				}
			}
			char buffer[32];
			std::tm * ptm = std::localtime(&prev);
			std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);  
			std::string res3(buffer);
			ptm = std::localtime(&end);
			std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);  
			std::string res1(buffer);
			if(end-prev>0)
			{
				nonPrimeIntervals[0].push_back(prev);
				nonPrimeIntervals[1].push_back(end-prev);
			}
			time_t now=0;
			time(&now);
			
			//taskAt(primeIntervals,nonPrimeIntervals,now);
			/*std::thread t1(&Instant::taskAt, this,primeIntervals,nonPrimeIntervals,now,max);
			t1.detach();*/
			
			//std::cout<<"maxPrime:"<<maxPrime<<std::endl;
			for(unsigned int i=0;i<primeIntervals[0].size();i++)
			{
				if(primeIntervals[0][i]>now && primeIntervals[0][i]>maxPrime && primeIntervals[0][i]<(now+86400*2))
				{
					maxPrime=primeIntervals[0][i];
					//std::cout<<"maxPrime="<<maxPrime<<std::endl;
					std::stringstream command;
					command<<"at -f $MODEL26_HOME/util/changeinterval.sh -t "<<Util::timeToStrForAt(primeIntervals[0][i])<<" 2>/dev/null";
					system(command.str().c_str());
				
				}
			}
			//getchar();
			//std::cout<<"maxNonPrime:"<<maxNonPrime<<std::endl;
			for(unsigned int i=0;i<nonPrimeIntervals[0].size();i++)
			{
				if(nonPrimeIntervals[0][i]>now && nonPrimeIntervals[0][i]>maxNonPrime && nonPrimeIntervals[0][i]<(now+86400*2))
				{
					maxNonPrime=nonPrimeIntervals[0][i];
					//std::cout<<"maxNonPrime="<<maxNonPrime<<std::endl;
					std::stringstream command2;
					command2<<"at -f $MODEL26_HOME/util/changeinterval.sh -t "<<Util::timeToStrForAt(nonPrimeIntervals[0][i])<<" 2>/dev/null";
					system(command2.str().c_str());
				}
			}
			//getchar();
				
			if(prime)
				return primeIntervals;
			else
				return nonPrimeIntervals;
		}

		Instant()
		{
			//std::cout<<"COSTRUTTORE INSTANT "<<std::endl;
			//getchar();
			maxPrime=maxNonPrime=0;
			std::stringstream echo_cmd;			
			echo_cmd<<"echo $PBS_HOME";
			FILE * fpe=popen(echo_cmd.str().c_str(),"r");
			if(!fpe)
			{
				std::cout<<"cant read command output "<<echo_cmd.str()<<std::endl;
				//exit(-1);
				getchar();
			}
			char buffE[2048];
			while(fgets(buffE,2048,fpe));
			int l=strlen(buffE);
			buffE[l-2]='\0';
			//std::cout<<"--"<<buffE<<"--"<<std::endl;
			fclose(fpe);

			std::stringstream job_cmd;
			job_cmd<<buffE<<"/sched_priv/holidays";
			FILE * fp=fopen(job_cmd.str().c_str(),"r");
			if(!fp)
			{
				std::cout<<"cant read holidays file "<<job_cmd.str()<<std::endl;
				//getchar();
			}
			char buff[2048];
			std::string week,sat,sun;
			wS=0,wE=0,satS=0,satE=0,sunS=0,sunE=0;
			while(fgets(buff,2048,fp))
			{
				std::string s(buff);
				//Util::trim(s);	
				if(s.size()>1)	
					s.erase(s.end()-1);
				unsigned int pos=-1;
				//std::cout<<"--"<<s<<"--"<<std::endl;
				if((pos=s.find("  weekday\t"))<s.size())
				{
					s.erase(0,sizeof(char)*strlen("  weekday\t"));
					week=s;
					if((pos=s.find("none"))==0)
					{
						wE=wS=60*60*24;
					}
					else if((pos=s.find("all"))==0)
					{
						wS=0;
						wE=60*60*24;
					}
					else
					{	
						std::string hI=week.substr(0,2);
						std::string mI=week.substr(2,2);
						wS=atoi(hI.c_str())*60*60+atoi(mI.c_str())*60;
						std::string resto=week.substr(5);
						std::string hF=week.substr(5,2);
						std::string mF=week.substr(7,2);	
						if((pos=resto.find("all"))<resto.size())
						{
							wE=60*60*24;
						}
						else
						{
							wE=atoi(hF.c_str())*60*60+atoi(mF.c_str())*60;
						}
						//std::cout<<"W--"<<wS<<":"<<wE<<"--"<<std::endl;
					}
			
				}
				else if((pos=s.find("  saturday\t"))<s.size())
				{
					s.erase(0,sizeof(char)*strlen("  saturday\t"));
					sat=s;
					if((pos=s.find("none"))==0)
					{
						satE=satS=60*60*24;
					}
					else if((pos=s.find("all"))==0)
					{
						satS=0;
						satE=60*60*24;
					}
					else
					{	
						std::string hI=sat.substr(0,2);
						std::string mI=sat.substr(2,2);
						satS=atoi(hI.c_str())*60*60+atoi(mI.c_str())*60;
						std::string resto=sat.substr(5);
						std::string hF=sat.substr(5,2);		
						std::string mF=sat.substr(7,2);			
						if((pos=resto.find("all"))<resto.size())
						{
							satE=60*60*24;
						}
						else
						{
							satE=atoi(hF.c_str())*60*60+atoi(mF.c_str())*60;
						}
						//std::cout<<"W--"<<s<<" "<<satS<<":"<<satE<<"--"<<std::endl;
					}
				}
				else if((pos=s.find("  sunday\t"))<s.size())
				{
					s.erase(0,sizeof(char)*strlen("  sunday\t"));
					sun=s;
					if((pos=s.find("none"))==0)
					{
						sunE=sunS=60*60*24;
					}
					else if((pos=s.find("all"))==0)
					{
						sunS=0;
						sunE=60*60*24;
					}
					else
					{			
						std::string hI=sun.substr(0,2);
						std::string mI=sun.substr(2,2);
						sunS=atoi(hI.c_str())*60*60+atoi(mI.c_str())*60;
						std::string resto=sun.substr(5);
						std::string hF=sun.substr(5,2);
						std::string mF=sun.substr(7,2);	
						if((pos=resto.find("all"))<resto.size())
						{
							sunE=60*60*24;
						}
						else
						{
							sunE=atoi(hF.c_str())*60*60+atoi(mF.c_str())*60;
						}
						//std::cout<<"W--"<<sunS<<":"<<sunE<<"--"<<std::endl;
					}
				}
			
			}
			fclose(fp);		
		}

		
};
#endif
