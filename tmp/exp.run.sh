for data in 1.movie.finl 2.book.finl 3.jester.finl 4.foods.finl
do
	./run -i ../DATA/zmdata/$data -m -l 200 | tee ${data}_res_m
	./run -i ../DATA/zmdata/$data -a -l 200 | tee ${data}_res_a
	./run -i ../DATA/zmdata/$data -U -l 200 | tee ${data}_res_U
	./run -i ../DATA/zmdata/$data -I -l 200 | tee ${data}_res_I
done
