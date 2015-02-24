#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#  Ugly script that checks for recursive function calls in shell scripts.
#
from __future__ import absolute_import
from __future__ import unicode_literals, division, generators
from __future__ import print_function, nested_scopes, with_statement

import argparse
import os
import sys
import re



def iread_func_names ( infiles ):
   re_funcdef = re.compile (
      r'^\s*(?P<name>[a-z_0-9A-Z]+)\s*[(][)]\s*[{](?:\s*(?P<code>.*\S)?\s*(?P<eof>[;]\s*[}]\s*)|.*)$'
   )

   for infile in infiles:
      for linom, line in enumerate(infile):
         rsline = line.rstrip()

         fmatch = re_funcdef.match(rsline)
         if fmatch is not None:
            fname, fcode, fend = fmatch.groups()
            yield ( infile.name, linom+1, fmatch.group('name') )
# --- end of iread_func_names (...) ---


def main ( exe, argv ):
   parser = argparse.ArgumentParser(prog=exe)
   parser.add_argument ( 'infiles', nargs='*', type=argparse.FileType('r') )
   parser.add_argument (
      '-s', '--sort', dest='want_sort', default=False, action='store_true'
   )
   parser.add_argument (
      '-F', '--show-src', dest='want_full_output',
      default=False, action='store_true'
   )

   config = parser.parse_args ( argv )

   outstream = sys.stdout
   writes    = outstream.write
   outformat = '{0} {1:d} {2}\n' if config.want_full_output else '{2}\n'

   if config.want_sort:
      func_defs = sorted (
         iread_func_names ( config.infiles ), key=lambda x: x[-1]
      )
   else:
      func_defs = iread_func_names ( config.infiles )

   for srcfile, lino, func_name in func_defs:
      writes ( outformat.format ( srcfile, lino, func_name ) )

   return os.EX_OK


if __name__ == '__main__':
   sys.exit ( main ( sys.argv[0], sys.argv[1:] ) )
