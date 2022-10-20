/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                File: QueueReader.hpp                                                 *
*                                                                                                                      *
*                                                                                                                      *
 **********************************************************************************************************************/

#ifndef Def_QueueReader
#define Def_QueueReader
#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "Queue.hpp"
#include "Model26Util.hpp"
//#include "Util.hpp"
#include <sstream>
#include <climits>

//using namespace std;
class QueueReader
{
	private:
		std::string _file;
	public:
		QueueReader(std::string file);
		QueueReader(){};
		std::vector<Queue> read();
		std::vector<Queue> readFromPBS();
};

inline std::vector<Queue> QueueReader::readFromPBS()
{
	std::stringstream job_cmd;
	//job_cmd<<"$PBS_EXEC/bin/qstat -Q -f | grep \"Queue: \\|Priority = \\|started = \" ";
	job_cmd<<"qstat -Q -f";
	FILE * fp=popen(job_cmd.str().c_str(),"r");
	if(!fp)
	{
		std::cout<<"cant read command output "<<job_cmd.str()<<std::endl;
		//exit(-1);
	}
	char buff[1024];
	std::string coda;
	int priorita=INT_MIN;
	bool enabled=true;
	unsigned int pos=0;
	std::vector<Queue> code;
	std::vector<std::string> dc;
	std::vector<int> util(Model26Util::getNumberOfNodes(),1);
	while(fgets(buff,1024,fp))
	{
	
		std::string s(buff);
		if(s.size()>1)
			s.erase(s.end()-1);
		
		//Util::trim(s);	
		//cout<<"CODA -- "<<s<<"--"<<endl;
		std::string q("Queue: "),p("    Priority = "),e("    started = "),c("    default_chunk.Qlist = "),c2("    default_chunk.Qlist += ");
		if((pos=s.find(q))<s.size())
		{
			if(priorita!=INT_MIN)
			{	
				Queue tmp(coda,priorita,enabled,util,dc);
				code.push_back(tmp);
				//cout<<"Inserita coda: "<<coda<<" con pri: "<<priorita<<" max wait "<<tmp.getMaxMinutesToWait()<<" def chunk: ...";
				/*for(unsigned int i=0;i<dc.size();i++)
				{
					cout<<dc[i]<<", ";
				}
				cout<<endl;*/
				dc.clear();
			}
			
			s.erase(0,sizeof(char)*q.size());
			coda=s;
			if(coda[0]=='R')
				priorita=1023;
			else
				priorita=800;
		}
		else if((pos=s.find(p))<s.size())
		{
			s.erase(0,sizeof(char)*p.size());
			priorita=atoi(s.c_str());
		}
		else if((pos=s.find(e))<s.size())
		{
			s.erase(0,sizeof(char)*e.size());
			if(s.compare("true")==0 || s.compare("True")==0)
				enabled=true;
			else
				enabled=false;
		}
		else if((pos=s.find(c))<s.size())
		{
			s.erase(0,sizeof(char)*c.size());
			dc.push_back(s);
		}
		else if((pos=s.find(c2))<s.size())
		{
			s.erase(0,sizeof(char)*c2.size());
			dc.push_back(s);
		}
	
	}

	Queue tmp(coda,priorita,enabled,util,dc);
	code.push_back(tmp);
	/*cout<<"Inserita coda: "<<coda<<" con pri: "<<priorita<<" max wait "<<tmp.getMaxMinutesToWait()<<" def chunk: ";
	for(unsigned int i=0;i<dc.size();i++)
	{
		cout<<dc[i]<<", ";
	}	
	cout<<endl;*/
	pclose(fp);
	//getchar();
	return code;
}

inline QueueReader::QueueReader(std::string file)
{
	_file=file;
}

inline std::vector<Queue> QueueReader::read()
{
	std::ifstream inputCode(_file.c_str());
	if (!inputCode) 
	{
		std::cout << "ERROR: Could not open file \"" << _file << "\"" << std::endl;
		exit(1);
	}
	std::vector<std::string> chunk;
	std::vector<Queue> code;
	std::vector<int> u(Model26Util::getNumberOfNodes(),1);
	while (!inputCode.eof())
	{
		int w;
		std::string id;
		inputCode>>id;
		inputCode>>w;
		code.push_back(Queue(id,w,true,u,chunk));
	}
	inputCode.close();
	return code;
}
#endif
