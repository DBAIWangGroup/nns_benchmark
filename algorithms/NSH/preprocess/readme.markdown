# NEIGHBOR-SENSITIVE HASHING

Neighbor-Sensitive Hashing is a state-of-the-art approximate k-Nearest Neighbor
algorithm for high-dimensional objects which are represetned as vectors of
hundreds of dimensions. Our work is based on a seemingly counter-intuitive (but
effective) idea that increasing the distances between nearby objects in
hashed-space (or equivanlently, Hamming space) results in hash functions that
are only neighbor-sensitive. Our technique may not be an optimal choice if users
are interested in sorting all data objects in a database in terms of their
distances from a query. However, our techinique is greatly successful at
distinguishing k-Nearest Neighbor objects when the value of k is small enough
compared to the number of the total objects in a database.

Our work has shown superior performances over other state-of-the-art techniques
such as Spectral Hashing, Spherical Hashing, Complementary Projection Hashing,
and so on, according to our comprehensive experiments not only including
"codesize vs. recall" but also "latency vs. recall"; for search latency measure,
we used a state-of-the-art Hamming search algorithm Multi-Index Hashing with
those hashing algorithms.

http://www-personal.umich.edu/~pyongjoo/#nsh
