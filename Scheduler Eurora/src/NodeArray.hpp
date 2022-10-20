/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                 File: NodeArray.hpp                                                  *
*                                                                                                                      *
*                                                                                                                      *
 **********************************************************************************************************************/

#ifndef Def_NodeArray
#define Def_NodeArray
#include <vector>
#include "NodeReader.hpp"

class NodeArray
{
	private:
		std::vector<Node> _nodes;
		void checkQueuesAllocation();
	public:
		NodeArray(std::string fNode);
		Node& operator[] (int x) {return _nodes[x];}
		Node get(int i){return _nodes[i];}
		NodeArray();
		int size(){return _nodes.size();}
		int getNumberOfResources(){return 4;}
		void updateState();
		int numberOfActiveNodes()
		{
			int tot=0;
			for(unsigned int i=0;i<_nodes.size();i++)
			{
				if(_nodes[i].isUsable() && _nodes[i].isWorking())
					tot++;
			}
			return tot;
		}
		std::vector<bool> getStates()
		{
			std::vector<bool> res(size(),false);
			for(int i=0;i<size();i++)
			{
				if(_nodes[i].isUsable() && _nodes[i].isWorking())
				{
					res[i]=true;
				}
			}
			return res;
		}
};

inline NodeArray::NodeArray()
{
	NodeReader r;
	_nodes=r.readFromPBS();
}

inline NodeArray::NodeArray(std::string fNode)
{
	NodeReader r(fNode);
	_nodes=r.read();
}

inline void NodeArray::updateState()
{
	
	std::ofstream output("NodesState.log", std::ofstream::app);
	if (!output) 
	{
		std::cout << "ERROR: Could not open file \"NodesState.log\"" << std::endl;
			//getchar();
	}
	time_t now=0;
	time(&now);	
	for(unsigned int i=0;i<_nodes.size();i++)
	{
		_nodes[i].checkState();
		output<<now<<" - "<<_nodes[i].toString()<<std::endl;
	}
	output.close();
}





#endif
