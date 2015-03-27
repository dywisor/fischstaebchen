#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#  Ugly script that checks for recursive function calls in shell scripts.
#
from __future__ import absolute_import
from __future__ import unicode_literals, division, generators
from __future__ import print_function, nested_scopes, with_statement

import sys
import re

def filter_escaped ( seq ):
   bs = chr(0x5c)

   blind = False
   for c in seq:
      if blind:
         blind = False
      elif c == bs:
         blind = True
      else:
         yield c

def readlines ( filepath ):
   with open ( filepath, 'rt' ) as fh:
      for linom, line in enumerate(fh):
         yield ( linom+1, line )

def main ( exe, argv ):
   re_funcdef = re.compile (
      r'^\s*(?P<name>[a-z_0-9A-Z]+)\s*[(][)]\s*[{](?:\s*(?P<code>.*\S)?\s*(?P<eof>[;]\s*[}]\s*)|.*)$'
   )

   outstream = sys.stdout
   writes    = outstream.write

   for infile in argv:
      fstack = []

      for lino, line in readlines(infile):
         rsline = line.rstrip()
         sline  = rsline.lstrip()

         fmatch = re_funcdef.match(rsline)
         if fmatch is not None:
            fname, fcode, fend = fmatch.groups()

            if fstack:
               writes (
                  '{}: {:d}: nested function: {}\n'.format ( infile, lino, fname )
               )

            if not fend:
               fstack.append ( fname )

            continue

         elif sline == '}':
            try:
               fstack.pop()
            except IndexError:
               writes (
                  '{}: {:d}: \'}}\' outside of function: {}\n'.format (
                     infile, lino, rsline
                  )
               )

            continue

         else:
            nopens  = sline.count('{')
            ncloses = sline.count('}')

            ndelta  = ncloses - nopens
            if ndelta > 0:
               for k in range(ndelta):
                  if fstack and fstack[-1] is None:
                     fstack.pop()
                  else:
                     writes (
                        '{}: {:d}: straw \'}}\': {}\n'.format ( infile, lino, rsline )
                     )

            elif ndelta < 0:
               for k in range(abs(ndelta)): fstack.append(None)
         # --

         for upper_func in filter ( None, fstack ):
            parts = sline.split(upper_func)
            if len(parts) < 2: continue

            lq = lqq = 0
            for k, r in enumerate(parts[1:]):
               l = parts[k]

               for c in filter_escaped(l):
                  if c == '"':
                     lqq += 1
                  elif c == "'":
                     lq += 1

               # assuming that function calls are never in a quoted expression:
               if (lq % 2) or (lqq % 2): continue

               # if lhs or rhs not empty: does not call %upper_func
               if l and l[-1].strip(): continue
               if r and r[0].strip(): continue

               writes (
                  '{}: {:d}: possible recursion of {}: {}\n'.format (
                     infile, lino, upper_func, sline
                  )
               )
            # --
         # --
# --- end of main (...) ---

if __name__ == '__main__':
   main ( sys.argv[0], sys.argv[1:] )
