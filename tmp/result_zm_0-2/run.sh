for (( i = 0; i < 41; i++ )); do
	xy=$(awk "BEGIN{print $i * 0.05}")
	./run -i ../DATA/movielens/movielens_3c.txt -Z -l 100 -z $xy | tee $xy
done
