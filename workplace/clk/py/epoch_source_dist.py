#!/usr/bin/python
#coding=UTF-8

import os

source_file = open( 'sourcesData.txt', 'r' )
mat_file = open( 'sourcesMat.txt', 'w')

line = source_file.readline()

while line:
   line = line.strip( '\n' )

   ## find header
   if -1 != line.find( 'GPS' ) :
      mat_file.write( '\n' )
      mat_file.write( line +' ')
      continue

   ## print source number list
   split_dict = line.split( ' ' )
   mat_file.write(split_dict[1]+' ')

   line = source_file.readline()


source_file.close()
mat_file.close()

