for data in "audio"
do

data_path=../index/${data}/${data}_base_NP.fvecs
coarse_path=../index/${data}/${data}_coarse.dat
fine_path=../index/${data}/${data}_fine.dat
indice_path=../index/${data}
coarse_bin_path=../index/${data}/${data}_coarse.bin
n=53387
dim=192
use_residual=0

../src/build_master/indexer_launcher \
--threads_count=1 \
--multiplicity=2 \
--points_file=${data_path} \
--coarse_vocabs_file=${coarse_path} \
--fine_vocabs_file=${fine_path} \
--input_point_type="FVEC" \
--points_count=$n \
--space_dim=$dim \
--files_prefix=${indice_path}/ \
--coarse_quantization_file=${coarse_bin_path} \
--metainfo_file="fake1.txt" \
--build_coarse 0 \
--use_residuals  ${use_residual} \

done
