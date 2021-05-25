# ========== Run "make help" to see the available commands ==========
CODE	= src
SERVER	= $(CODE)/travel
CLIENT	= $(CODE)/monitor
COMMON	= $(CODE)/common
HEADER	= $(CLIENT)/include/*.hpp $(SERVER)/include/*.hpp include/*.hpp
SOURCE	= $(CLIENT)/*.cpp $(SERVER)/*.cpp $(COMMON)/*.cpp
OBJS	= $(CLIENT)/*.o $(SERVER)/*.o $(COMMON)/*.o
TARGET	= travelMonitorClient
TARGET2	= monitorServer
SCRIPTS	= create_infiles.sh

# Run Parameters
BUFFSZ	= 64
CBUFFSZ	= 10
NUMBER	= 9
BLOOMSZ	= 1000
INDIR	= input_dir/
THREADS	= 2
INFILE	= citizenRecordsFile
BINDIR	= bin/
LOGDIR	= logs/

all:
	$(MAKE) -C $(SERVER)
	$(MAKE) -C $(CLIENT)

$(TARGET):
	$(MAKE) -C $(SERVER)

$(TARGET2):
	$(MAKE) -C $(CLIENT)

clean:
	$(MAKE) clean -C $(SERVER)
	$(MAKE) clean -C $(CLIENT)

cleanFull:
	$(MAKE) clean -C $(SERVER)
	$(MAKE) clean -C $(CLIENT)
	rm -rf $(INDIR) $(BINDIR) $(LOGDIR)

count:
	wc -l -w $(SOURCE) $(HEADER) $(SCRIPTS)

run:
	./$(TARGET) -m $(NUMBER) -b $(BUFFSZ) -c $(CBUFFSZ) -s $(BLOOMSZ) -i $(INDIR) -t $(THREADS)

run2:
	./$(TARGET2) -p 100 -t 3 -b 64 -c 10 -s 1000 ./input_dir/AUSTRALIA/

scriptRun:
	./$(SCRIPTS) $(INFILE) $(INDIR) $(NUMBER)

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --show-reachable=yes --trace-children=yes --track-origins=yes ./$(TARGET) -m $(NUMBER) -b $(BUFFSZ) -s $(BLOOMSZ) -i $(INDIR)

help:
	@echo Options:
	@printf "make (all) %14s -- build $(TARGET) and $(TARGET2)\n"
	@printf "make $(TARGET) %6s -- build $(TARGET)\n"
	@printf "make $(TARGET2) %12s -- build $(TARGET2)\n"
	@printf "make clean %14s -- delete application\n"
	@printf "make cleanFull %10s -- delete application and its data\n"
	@printf "make count %14s -- project line and words accounting\n"
	@printf "make run %16s -- run $(TARGET) test\n"
	@printf "make scriptRun %10s -- run $(SCRIPTS) test\n"
	@printf "make valgrind %11s -- run $(TARGET) test with valgrind enabled\n"
	@printf "make help %15s -- view this help message\n"