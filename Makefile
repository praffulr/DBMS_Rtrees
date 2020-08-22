sampleobjects = buffer_manager.o file_manager.o sample_run.o
mainobjects = buffer_manager.o file_manager.o rtree.o

rtree : $(mainobjects)
	g++ -std=c++11 -o rtree $(mainobjects)

rtree.o : rtree.cpp
	g++ -std=c++11 -c rtree.cpp

sample_run : $(sampleobjects)
	g++ -std=c++11 -o sample_run $(sampleobjects)

sample_run.o : sample_run.cpp
	g++ -std=c++11 -c sample_run.cpp

buffer_manager.o : buffer_manager.cpp
	g++ -std=c++11 -c buffer_manager.cpp

file_manager.o : file_manager.cpp
	g++ -std=c++11 -c file_manager.cpp

clean :
	rm -f *.o
	rm -f rtree
	rm -f rtree.txt
	clear
