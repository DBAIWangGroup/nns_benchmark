
k=20
ntree=50

mkdir ../index 
sh ./indexer.sh ../../../data/audio_base.fvecs ${ntree} ../index/audio.${ntree}tree ../index/index_${ntree}.txt

mkdir ../result

sh ./searcher.sh ../index/audio.${ntree}tree ../../../data/audio_query.fvecs ../../../data/audio_groundtruth.fvecs ${k} ../result/audio_${ntree}tree.txt

