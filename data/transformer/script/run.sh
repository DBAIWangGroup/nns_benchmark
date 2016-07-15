
../src/compute_groundtruth -f ../../audio/audio.hdf5 -k 20


../src/load_from_hdf -f ../../audio/audio.hdf5 -n groundtruth -o audio_groundtruth.txt -t txt -y i

