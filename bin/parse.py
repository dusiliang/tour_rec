#!/usr/bin/python
import simplejson as json
import sys, os
import subprocess

PROC = "./a.out"

def parse_sop1(filename, output_file):
	global PROC
	fail_num = 0
	out_str = []

	ifile = open(filename)
	for line in ifile:
		try:
			contents = json.loads(line)
			out_str.append(subprocess.check_output([PROC, contents['text']]))
		except:
			fail_num += 1
			continue
	ifile.close()
	ofile = open(output_file, 'w')
	ofile.writelines(out_str)
	ofile.close()
	return fail_num

if __name__ == '__main__':
	if len(sys.argv) == 3:
		parse_sop1(sys.argv[1], sys.argv[2])
	else:
		pass
