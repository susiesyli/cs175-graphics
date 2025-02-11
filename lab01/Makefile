LAB       = lab1

BREWPATH  = $(shell brew --prefix)
CXX       = $(shell fltk-config --cxx) -std=c++11
CXXFLAGS  = $(shell fltk-config --cxxflags) -I$(BREWPATH)/include
LDFLAGS   = $(shell fltk-config --ldflags --use-gl --use-images) -L$(BREWPATH)/lib
   
POSTBUILD = fltk-config --post # build .app folder for osx. (does nothing on pc)

$(LAB): % : main.o ply.o 
	$(CXX) $(LDFLAGS) $^ -o $@
	$(POSTBUILD) $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

clean:
	rm -rf $(LAB) $(LAB).app *.o *~ *.dSYM

