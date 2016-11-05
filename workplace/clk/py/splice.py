#!/usr/bin/python
#coding=UTF-8

import os

clk_dir = "/Users/gaokang/Documents/gps/ROCKET/workplace/clk/"

rin_obs_list_path = clk_dir+"clk.rnxlist"

rin_obs_list_new_path = clk_dir+"splice_clk.rnxlist"

reader = open( rin_obs_list_path, "r" )

writer = open( rin_obs_list_new_path, "w+")

obs_file_path = reader.readline()

while obs_file_path :
   obs_file_path = obs_file_path.strip('\n')
   dir_, file_name = os.path.split(obs_file_path);
   cmd_string = 'teqc -st 20111001001000 -e 20111001002000 %s > %s'%(obs_file_path,'./splice_data/splice_'+file_name)
   os.system(cmd_string);
   print cmd_string
   writer.write( clk_dir+'splice_data/splice_'+file_name+'\n')
   obs_file_path = reader.readline()

reader.close()
writer.close()


