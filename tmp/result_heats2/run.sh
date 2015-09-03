for (( i = 0; i < 9; i++ )); do
	xy=$(awk "BEGIN{print $i * 0.25}")
	for (( j = 0; j < 9; j++ )); do
		tk=$(awk "BEGIN{print $j * 0.25}")
		./run -i ../DATA/movielens/movielens_3c.txt -B -l 20 -c $xy -e $tk | tee ${xy}_$tk
	done
done
