include ../config.mk
DIRS := $(sort $(dir $(wildcard */*.cpp)))
LIBS := $(patsubst %/,%/application.so, $(DIRS))

CXXFLAGS += -Wall -I../core -I../style -I../ -I../hardware


all: $(LIBS)

%.o: %.cpp
	@echo "  [CXX] $@"
	$(CXX) $(CXXFLAGS) $(PIC) -c -o $@ $<

%.o: %.c
	@echo "  [CC]  $@"
	$(CC) $(CFLAGS) $(PIC) -c -o $@ $<

$(LIBS): $(subst .c,.o,$(subst .cpp,.o,$(shell find $(dir $(LIBS)) -name *.cpp -o -name *.c)))
	@echo "  [LD]  $@"
	$(MAKESO) $(SOLDFLAGS) -o $@ $(MODLIBS) $(subst .c,.o,$(subst .cpp,.o,$(shell find $(dir $(@)) -name *.cpp -o -name *.c)))

clean:
	@echo "  [CLEAN]"
	$(RM) $(shell find . -iname '*.o' -o -iname '*.so')
