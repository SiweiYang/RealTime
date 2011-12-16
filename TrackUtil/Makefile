include ../Makefile.inc

PATH := ../../gcc/arm-elf/bin:../../gcc/libexec/gcc/arm-elf/4.0.2:$(PATH)
CFLAGS  = -c -fPIC -Wall -I. -I.. -I../include -IReservUtil -mcpu=arm920t -msoft-float $(OPTMZFLAGS)

all:  Router.o TrackMonitor.o SwitchProfile.o TrainProfile.o Reservation.o TrackInformation.o track_data.o

# test: StringUtil.c ../include/StringUtil.h
# 	gcc -DTEST -I. TrackInformation.c SwitchProfile.c TrainProfile.c TrackMonitor.c track_data.c test.c -o test
# 	gcc -DTEST -I. Router.c TrackInformation.c SwitchProfile.c TrainProfile.c TrackMonitor.c track_data.c test_router.c -o test_router

Reservation.s: Reservation.c Reservation.h
	$(XCC) -S $(CFLAGS) Reservation.c

Reservation.o: Reservation.s
	$(AS) $(ASFLAGS) -o Reservation.o Reservation.s

TrainProfile.s: TrainProfile.c TrainProfile.h
	$(XCC) -S $(CFLAGS) TrainProfile.c

TrainProfile.o: TrainProfile.s
	$(AS) $(ASFLAGS) -o TrainProfile.o TrainProfile.s

SwitchProfile.s: SwitchProfile.c SwitchProfile.h
	$(XCC) -S $(CFLAGS) SwitchProfile.c

SwitchProfile.o: SwitchProfile.s
	$(AS) $(ASFLAGS) -o SwitchProfile.o SwitchProfile.s

Router.s: Router.c Router.h
	$(XCC) -S $(CFLAGS) Router.c

Router.o: Router.s
	$(AS) $(ASFLAGS) -o Router.o Router.s

TrackInformation.s: TrackInformation.c TrackInformation.h
	$(XCC) -S $(CFLAGS) TrackInformation.c

TrackInformation.o: TrackInformation.s
	$(AS) $(ASFLAGS) -o TrackInformation.o TrackInformation.s

track_data.s: track_data.c track_data.h track_node.h
	$(XCC) -S $(CFLAGS) track_data.c

track_data.o: track_data.s
	$(AS) $(ASFLAGS) -o track_data.o track_data.s

TrackMonitor.s: TrackMonitor.c TrackMonitor.h
	$(XCC) -S $(CFLAGS) TrackMonitor.c

TrackMonitor.o: TrackMonitor.s
	$(AS) $(ASFLAGS) -o TrackMonitor.o TrackMonitor.s

clean:
	-rm -f *.s *.o test
