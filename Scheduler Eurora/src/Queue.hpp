/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                   File: Queue.hpp                                                    *
*                                                                                                                      *
*                                                                                                                      *
 **********************************************************************************************************************/

#ifndef Def_Queue
#define Def_Queue
#include <string>
#include <vector>
class Queue
{
	private:
		std::string _id;
		int _priority;
		bool _enabled;
		std::vector<int> _usableNodes;
		std::vector<std::string> _defaultChunk;
	public:
		int getMaxMinutesToWait()
		{
			//std::cout<<"MAX WAIT di "<<_id<<" = "<<86484-84*_priority<<std::endl;
			return 86484-84*_priority;
		}
		std::string getId(){return _id;}
		int getPriority(){return _priority;}
		bool isEnabled(){return _enabled;}
		std::vector<int> getUsableNodes(){return _usableNodes;}

		std::vector<std::string> getDefaultChunk()
		{
			return _defaultChunk;
		}

		void setPriority(int w){_priority=w;}
		void setId(std::string id){_id=id;}
		void setEnabled(bool e){_enabled=e;}
		void setUsableNodes(std::vector<int> un){_usableNodes=un;}
		void setDefaultChunk(std::vector<std::string> dc ){_defaultChunk=dc;}
		Queue(std::string id,int p,bool e,std::vector<int> u,std::vector<std::string> dc )
		{
			setId(id);
			setPriority(p);
			setEnabled(e);
			setUsableNodes(u);
			setDefaultChunk(dc);
		}
};
#endif
