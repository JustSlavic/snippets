latex_garbage = doc/report.aux doc/report.out \
	doc/report.log doc/report.run.xml doc/report.toc \
	doc/report.blg doc/report.bcf doc/report.bbl

compile : main.c
	mpicc main.c -o m -std=c99 -fopenmp

display : display.py
	python3 display.py

doc : doc/report.tex
	cd doc && pdflatex report

clean : 
	rm $(latex_garbage) m
