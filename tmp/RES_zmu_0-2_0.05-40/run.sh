for (( i = 0; i < 41; i++ )); do
	xy=$(awk "BEGIN{print $i * 0.05}")
	./run -i ../DATA/movielens/movielens_3c.txt -M -l 40 -L 20 -r $xy | tee $xy
done
