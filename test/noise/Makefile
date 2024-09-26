run:
	@g++ -o 1 main.cpp ../../lib/noise.cpp
	@./1 > noise_data.txt
	@python3 plot.py

clean:
	@rm -f 1 noise_data.txt