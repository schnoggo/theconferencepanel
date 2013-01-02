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
import math
import numpy

best_solution=[]
best_deviation=1000000
tri_base=math.tan(9)

"""Normalize Radial Segment Wedge Area
We'll be dealing in percentages, so total radius is 100 units.

See the post at  __ for more details.

Step through the possible positions for 5 dividers which define 6 segments of an arc.

We know that the no cell can be narrower than its outside neighbor
and
(because of that) that the innermost cell must be at least {total_radius}/number of cells wide

So our innermost ring is at least 100/6 (approx 17)
"""
def array_calc():
  global best_solution
  global best_deviation
  global tri_base
  current_solution=[]
 
  # A quirk of the Python "range" function is that it does not include the end value. 
  # So all these range ends are one beyonf the actual end.
  for r1 in range(17, 96):
    for r2 in range(r1, 97):
      for r3 in range(r2, 98):
        for r4 in range(r3, 99):
          for r5 in range(r4, 100):
            # calculate the array:
            current_solution=[r1,r2,r3,r4,r5,100]
            areas=[]
            neighbor_area=0
            for radius in current_solution:
              this_area=radius*radius*tri_base
              areas.append(this_area-neighbor_area)
              neighbor_area=this_area
            current_deviation=0
            mean=numpy.mean(areas)
            for area in areas:
              current_deviation=current_deviation + math.pow(area-mean,2)
            current_deviation = math.sqrt(current_deviation/6.0)
            if current_deviation < best_deviation:
              best_deviation = current_deviation
              best_solution = current_solution
              print current_deviation
  print 'best solution:'
  print best_solution



def main():
  args = sys.argv[1:]
  array_calc()
"""
  if not args:
    print 'usage: [--todir dir] logfile '
    sys.exit(1)

  todir = ''
  if args[0] == '--todir':
    todir = args[1]
    del args[0:2]
"""
if __name__ == '__main__':
  main()