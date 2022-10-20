/**********************************************************************************************************************
*                                                                                                                      *
*                                   ERC Multitherman Lab @ DEI - University of Bologna                                 *
*                                                                                                                      *
*                                               Viale Pepoli 3/2 - 40136                                               *
*                                              Bologna - phone 0512092759                                              *
*                                                                                                                      *
*                                     Author: Thomas Bridi - thomas.bridi2@unibo.it                                    *
*                       Project: Tecniche di allocazione energy aware per il supercomputer Eurora                      *
*                                                   File: main26.cpp                                                   *
*                                                                                                                      *
*                                                                                                                      *
 ***********************************************************************************************************************/

#include "Scheduler4.hpp"
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <typeinfo>
#include <ctime>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Model26Util.hpp"
#include <thread>


//using namespace std;
JobArray totale;
void save(Scheduler4 * s,JobArray r)
{
	/*std::stringstream file("");
	file<<s->getDirectory();
	time_t now=0;
	time(&now);
	file<<"Model26_";
	r.write(file.str(),s->getInitialTime());*/
}

void newSchedule(Scheduler4 * s,std::string * stringaTempi)
{
	JobArray r;	
	
	int numModelli=1;
	int orderWT=0;
	double wtDelta=2;
	int orderNL=1;
	double numLateDelta=0;
	int orderMK=2;
	double makespanDelta=0;
	std::stringstream dest;
	time_t now=0;
	time(&now);
	//mkdir(s->getDirectory().c_str(),S_IRWXU|S_IRWXG|S_IRWXO);
	std::cout<<"Istante: "<<Util::timeToStr(now)<<std::endl;
	
	dest.str("");

	
	std::string temp;
	r=s->Solve(now,numModelli,orderWT,wtDelta,orderNL,numLateDelta,orderMK,makespanDelta,&temp);
	std::cout<<"Main solve finished"<<std::endl;
	
	time_t a1,a2;
	time(&a1);
	for(int j=0;j<r.size();j++)
	{
		bool toInsert=true;
		for(int i=0;i<totale.size();i++)
		{
			if(totale[i].getJobId().compare(r[j].getJobId())==0)
			{
				totale[i]=r[j];
				toInsert=false;
			}
		}
		if(toInsert)
		{	
			totale.Add(r[j]);
		}
	}
	std::cout<<"main pre execute"<<std::endl;
	s->execute(s->getInitialTime(),now);
	std::cout<<"main post execute"<<std::endl;
	r=s->getResult();
	std::cout<<"main get result"<<std::endl;
	for(int i=0;i<r.size();i++)
	{
		if(!r[i].isHolding() && !r[i].isScheduled())
			std::cout<<r[i].toString(now,s->getInitialTime())<<std::endl;
	}
	for(int i=0;i<r.size();i++)
	{
		if(!r[i].isHolding() && r[i].isScheduled())
			std::cout<<r[i].toString(now,s->getInitialTime())<<std::endl;
	}
	std::cout<<"main finishing"<<std::endl;
	
	time(&a2);
	std::stringstream totale;
	totale<<temp<<"+"<<a2-a1;
	*stringaTempi=totale.str();
}


int main(int argc, const char * argv[]) 
{
	std::string stringaTempi;
	std::cout<<"Model26_sched "<<Model26Util::getNumberOfNodes()<<std::endl;
	time_t istanteAttesa=0;
	time_t inizioEvento=0;
	time_t fineUpdate=0;
	time_t fineEvento=0;
	time_t now=0;
	time(&inizioEvento);
	time(&now);
	std::stringstream directory;
	directory<<now;	
	std::cout<<"Node file "<<argv[1]<<std::endl;
	std::string fNodes(argv[1]);
	std::cout<<"Creating scheduler"<<std::endl;
	Scheduler4 sched(fNodes,directory.str());
	//Scheduler4 sched(directory.str());
	std::cout<<"Updating job list"<<std::endl;
	sched.updateJobsList();
	std::cout<<"start Scheduling"<<std::endl;
	time(&fineUpdate);
	newSchedule(&sched,&stringaTempi);
	time(&fineEvento);
	std::cout<<"Tempi "<<fineEvento-inizioEvento<<";"<<fineUpdate-inizioEvento<<"+" <<fineEvento-fineUpdate<<";"<<stringaTempi<<std::endl;
	std::stringstream fileTempi;
	fileTempi<<"TempiOverhead_"<<now;
	std::ofstream output(fileTempi.str().c_str(), std::ofstream::app);
	if (!output) 
	{
		std::cout << "ERROR: Could not open file \"" << fileTempi.str() << "\"" << std::endl;
		getchar();
	}
	
	output<<"Total;JobUdate+Solution;WalltimeExceedCheck+QueueUpdate+NodeUpdate+IntervalsReadin+ReservationFeasibilityCheck+JobsFeasibilityCheck+JobsSubdivisions+ModelCreation+ModelSolve+ReservationSubdivision+ReservationsJobsSolve+WriteBack+StartRun"<<std::endl;
	output<<fineEvento-inizioEvento<<";"<<fineUpdate-inizioEvento<<"+" <<fineEvento-fineUpdate<<";"<<stringaTempi<<std::endl;
	output.close();
	
	unsigned int pos;
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int  n;

	/* First call to socket() function */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
	{
		perror("ERROR opening socket");
		exit(1);
	}
	/* Initialize socket structure */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = 4998;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	int yes=1;
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) == -1)
	{
		std::cout<<"setsockopt error"<<std::endl;
	}
 
	/* Now bind the host address using bind() call.*/
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{
		 perror("ERROR on binding");
		 exit(1);
	}

	/* Now start listening for the clients, here process will
	* go in sleep mode and will wait for the incoming connection
	*/
	listen(sockfd,5);

	while(true)
	{
		
		time(&istanteAttesa);
		std::cout<<"Attesa nuovo evento alle "<<Util::timeToStr(istanteAttesa)<<std::endl;
		/* Accept actual connection from the client */
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,&clilen);
		if (newsockfd < 0) 
		{
			perror("ERROR on accept");	
			exit(1);
		}

		/* If connection is established then start communicating */
		bzero(buffer,256);
		n = read( newsockfd,buffer,255 );
		if (n < 0)
		{
			perror("ERROR reading from socket");
			exit(1);
		}
		std::string s(buffer);
		std::cout<<"Message: "<<s<<std::endl;
		
		//close(sockfd);
		close(newsockfd);

		
		time(&inizioEvento);
		std::cout<<"Arrivo nuovo evento alle "<<Util::timeToStr(inizioEvento)<<std::endl;
		if((pos=s.find("New Job"))<s.size())
		{
			//std::vector<std::string> params=Util::split(s,";");
			bool toUpdate=sched.updateJobsList();
			//toUpdate=true;
			time(&fineUpdate);	
			if(toUpdate)
			{
				newSchedule(&sched,&stringaTempi);
			}
			else
			{
				std::cout<<"########################## SCHEDULING EVITATO ##########################"<<std::endl;
				std::string s("0+0+0+0+0+0+0+0+0+0+0+0+0");
				stringaTempi=s;
			}
		}
		else if((pos=s.find("Job Terminated"))<s.size())
		{
			std::vector<std::string> params=Util::split(s,";");
			if(params.size()>=2)
			{
				bool toUpdateTer=sched.terminateJob(params[1]);
				bool toUpdateUpd=sched.updateJobsList();	
				time(&fineUpdate);
				if(toUpdateTer || toUpdateUpd)
				{
					newSchedule(&sched,&stringaTempi);
				}
				else
				{
					std::cout<<"########################## SCHEDULING EVITATO ##########################"<<std::endl;
					std::string s("0+0+0+0+0+0+0+0+0+0+0+0+0");
					stringaTempi=s;
				}
			}
			else
			{
				std::cout<<"Job Terminated command error: "<<s<< " need 3 params"<<std::endl;
			}
			
	
		}
		else if((pos=s.find("Update Job"))<s.size())
		{
			std::vector<std::string> params=Util::split(s,";");
			
			if(params.size()==2)
			{
				sched.updateJob(params[1]);
			}
			else
			{
				std::cout<<"Update Job command error: "<<s<< " need 2 params"<<std::endl;
			}	
			sched.updateJobsList();
			time(&fineUpdate);
			newSchedule(&sched,&stringaTempi);
			
		}
		else if((pos=s.find("New Reservation"))<s.size())
		{
			sched.updateJobsList();
			time(&fineUpdate);	
			newSchedule(&sched,&stringaTempi);
			
		}
		else if((pos=s.find("Start reservation"))<s.size())
		{
			sched.updateJobsList();
			time(&fineUpdate);
			newSchedule(&sched,&stringaTempi);
		}
		else if((pos=s.find("End reservation"))<s.size())
		{
			sched.updateJobsList();
			time(&fineUpdate);
			newSchedule(&sched,&stringaTempi);
		}
		else if((pos=s.find("Exit"))<s.size())
		{	
			time(&fineUpdate);
			close(sockfd);
			close(newsockfd);
			return 0;
		}
		else if((pos=s.find("Save"))<s.size())
		{	
			std::vector<std::string> params=Util::split(s,";");
			JobWriter r(params[1]);
			r.write(totale.toArray());
			time(&fineUpdate);
		}
		else
		{
			sched.updateJobsList();
			time(&fineUpdate);
			newSchedule(&sched,&stringaTempi);
		}
		
		time(&fineEvento);
		std::cout<<"Tempi "<<fineEvento-inizioEvento<<";"<<fineUpdate-inizioEvento<<"+" <<fineEvento-fineUpdate<<";"<<stringaTempi<<std::endl;
		std::stringstream fileTempi;
		fileTempi<<"TempiOverhead_"<<now;
		std::ofstream output(fileTempi.str().c_str(), std::ofstream::app);
		if (!output) 
		{
			std::cout << "ERROR: Could not open file \"" << fileTempi.str() << "\"" << std::endl;
			getchar();
		}
		output<<fineEvento-inizioEvento<<";"<<fineUpdate-inizioEvento<<"+" <<fineEvento-fineUpdate<<";"<<stringaTempi<<std::endl;
		output.close();
		
	}

	return 0;
}
