#!/usr/bin/python
# Copyright 2013 Lon Koenig
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.

#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.

#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.



import sys
import re

best_solution=[]
best_deviation=100000

"""Normalize Radial Segment Wedge Area
We'll be dealing in percentages, so total radius is 100 units.

See the post at  __ for more details.

Step through the possible positions for 5 dividers which define 6 segments of an arc.

We know that the no cell can be narrower than its outside neighbor
and
(because of that) that the innermost cell must be at least {total_radius}/number of cells wide

So our innermost ring is at least 100/6 (approx 17)
"""

def extract_names(filename):
  """
  Given a file name for baby.html, returns a list starting with the year string
  followed by the name-rank strings in alphabetical order.
  ['2006', 'Aaliyah 91', Aaron 57', 'Abagail 895', ' ...]
  """
  # +++your code here+++
  # year is actually in the file name: "baby1990.html"
  names=[]
  ranks={}
  year_match = re.search(r'baby(\d\d\d\d).html', filename)
  if not year_match:
    # We didn't find a year, so we'll exit with an error message.
    sys.stderr.write('Couldn\'t find the year!\n')
    sys.exit(1)
  year = year_match.group(1)
  names.append(year)
  
  f = open(filename, 'rU')
  for line in f:   ## iterates over the lines of the file
    thisrec=re.search(r'<td>(\d+)</td><td>(\w+)</td>\<td>(\w+)</td>', line)
    if thisrec:
      boyname=thisrec.group(2)
      girlname=thisrec.group(3)
      rank=int(thisrec.group(1))
      if boyname not in ranks:
        ranks[boyname]={'boy':rank}
      else:
        if 'boy' not in ranks[boyname]:
          ranks[boyname]['boy']=rank
          
      if girlname not in ranks:
        ranks[girlname]={'girl':rank}
      else:
        if 'girl' not in ranks[girlname]:
          ranks[girlname]['girl']=rank
          
  f.close()
 
  for name in ranks.keys():
    this_line = name + '('
    this_line += ', '.join([ gender  + ': ' + str(ranks[name][gender]) for gender in ranks[name].keys() ])
    this_line += ')'
    names.append( this_line)
  return names



def main():
  # This command-line parsing code is provided.
  # Make a list of command line arguments, omitting the [0] element
  # which is the script itself.
  args = sys.argv[1:]

  if not args:
    #print 'usage: [--summaryfile] file [file ...]'
    #sys.exit(1)
    args =['baby1990.html']

  # Notice the summary flag and remove it from args if it is present.
  summary = False
  if args[0] == '--summaryfile':
    summary = True
    del args[0]

  # +++your code here+++
  # For each filename, get the names, then either print the text output
  # or write it to a summary file
  for filename in args:
    names = extract_names(filename)
    text = '\n'.join(names)
    print text
  
if __name__ == '__main__':
  main()
