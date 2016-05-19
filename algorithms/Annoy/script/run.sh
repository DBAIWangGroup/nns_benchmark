
mkdir ../index 
sh ./indexer.sh ../../../data/audio_base.fvecs 50 ../index/audio.50tree ../index/index.txt


mkdir ../result

sh ./searcher.sh ../index/audio.50tree ../../../data/audio_query.fvecs ../../../data/audio_groundtruth.ivecs 20 ../result/audio_50tree.txt
