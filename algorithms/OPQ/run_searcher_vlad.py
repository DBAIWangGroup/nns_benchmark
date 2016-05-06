#!/usr/bin/python
import os
import datetime

############# launch configuration #################

# folder to keep built binaries in
build_folder = './build_master'


# Folder with BigAnn base
bigann_root = '/sata/ResearchData/BigAnn'

# input query point type (BVEC or FVEC)
query_input_type = 'FVEC'

# Multi-1 or Multi-2 or Multi-4
multiplicity = 2

# prefix of all vocabs, coarse quantizations, etc.
prefix = 'vlad500K'

# dimension of input space
space_dim = 128

# coarse vocabs size
coarse_vocabs_size = 4096

# fine vocabs count
fine_vocabs_count = 8

# should we use residuals?
use_residuals = 1

# number of centroids handled in each subdimension
subspace_centroids_count = 4096

# queries file
queries_file = 'vlad1M_queries.fvecs'

# groundtruth file
gnd_file = 'vlad500K_groundtruth.ivecs'

# number of queries
queries_count = 1000

# number of neighbors to seek
neighbors_count = 10000

# should we rerank ?
do_rerank = 0

# postfix added by users to all multiindex files
user_added_postfix = ''

##################################################

multiplicity_extension = ''
if multiplicity == 1:
    multiplicity_extension = 'single'
if multiplicity == 2:
    multiplicity_extension = 'double'
if multiplicity == 4:
    multiplicity_extension = 'quad'

coarse_vocabs_filename = prefix + '_' + multiplicity_extension + '_' + str(coarse_vocabs_size) + '.dat'
fine_vocabs_filename = prefix + '_' + multiplicity_extension + '_' + str(coarse_vocabs_size) + '_' + str(fine_vocabs_count) + '.dat'
filename_prefix = prefix + '_' + multiplicity_extension + '_' + str(coarse_vocabs_size) + '_' + str(fine_vocabs_count) + user_added_postfix

launch_time = datetime.datetime.now().strftime("%I_%M_%S%p_%B_%d_%Y")
os.system('mkdir -p ' + build_folder + '/' + launch_time)
os.system('cp ' + build_folder + '/searcher_tester ' + build_folder + '/' + launch_time)
os.system('cp run_searcher_vlad.py ' + build_folder + '/' + launch_time)
report_filename = build_folder + '/' + launch_time + '/report'

launch_line = build_folder + '/' + launch_time + '/searcher_tester '
launch_line = launch_line + '--queries_file=' + bigann_root + '/bases/' + queries_file + ' '
launch_line = launch_line + '--groundtruth_file=' + bigann_root + '/gnd/' + gnd_file + ' '
launch_line = launch_line + '--coarse_vocabs_file=' + bigann_root + '/coarse_vocabs/' + coarse_vocabs_filename + ' '
launch_line = launch_line + '--fine_vocabs_file=' + bigann_root + '/fine_vocabs/' + fine_vocabs_filename + ' '
launch_line = launch_line + '--query_point_type=' + query_input_type + ' '
launch_line = launch_line + '--queries_count=' + str(queries_count) + ' '
launch_line = launch_line + '--neighbours_count=' + str(neighbors_count) + ' '
launch_line = launch_line + '--subspaces_centroids_count=' + str(subspace_centroids_count) + ' '
launch_line = launch_line + '--space_dim=' + str(space_dim) + ' '
launch_line = launch_line + '--index_files_prefix=' + bigann_root + '/indices/' + filename_prefix + ' '
launch_line = launch_line + '--report_file=' + report_filename + ' ' 
if use_residuals:
    launch_line = launch_line + '--use_residuals' + ' '
if do_rerank:
    launch_line = launch_line + '--do_rerank' + ' '

f = open(build_folder + '/' + launch_time + '/launch.sh', 'w')
f.write(launch_line)
f.close()
log_filename = prefix + '_' + multiplicity_extension + '_' + str(coarse_vocabs_size) + '_' + str(fine_vocabs_count) + '_' + str(neighbors_count) + '.txt'
os.system('nohup ' + launch_line + ' > ' + build_folder + '/' + launch_time + '/' + log_filename + ' &')
print 'Log file: ' + build_folder + '/' + launch_time + '/' + log_filename 
