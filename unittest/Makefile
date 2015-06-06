include ../config.mk
CXXFLAGS+=-I../core -I../style -I../hardware -I.. -Wall

NAME=unitTest
LIB=$(NAME).a
SRCS=$(wildcard *.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))

all: $(LIB)

unittest.cpp: unittest.h

unittest.h: Makefile $(subst unittest.cpp,,$(SRCS))
	@echo [HEADER] $@
	@echo '#ifndef _UNITTEST_H_' > $@
	@echo '#define _UNITTEST_H_' >> $@
	@echo >> $@
	@for i in $(subst unittest.cpp,,$(SRCS)); \
	do \
	    echo bool `basename $$i .cpp`UnitTest\(\)\;; \
	    echo const char *`basename $$i .cpp`GetName\(\)\;; \
	done >> $@
	@echo >> $@
	@echo 'struct test {' >> $@
	@echo '	const char *(*name)();' >> $@
	@echo '	bool (*test)();' >> $@
	@echo '} tests[] = {' >> $@
	@for i in $(subst unittest.cpp,,$(SRCS)); \
	do \
	    echo '	{'`basename $$i .cpp`GetName\, `basename $$i .cpp`UnitTest\}\,;\
	done >> $@
	@echo '	{0}' >> $@
	@echo '};' >> $@
	@echo >> $@
	@echo '#endif' >> $@

%.o: %.cpp
	@echo "[CXX] $@"
	@$(CXX) -c -o $@ $< $(CXXFLAGS)

$(LIB): $(OBJS)
	@echo [LIB] $@
	@$(AR) $@ $^
	@$(RANLIB) $@

clean:
	@echo [CLEAN]
	@$(RM) $(OBJS) unittest.h $(LIB)
