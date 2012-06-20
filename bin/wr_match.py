#!/usr/bin/python
import simplejson as json
import sys

reload(sys)
sys.setdefaultencoding('utf-8')

def load_msg(sop2file, msg_list):
	ifile = open(sop2file)
	for line in ifile:
		de_line = json.loads(line)
		msg_list.append(de_line)
	ifile.close()

def load_line(pos1file, line_list):
	ifile = open(pos1file)
	for line in ifile:
		de_line = json.loads(line)
		line_list.append(de_line)
	ifile.close()

def is_ns(word):
	if word.find('/ns'):
		return True
	else:
		return False

def is_match(msg, line):
	all_words = msg['words'].split()
	keywords = line['product_keyword'].lstrip()
	pos = keywords.find(' ')
	if pos != -1:
		all_des = keywords[pos+1:].split()
	else:
		return False
	if len(all_des) == 0 or len(all_words) == 0:
		return False

	for word in all_words:
		if not is_ns(word):
			continue
		for des in all_des:
			if word.find(des) != -1:
				return True
	return False
			

def get_match_lines(msg, line_list, match_list):
	for line in line_list:
		if is_match(msg, line):
			match_list.append(line)

def append_result(msg, line, result_list):
	product = '"product_id":"%s","product_wid":"%s","product_url":"%s","product_description":"%s"' %\
			(line['product_id'], line['product_wid'], line['product_url'], line['product_description'])
	one_result = '{"id":"%s","user_id":"%s","user_name":"%s",%s}' % \
			(msg['id'], msg['user_id'], msg['user_name'], product)
	result_list.append(one_result)

def out_put(result_list, outfile):
	ofile = open(outfile, 'w')
	ofile.write('\n'.join(result_list))
	ofile.close()

def do_match(sop2file, pos1file, outfile):
	msg_list = []
	line_list = []
	result_list = []
	load_msg(sop2file, msg_list)
	load_line(pos1file, line_list)
	#print len(msg_list)
	#print len(line_list)

	for msg in msg_list:
		match_list = []
		get_match_lines(msg, line_list, match_list)
		if len(match_list) > 0:
			append_result(msg, match_list[0], result_list)
	out_put(result_list, outfile)

if __name__ == '__main__':
	if len(sys.argv) == 4:
		do_match(sys.argv[1], sys.argv[2], sys.argv[3])
	else:
		print 'usage:', sys.argv[0], ' sop2 pos1 out_file'
