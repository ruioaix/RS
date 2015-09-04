xy=0.75
for (( j = 0; j < 6; j++ )); do
	tk=$(awk "BEGIN{print $j * (-0.2)}")
	./run -i ../DATA/movielens/movielens_3c.txt -B -l 100 -c $xy -e $tk | tee ${xy}_$tk
done
