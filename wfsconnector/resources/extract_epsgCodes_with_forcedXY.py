#! /usr/bin/env python
import re

# extract from 
# <tr><td class="nospace">?</td><td><code>EPSG:2044</code>
regex = re.compile(
	r"""^
	(.*\?).*
	(?P<code><code>(?!<del>).*</code>)
	""",re.VERBOSE)


with open("epsg_forced_xy.txt") as input:
	with open("codes_with_latlon_order.txt","a") as output:
		output.truncate()
		for line in input:
			match = regex.match(line)
			if match:
				#print(match)
				s = match.group("code")
				code = s[6:len(s)-7]
				print(code)
				output.write(code + "\n")
				


		