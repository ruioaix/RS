for (( i = 0; i < 21; i++ )); do
	xy=$(awk "BEGIN{print $i * 0.05}")
	./run -i ../DATA/movielens/movielens_3c.txt -H -l 40 -L 20 -y $xy | tee $xy
done
