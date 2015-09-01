for (( i = 8; i < 21; i++ )); do
	xy=$(awk "BEGIN{print $i * 0.25}")
	./run -i ../DATA/movielens/movielens_3c.txt -M -l 100 -r $xy | tee $xy
done
