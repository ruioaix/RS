#!/bin/zsh
for (( i = 50; i < 100; i++ )); do
	./run -i ../DATA/movielens/movielens_3c.txt -mHUI -s $i >> result
done
