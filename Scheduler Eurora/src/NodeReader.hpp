/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                 File: NodeReader.hpp                                                 *
*                                                                                                                      *
*                                                                                                                      *
 **********************************************************************************************************************/

#ifndef Def_NodeReader
#define Def_NodeReader
#include <vector>
#include <iostream>
#include <fstream>
#include "Node.hpp"

class NodeReader
{
	private:
		std::string _file;
	public:
		NodeReader(std::string file);
		NodeReader(){};
		std::vector<Node> read();
		std::vector<Node> readFromPBS();
};

inline NodeReader::NodeReader(std::string file)
{
	_file=file;
}

inline std::vector<Node> NodeReader::readFromPBS()
{
	std::stringstream job_cmd;
	std::vector <Node> node;
	job_cmd<<"$PBS_EXEC/bin/pbsnodes -a ";
	FILE * fp=popen(job_cmd.str().c_str(),"r");
	if(!fp)
	{
		std::cout<<"cant read command output readNodeFrom PBS  pbsnodes "<<job_cmd.str()<<std::endl;
	}
	std::string name,ghz;
	int numCores=0;
	int numGPUs=0;
	int numMICs=0;
	int totMem=0;
	int usable=0;
	std::vector <std::string> code;
	char buff[1024];
	while(fgets(buff,1024,fp))
	{
		std::string s(buff);
		if(s.size()>1)
			s.erase(s.end()-1);
		unsigned int pos=0;
		//std::cout<<"--"<<s<<"--"<<std::endl;	
		if(s.size()>0 && s[0]!=' ')
		{
			//std::cout<<"Nome nodo "<<s<<std::endl;
			name =s;
			usable=1;
		}
		else if((pos=s.find("resources_available.cpuspeed = "))<s.size())
		{
			s.erase(0,sizeof(char)*(pos+strlen("resources_available.cpuspeed = ")));
			ghz=s;
			//std::cout<<"ghz=s "<<ghz<<std::endl;
		}
		else if((pos=s.find("resources_available.mem = "))<s.size())
		{
			s.erase(0,sizeof(char)*(pos+strlen("resources_available.mem = ")));
			int mul=1;
			if((pos=s.find("kb"))<s.size())
			{
				mul=1;
			}
			else if((pos=s.find("mb"))<s.size())
			{
				mul=1024;
			}
			else if((pos=s.find("gb"))<s.size())
			{
				mul=1024*1024;
			}
			totMem=atoi(s.c_str())*mul;
			//std::cout<<"totMem=s "<<totMem<<std::endl;
		}
		else if((pos=s.find("resources_available.ncpus = "))<s.size())
		{
			s.erase(0,sizeof(char)*(pos+strlen("resources_available.ncpus = ")));
			numCores=atoi(s.c_str());
			//std::cout<<"numCores=s "<<numCores<<std::endl;
		}
		else if((pos=s.find("resources_available.ngpus = "))<s.size())
		{
			s.erase(0,sizeof(char)*(pos+strlen("resources_available.ngpus = ")));
			numGPUs=atoi(s.c_str());
			//std::cout<<"numGPUs=s "<<numGPUs<<std::endl;
		}
		else if((pos=s.find("resources_available.nmics = "))<s.size())
		{
			s.erase(0,sizeof(char)*(pos+strlen("resources_available.nmics = ")));
			numMICs=atoi(s.c_str());
			//std::cout<<"numMICs=s "<<numMICs<<std::endl;
		}
		else if((pos=s.find("resources_available.Qlist = "))<s.size())
		{
			s.erase(0,sizeof(char)*(pos+strlen("resources_available.Qlist = ")));
			//std::cout<<" qlist "<<s<<" "<<s<<std::endl;
			code=Util::split(s,",");
			//code.push_back(s);
			/*for(unsigned int i=0;i<code.size();i++)
				std::cout<<code[i]<<std::endl;*/
		}
		else if(s.size()==0 && name.size()>0)
		{
			node.push_back(Node(name,numCores,numGPUs,numMICs,totMem,ghz,(usable==1?true:false),code));
			numCores=0;
			numGPUs=0;
			numMICs=0;
			totMem=0;
			usable=0;
		}
	}
	pclose(fp);
	//getchar();
	return node;
}

inline std::vector<Node> NodeReader::read()
{
	std::ifstream input(_file.c_str());
	if (!input) 
	{
		std::cout << "ERROR: Could not open file \"" << _file << "\"" << std::endl;
		exit(1);
	}
	
	std::vector<Node> node;
	while (!input.eof())
	{
		std::string name,ghz;
		int numCores;
		int numGPUs;
		int numMICs;
		int totMem;
		int usable;
		input>>name;
		input>>numCores;
		input>>numGPUs;
		input>>numMICs;
		input>>totMem;
		input>>ghz;
		input>>usable;
		//std::cout<<"Letto nodo "<<name<<" con "<<numCores<<" "<<numGPUs<<" "<<numMICs<<" "<<totMem<<" "<<(usable==1?true:false)<<std::endl;
		if(!name.empty())
		{
			//std::cout<<"1"<<std::endl;
			
			std::vector <std::string> code;
			if(usable)
			{
			//std::cout<<"2"<<std::endl;
				std::stringstream exec_str;
				//exec_str<<"$PBS_EXEC/bin/qmgr -c \"list node "<<name<<"\" | grep Qlist";
				exec_str<<"$PBS_EXEC/bin/pbsnodes -v "<<name;
				FILE * fp=popen(exec_str.str().c_str(),"r");
				if(!fp)
				{
					std::cout<<"cant read command output "<<exec_str.str()<<std::endl;
					//exit(-1);
				}
				char buff[1024];
				//size_t length,read;
				while(fgets(buff,1024,fp))
				{	
					std::string s(buff);
					//Util::trim(s);
					if(s.size()>1)
						s.erase(s.end()-1);
					unsigned int pos=0;
					if((pos=s.find("resources_available.Qlist = "))<s.size())
					{
						s.erase(0,sizeof(char)*(pos+strlen("resources_available.Qlist = ")));
					//std::cout<<" qlist "<<s<<" "<<s<<std::endl;
						code=Util::split(s,",");
						//code.push_back(s);
						/*for(unsigned int i=0;i<code.size();i++)
							std::cout<<code[i]<<std::endl;*/
					}
					else if((pos=s.find("resources_available.Qlist += "))<s.size())
					{
						s.erase(0,sizeof(char)*(pos+strlen("resources_available.Qlist += ")));
					//std::cout<<" qlist "<<s<<" "<<s<<std::endl;
						code=Util::split(s,",");
						//code.push_back(s);
						/*for(unsigned int i=0;i<code.size();i++)
							std::cout<<code[i]<<std::endl;*/
					}
				}
				pclose(fp);
			}
			//code.push_back("parallel");
			//code.push_back("np_longpar");
			/*std::cout<<"Nodo "<<name<<" car run queues: "; 
			for(unsigned int i=0;i<code.size();i++)
			{
				std::cout<<code[i]<<", ";
			}
			std::cout<<std::endl;*/
			//getchar();
			node.push_back(Node(name,numCores,numGPUs,numMICs,totMem,ghz,(usable==1?true:false),code));
		}
		/*else
		{
			break;
		}*/		
	}
	//getchar();
	input.close();
	return node;
}
#endif
