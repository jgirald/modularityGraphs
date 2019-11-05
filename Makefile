all : modularityGraphs

clean :
	rm modularityGraphs

%: %.cpp
	g++ $< -o $@
