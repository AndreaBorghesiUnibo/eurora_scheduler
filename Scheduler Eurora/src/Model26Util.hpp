/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                 File: Model26Util.hpp                                                *
*                                                                                                                      *
*                                                                                                                      *
 **********************************************************************************************************************/

#ifndef Def_Model26Util
#define Def_Model26Util
#include <string>
#include <iostream>
#include <sstream>

//using namespace std;

class Model26Util
{

public:

	static int getNumberOfNodes()
	{
		std::stringstream exec_str;
		exec_str<<"cat $MODEL26_HOME/Scheduler\\ Eurora/data/nodi | wc -l";
		FILE * fp=popen(exec_str.str().c_str(),"r");
		if(!fp)
		{
			std::cout<<"cant read command output getNumberOfNodes "<< exec_str.str()<<std::endl;
		}
		int result=0;
		char buff[1024];
		fgets(buff,1024,fp);
		result=atoi(buff);
		pclose(fp);
		return result;
	}
};
#endif
