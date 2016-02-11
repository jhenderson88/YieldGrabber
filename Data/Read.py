import re
import sys

print "Beginning reading from wget output..."

datecheck = re.compile('.*\d{4}-\d{2}-\d{2}.*')
elementcheck = re.compile('.*>\d+</sub><b>\w+<.*')
isotopecheck = re.compile('\s+[a-zA-z]+\d+\w+\s+')
yieldcheck = re.compile('.*\d+.\d+E\d+.*')
smallyieldcheck = re.compile('.*\d+.\d+E-\d+.*')
protoncheck = re.compile('<div class="ui-dt-c">\d</div>')
sourcecheck_Re = re.compile('.*Re surface.*')
sourcecheck_Ta = re.compile('.*Ta surface.*')
sourcecheck_TRILIS = re.compile('.*TRILIS.*')
sourcecheck_IGLIS= re.compile('.*IG-LIS.*')
sourcecheck_FEBIAD = re.compile('.*FEBIAD.*')

infile = open(sys.argv[1],'r')
outfile = open(sys.argv[2],'w');
flag = False
first = True
commentflag = False

outfile.write("Isotope	State	Intensity (pps)	Proton current	Ion source	Info\n")

for line in infile:
	if "records retrieved on" in line:
		flag = True
	if flag and "PrimeFaces.cw" in line:
		flag = False
	if flag and datecheck.match(line):
		date = re.search('\d{4}-\d{2}-\d{2}',line).group(0)
		if first:
			#outfile.write(date)
			#outfile.write('\t')
			first = False
	if flag and elementcheck.match(line):
		quickelement = re.search('\d+</sub><b>\w+',line).group(0)
		z = re.search('\d+',quickelement).group(0);
		element = re.search('(?<=>)\w+',quickelement).group(0)
		outfile.write(z)
		outfile.write(element)
		outfile.write('\t')
	if flag and isotopecheck.match(line):
		isotope = re.search('[a-zA-z]+\d+\w+',line).group(0)
		outfile.write(isotope)
		outfile.write('\t')
	if commentflag:
		try:
			comment = line
			comment = comment.strip()
			outfile.write(comment)
			outfile.write('\n')
		except:
			outfile.write(' \n')
		commentflag = False
	if flag and yieldcheck.match(line):
		yield_val = re.search('\d+.\d+E\d+',line).group(0)
		outfile.write(yield_val)
		outfile.write('\t')
		try:
			protontest = re.search('>\d+<',line).group(0)
		except:
			protontest = re.search('>\d+.\d+<',line).group(0)
		proton = re.search('(?<=>)\w+',protontest).group(0)
		outfile.write(proton)
		outfile.write('\t')
		ionsource = "Ion source"
		if sourcecheck_Re.match(line):
			ionsource = "Re surface"
		if sourcecheck_Ta.match(line):
			ionsource = "Ta surface"
		if sourcecheck_TRILIS.match(line):
			ionsource = "TRILIS"
		if sourcecheck_IGLIS.match(line):
			ionsource = "IG-LIS"
		if sourcecheck_FEBIAD.match(line):
			ionsource = "FEBIAD"
		outfile.write(ionsource)
		outfile.write('\t');
		commentflag = True
	if flag and smallyieldcheck.match(line):
		yield_val = re.search('\d+.\d+E-\d+',line).group(0)
		outfile.write(yield_val)
		outfile.write('\t')
		try:
			protontest = re.search('>\d+<',line).group(0)
		except:
			protontest = re.search('>\d+.\d+<',line).group(0)
		proton = re.search('(?<=>)\w+',protontest).group(0)
		outfile.write(proton)
		outfile.write('\t')
		ionsource = "Ion source"
		if sourcecheck_Re.match(line):
			ionsource = "Re surface"
		if sourcecheck_Ta.match(line):
			ionsource = "Ta surface"
		if sourcecheck_TRILIS.match(line):
			ionsource = "TRILIS"
		if sourcecheck_IGLIS.match(line):
			ionsource = "IG-LIS"
		if sourcecheck_FEBIAD.match(line):
			ionsource = "FEBIAD"
		outfile.write(ionsource)
		outfile.write('\t');
		commentflag = True
