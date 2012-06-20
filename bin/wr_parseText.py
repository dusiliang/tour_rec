#!/usr/bin/python
import simplejson as json
import sys
import subprocess

reload(sys)
sys.setdefaultencoding('utf-8')

PROC = './parse_one'

def parse_text_line(line):
	global PROC
	unparse = json.loads(line)
	text = unparse['text']
	p = subprocess.Popen([PROC, text], stdout=subprocess.PIPE)
	parsed = p.stdout.readlines()[0]
	one_str = '{"id":"%s","user_id":"%s","user_name":"%s","location":"%s","words":"%s"}' % \
			(unparse["id"], unparse["user_id"], unparse["user_name"], unparse["location"], parsed)
	print one_str

def parse_text(inputfile, outputfile):
	global PROC
	num = 0
	out_str = []
	ifile = open(inputfile)
	for line in ifile:
		try:
			unparse = json.loads(line)
			text = '"%s"' % unparse['text']
			p = subprocess.Popen([PROC, text], stdout=subprocess.PIPE)
			parsed = p.stdout.readlines()[0]
			one_str = '{"id":"%s","user_id":"%s","user_name":"%s","location":"%s","words":"%s"}' % \
					(unparse["id"], unparse["user_id"], unparse["user_name"], unparse["location"], parsed)
			num += 1
			out_str.append(one_str)
		except:
			print num
			continue
	ifile.close()
	ofile = open(outputfile, 'w')
	ofile.writelines(out_str)
	ofile.close()

if __name__ == '__main__':
	if len(sys.argv) == 3:
		parse_text(sys.argv[1], sys.argv[2])
	elif len(sys.argv) == 2:
		parse_text_line(sys.argv[1])
