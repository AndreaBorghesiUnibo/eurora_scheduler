/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                    File: Util.hpp                                                    *
*                                                                                                                      *
*                                                                                                                      *
 **********************************************************************************************************************/

#ifndef Def_Util
#define Def_Util
#include "Job.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#include <math.h>
#include <sstream>
#include <time.h> 
#include <algorithm>
//#include "chrono.hpp"
//#include "boost/date_time/local_time/local_time.hpp"

//using namespace std;

class Util
{
private:
	static time_t strTimeToTime(std::string str)
	{
		time_t t=0;
		std::vector<std::string> data=split(str,":");
		if(data.size()>3 || data.size()==0)
		{
			std::cout<<"ERROR Util::strHHMMToTime( "<<str<<" ) wrong format: Format accepted HH:MM"<<std::endl;
		} 
		else
		{
			//t+=std::atoi(data[0].c_str())*60*60+std::atoi(data[0].c_str())*60;
			for(int i=data.size()-1;i>=0;i--)
			{
				t+=std::atoi(data[i].c_str())*pow(60,data.size()-1-i);
			}
		}
		return t;
	}
public:


	static void trim(std::string& s)
	{
		
		if(s.size()>1)	
		{
			while(s[0]==' ' || s[0]=='\t')
			{
				s.erase(0,1);
			}
		}
		//s.erase(s.find_first_not_of(' ')+1);
	}

	static std::string removeChar(std::string origin,char del)
	{
		std::string res=origin;
		int count=0;
		for(unsigned int i=0;i<origin.size();i++)
		{
			if(origin[i]!=del)
			{
				res[count++]=origin[i];
			}
		}
		res[count]='\0';
		return res;
	}
	
	static int min(int a,int b)
	{
		return (a<b?a:b);
	}
	
	static int max(int a,int b)
	{
		return (a>b?a:b);
	}
	static std::vector<std::string> split(std::string str, std::string del)
	{
		std::vector<std::string> res;
		int end=0;
		while((end=str.find(del,0))>=0)
		{
			res.push_back(str.substr(0,end));
			str=str.substr(end+del.size(),str.size());
		}
		if(str.size()>0)
			res.push_back(str);
		return res;
	}
	
	static std::string timeHHMMToStr(time_t t)
	{
		std::stringstream ret("");
		int h=t/(60*60);
		int m=(t-h*60*60)/60;
		ret<<h<<":"<<m;
		return ret.str();
		
	}

	static std::string timeToDayofWeek(time_t t)
	{
		std::tm * ptm = std::localtime(&t);
		char buffer[32];
		
		std::strftime(buffer, 32, "%a", ptm);  
		std::string res(buffer);
		std::transform(res.begin(),res.end(),res.begin(),::toupper);
		return res;
	}

	static std::string timeToStrForAtNoSec(time_t t)
	{

		std::tm * ptm = std::localtime(&t);
		char buffer[32];
		
		std::strftime(buffer, 32, "%Y%m%d%H%M", ptm);  
		std::string res(buffer);
		return res;
	}

	static std::string timeToStrForAt(time_t t)
	{

		std::tm * ptm = std::localtime(&t);
		char buffer[32];
		
		std::strftime(buffer, 32, "%Y%m%d%H%M.%S", ptm);  
		std::string res(buffer);
		return res;
	}
		
	static std::string dateToStrNoDel(time_t t)
	{
		std::tm * ptm = std::localtime(&t);
		char buffer[32];
		
		std::strftime(buffer, 32, "%Y%m%d", ptm);  
		std::string res(buffer);
		return res;
		
	}

	static std::string timeToStr(time_t t)
	{
		std::tm * ptm = std::localtime(&t);
		char buffer[32];
		
		std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);  
		std::string res(buffer);
		return res;
		
	}
	static time_t str2ToTime(std::string str) 
	{
		struct tm t;
		strptime(str.c_str(), "%c", &t);
		t.tm_isdst = -1;
		return mktime(&t) ;
	}
	static time_t strToTime(std::string str) 
	{
		struct tm t;
		strptime(str.c_str(), "%Y-%m-%d %H:%M:%S", &t);
		t.tm_isdst = -1;
		return mktime(&t) ;
		
	}
	
	static time_t strHHMMToTime(std::string str)
	{
		time_t t=0;
		std::vector<std::string> data=split(str,":");

		if(data.size()!=2)
		{
			std::cout<<"ERROR Util::strHHMMToTime( "<<str<<" ) wrong format: Format accepted HH:MM"<<std::endl;
		} 
		else
		{
			t+=std::atoi(data[0].c_str())*60*60+std::atoi(data[1].c_str())*60;
			/*for(int i=data.size()-1;i>=0;i--)
			{
			t+=std::atoi(data[i].c_str())*pow(60,data.size()-1-i);
			}*/
		}
		return t;
	}

	static time_t strHHMMSSToTime(std::string str)
	{
		time_t t=0;
		std::vector<std::string> data=split(str,":");

		if(data.size()!=3)
		{
			std::cout<<"ERROR Util::strHHMMSSToTime( "<<str<<" ) wrong format: Format accepted HH:MM:SS"<<std::endl;
		} 
		else
		{
			t+=std::atoi(data[0].c_str())*60*60+std::atoi(data[1].c_str())*60+std::atoi(data[2].c_str());
			/*for(int i=data.size()-1;i>=0;i--)
			{
			t+=std::atoi(data[i].c_str())*pow(60,data.size()-1-i);
			}*/
		}
		return t;
	}
};
#endif
