for i in 1250 1500 1750 2000
do
	echo $i  
	./train $i model_init.txt seq_model_01.txt model_01.txt
	./train $i model_init.txt seq_model_02.txt model_02.txt
	./train $i model_init.txt seq_model_03.txt model_03.txt
	./train $i model_init.txt seq_model_04.txt model_04.txt
	./train $i model_init.txt seq_model_05.txt model_05.txt
	./test modellist.txt testing_data1.txt result.txt
done

