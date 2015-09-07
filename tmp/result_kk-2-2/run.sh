for (( i = -8; i < 9; i++ )); do
	xy=$(awk "BEGIN{print $i * 0.25}")
	for (( j = -8; j < 9; j++ )); do
		tk=$(awk "BEGIN{print $j * 0.25}")
		./run -i ../DATA/movielens/movielens_3c.txt -Q -l 20 -f $xy -j $tk | tee res_${xy}_$tk
	done
done
