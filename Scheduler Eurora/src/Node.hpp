/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                    File: Node.hpp                                                    *
*                                                                                                                      *
*                                                                                                                      *
 **********************************************************************************************************************/

#ifndef Def_Node
#define Def_Node

#include <sstream>
#include <stdlib.h>
#include <string>
#include <cstring>

//using namespace std;
class Node 
{
	private:
		std::string _name;
		int _numCores;
		int _numGPUs;
		int _numMICs;
		int _totMem;
		int _ghz;
		bool _usable;
		bool _working;
		std::vector<std::string> _allocatedQueue;
	public:
		std::string getNodeName(){return _name;}
		int getCoresNumber(){return _numCores;}
		int getGPUsNumber(){return _numGPUs;}
		int getMICsNumber(){return _numMICs;}
		int getTotalMemory(){return _totMem;}
		int getGHz(){return _ghz;}
		int isUsable(){return _usable==true?1:0;}
		int isWorking(){return _working==true?1:0;}
		std::string toString()
		{
			std::stringstream res;
			res<<"Nodo "<<_name<<" nCores: "<<_numCores<<" nGPUs: "<<_numGPUs<<" nMICs: "<<_numMICs<<" mem: "<<_totMem<<" GHz: "<<_ghz<<" usable: "<<_usable;
			if(_working)
				res<<" ON";
			else
				res<<" DOWN";
			return res.str();
		}

		//void clearAllocatedQueue(){_allocatedQueue.clear();}
		//void addAllocatedQueue(string coda){_allocatedQueue.push_back(coda);}
		int canAllocateToQueue(Queue coda)
		{
			//cout<<"aqsize"<<_allocatedQueue.size()<<endl;
			if(coda.getId()[0]=='R' || coda.getId()[0]=='S')
				return 1;
			if(_allocatedQueue.size()==0)
				return 1;
			for(unsigned int i=0;i<_allocatedQueue.size();i++)
			{
				//cout<<coda<<"==?"<<_allocatedQueue[i]<<". "<<_allocatedQueue[i].compare(coda)<<endl;
				if(_allocatedQueue[i].compare(coda.getId())==0)
				{
					//cout<<"yes"<<endl;
					return 1;
				}
				for(unsigned int j=0;j<coda.getDefaultChunk().size();j++)
				{
					if(_allocatedQueue[i].compare(coda.getDefaultChunk()[j])==0)
					{
						return 1;
					}
				}
			}
			return 0;
		}
		void setGHz(std::string ghz)
		{
			if(ghz.compare("any")==0)
				_ghz=-1;
			else
				_ghz=atoi(ghz.c_str());
		}
		void setNodeName(std::string n){ _name=n;}
		void setCoresNumber(int n){ _numCores=n;}
		void setGPUsNumber(int n){ _numGPUs=n;}
		void setMICsNumber(int n){ _numMICs=n;}
		void setTotalMemory(int n){ _totMem=n;}
		void setUsable(int r){ _usable=(r==1?true:false);}
		void setUsable(bool r){ _usable=r;} 
		void setWorking(int w){_working=(w==1?true:false);}
		void setWorking(bool w){_working=w;}
		Node(std::string name,int cores,int gpus,int mics,int mem,std::string ghz,bool usable,std::vector<std::string> aq)
		{
			setNodeName(name),setCoresNumber(cores),setGPUsNumber(gpus),setMICsNumber(mics),setTotalMemory(mem),setUsable(usable);
			setGHz(ghz);
			_allocatedQueue=aq;
			//cout<<"aqsize"<<_allocatedQueue.size()<<endl;
			//getchar();
		}

		void checkState()
		{
			//cout<<"Node check state"<<endl;
			if(isUsable())
			{
				std::stringstream exec_str;
				//exec_str<<"qmgr -c \"list node "<<getNodeName()<<" state\"";
				exec_str<<"pbsnodes -v "<<getNodeName();
				FILE * fp=popen(exec_str.str().c_str(),"r");
				if(!fp)
				{
					std::cout<<"cant read command output "<<exec_str.str()<<std::endl;
					//exit(-1);
					return;
				}
				char buff[1024];
				//size_t length,read;
				bool trovato=false;
				while(fgets(buff,1024,fp))
				{	
					std::string s(buff);
//std::cout<<getNodeName()<<" stato "<<s<<std::endl;
					//Util::trim(s);
					unsigned int pos=0;
					if((pos=s.find("state = "))<s.size())
					{
						trovato=true;
						s.erase(0,sizeof(char)*(strlen("state = ")+pos));
//std::cout<<getNodeName()<<" stato -"<<s<<"-"<<std::endl;
						if((pos=s.find("busy"))<s.size() || (pos=s.find("free"))<s.size() || (pos=s.find("job-busy"))<s.size()  )
						
						{
							//std::cout<<" "<<getNodeName()<<" stato WORKING C:"<<_numCores<<" G:"<<_numGPUs<<" Mi:"<<_numMICs<<" Mem:"<<_totMem<<" GHz:"<<_ghz<<std::endl;
							setWorking(true);
							pclose(fp);
							return;
						}
						else
						{
							//std::cout<<" "<<getNodeName()<<" SPENTO"<<std::endl;		
							setWorking(false);
							pclose(fp);
							return;
						}
						
					}
					if(!trovato)
					{
						//std::cout<<" "<<getNodeName()<<" stato SPENTO!!!!!!!!!"<<std::endl;		
						setWorking(false);
					}
				}
				pclose(fp);
			}
		}
};
#endif
