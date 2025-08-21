LDFLAGS = -lwiringPi
INCLUDE = -I ./include
SRC = ir.cpp PulseController.cpp Ceiling.cpp Aircon.cpp

ir: $(SRC)
	$(CXX) $(INCLUDE) $^ -o $@ $(LDFLAGS)