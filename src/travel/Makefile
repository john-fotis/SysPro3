CPP	= g++
FLAGS	= -g -c -Wall -std=c++0x
OBJS	= main.o MonitorInfo.o Request.o RequestRegistry.o WorkDistribution.o VirusRegistry.o $(EXTERN)/SocketLibrary.o $(EXTERN)/AppStandards.o $(EXTERN)/Messaging.o $(EXTERN)/LogHistory.o
LDLIBS	=
TARGET	= ../../travelMonitorClient
EXTERN	= ../common

$(TARGET): $(OBJS)
	$(CPP) -g $(OBJS) -o $@ $(LDLIBS)
	
%.o: %.cpp
	$(CPP) $(FLAGS) -c $< -o $@

clean:
	rm -f $(EXTERN)/*.o $(OBJS) $(TARGET)