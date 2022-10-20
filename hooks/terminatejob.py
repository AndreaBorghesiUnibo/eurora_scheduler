#!/usr/bin/env python
import pbs
import sys
ins=open("/etc/environment","r")
for line in ins:
	if line.startswith("PBS_EXEC"):
		pbsExec=line.split("=")[1].strip()
ins.close()
my_paths = [pbsExec+'python/lib/python25.zip',
		pbsExec+'python/lib/python2.5',
		pbsExec+'python/lib/python2.5/plat_linux2',
		pbsExec+'python/lib/python2.5/lib-tk',
		pbsExec+'python/lib/python2.5/lib-dnyload',
		pbsExec+'python/lib/python2.5/site-packages']

for my_path in my_paths:
	if my_path not in sys.path:
		sys.path.append(my_path)
import socket
import os
import time

try:
	e=pbs.event()
	j=e.job
	s=socket.socket()
	host='server.localdomain'
	port=4998
	s.connect((host,port))
	now = time.strftime("%c")
	cmd='Job Terminated;'+j.id+';'+now
	s.send(cmd)
	s.close()
except SystemExit:
	pass
except:
	pass
	#j.rerun()
	#e.reject(execute)
