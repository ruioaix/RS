for (( i = 0; i < 21; i++ )); do
	xy=$(awk "BEGIN{print $i * 0.05}")
	./run -i ../DATA/movielens/movielens_3c.txt -W -l 40 -L 20 -w $xy | tee $xy
done
