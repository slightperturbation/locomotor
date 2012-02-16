#!/usr/bin/python
import MySQLdb
import os

def verifyDB():
	conn = MySQLdb.connect( host = '127.0.0.1', port = 9999, passwd = 'sm4ck3r', db = 'evocomp' )
	cursor = conn.cursor()
	cursor.execute( "SELECT VERSION()" )
	row = cursor.fetchone()
	print "got version: ", row[0]
	cursor.close()
	conn.close()
	
def harnessStats():
	conn = MySQLdb.connect( host = '127.0.0.1', port = 9999, passwd = 'sm4ck3r', db = 'evocomp' )
	cursor = conn.cursor()
	cursor.execute( """
	select ev.harness_generations as harnessGens, MAX(g.best_raw_fitness) as bestFit, AVG(g.best_raw_fitness) as meanFit, STDDEV(g.best_raw_fitness) as sdFit
	from generation as g, trial as t, experiment as ex, evolve as ev 
	where g.trial_id = t.id
	  and t.finish_time IS NOT NULL 
	  and t.experiment_id IN ( 102, 103, 104, 105, 106 )  
	  and t.experiment_id = ex.id
	  and ex.evolve_id = ev.id
	group by g.trial_id
	order by harness_generations DESC
	""")
	rows = cursor.fetchall()
	print "harnessGens\tbestFit\tmeanFit\tsdFit"
	for row in rows:
		print "%s\t%s\t%s\t%s" % (row[0], row[1], row[2], row[3] )
	cursor.close()
	conn.close()
	


if __name__ == '__main__':
	#print( "Start" )
	#os.system("ssh -L9999:127.0.0.1:3306 -N -t -x evocomp.org &")
	harnessStats()
	#print( "done." )	