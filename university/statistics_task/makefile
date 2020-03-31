all : hist doc

hist : src/main.m src/get_data.m data/ncalls.data data/durationcalls.data
	cd src && octave main.m && cd ..

doc : doc/main.tex data/hist1.data data/hist2.data
	cd doc && pdflatex main && cd ..