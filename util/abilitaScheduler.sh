#!/bin/bash 

echo "premi 1 per abilitare lo scheduler PBS"
echo "premi 2 per abilitare lo scheduler Model26"
read tasto

if [ "$tasto" = "1" ] ;then
	qmgr -c "set hook newjob enabled = false"
	qmgr -c "set hook updatejob enabled = false"
	qmgr -c "set hook terminatejob enabled = false"
	qmgr -c "set hook newreservation enabled = false"
	qmgr -c "set server scheduling = true"
	/etc/init.d/pbs restart
fi
if [ "$tasto" = "2" ] ;then
	qmgr -c "set hook newjob enabled = true"
	qmgr -c "set hook updatejob enabled = true"
	qmgr -c "set hook terminatejob enabled = true"
	qmgr -c "set hook newreservation enabled = true"
	qmgr -c "set server scheduling = false"
	/etc/init.d/pbs restart
fi
