include Makefile.inc
PATH := ../gcc/arm-elf/bin:../gcc/libexec/gcc/arm-elf/4.0.2:$(PATH)
GNULIB	= ../gcc/lib/gcc/arm-elf/4.0.2
CFLAGS  = -c -fPIC -Wall -I. -Iinclude -IStringUtil -ITrackUtil -mcpu=arm920t -msoft-float $(OPTMZFLAGS)
# -g: include hooks for gdb
# -c: only compile
# -mcpu=arm920t: generate code for the 920t architecture
# -fpic: emit position-independent code
# -Wall: report all warnings


LDFLAGS = -init main -Map kernel.map -N  -T orex.ld -L$(GNULIB) -LLib

all:  Kernel.elf

#test:
#	make clean;g++ -O2 -DTEST -I. -Iinclude -ITrackUtil UI.c trainAPI.c TrainTask.c NameServer.c LockServer.c StringUtil/StringUtil.c TrackUtil/Reservation.c TrackUtil/Router.c TrackUtil/SwitchProfile.c TrackUtil/track_data.c TrackUtil/TrackInformation.c TrackUtil/TrackMonitor.c TrackUtil/TrainProfile.c -fpermissive -c;~/u++-5.7.0/bin/u++ -O2 -DTEST -I. -ITrackUtil *.o TestingPlatform.c TestingEnvironment.c TestingKernel.c -o TestingPlatform;./TestingPlatform
#	g++ -DTEST -I. -Iinclude -ITrackUtil UI.c trainAPI.c TrainTask.c StringUtil/StringUtil.c TrackUtil/Reservation.c TrackUtil/Router.c TrackUtil/SwitchProfile.c TrackUtil/track_data.c TrackUtil/TrackInformation.c TrackUtil/TrackMonitor.c TrackUtil/TrainProfile.c -fpermissive TestingEnvironment.c TestingInput.c -o TestingInput;
#	g++ -DTEST -I. -Iinclude -ITrackUtil UI.c trainAPI.c TrainTask.c StringUtil/StringUtil.c TrackUtil/Reservation.c TrackUtil/Router.c TrackUtil/SwitchProfile.c TrackUtil/track_data.c TrackUtil/TrackInformation.c TrackUtil/TrackMonitor.c TrackUtil/TrainProfile.c -fpermissive TestingEnvironment.c TestingTimer.c -o TestingTimer;

TrackUtil/track_data.o TrackUtil/TrackInformation.o TrackUtil/SwitchProfile.o TrackUtil/TrainProfile.o TrackUtil/TrackMonitor.o TrackUtil/Router.o TrackUtil/Reservation.o:
	cd TrackUtil; $(MAKE) $(MFLAGS)

Log.s: Log.c Log.h TaskDescriptor.h include/StringUtil.h
	$(XCC) -S $(CFLAGS) Log.c

Log.o: Log.s
	$(AS) $(ASFLAGS) -o Log.o Log.s

StringUtil/StringUtil.o:
	cd StringUtil; $(MAKE) $(MFLAGS)

trainAPI.s: trainAPI.c trainAPI.h
	$(XCC) -S $(CFLAGS) trainAPI.c

trainAPI.o: trainAPI.s
	$(AS) $(ASFLAGS) -o trainAPI.o trainAPI.s

TrainTask.s: TrainTask.c TrainTask.h
	$(XCC) -S $(CFLAGS) TrainTask.c

TrainTask.o: TrainTask.s
	$(AS) $(ASFLAGS) -o TrainTask.o TrainTask.s

UI.s: UI.c UI.h
	$(XCC) -S $(CFLAGS) UI.c

UI.o: UI.s
	$(AS) $(ASFLAGS) -o UI.o UI.s

IO.s: IO.c IO.h
	$(XCC) -S $(CFLAGS) IO.c

IO.o: IO.s
	$(AS) $(ASFLAGS) -o IO.o IO.s

timer.s: timer.c timer.h
	$(XCC) -S $(CFLAGS) timer.c

timer.o: timer.s
	$(AS) $(ASFLAGS) -o timer.o timer.s

Notifier.s: Notifier.c Notifier.h
	$(XCC) -S $(CFLAGS) Notifier.c

Notifier.o: Notifier.s
	$(AS) $(ASFLAGS) -o Notifier.o Notifier.s

ClockServer.s: ClockServer.c ClockServer.h
	$(XCC) -S $(CFLAGS) ClockServer.c

ClockServer.o: ClockServer.s
	$(AS) $(ASFLAGS) -o ClockServer.o ClockServer.s

LockServer.s: LockServer.c LockServer.h
	$(XCC) -S $(CFLAGS) LockServer.c

LockServer.o: LockServer.s
	$(AS) $(ASFLAGS) -o LockServer.o LockServer.s

NameServer.s: NameServer.c NameServer.h
	$(XCC) -S $(CFLAGS) NameServer.c

NameServer.o: NameServer.s
	$(AS) $(ASFLAGS) -o NameServer.o NameServer.s

Interrupt.s: Interrupt.c Interrupt.h
	$(XCC) -S $(CFLAGS) Interrupt.c

Interrupt.o: Interrupt.s
	$(AS) $(ASFLAGS) -o Interrupt.o Interrupt.s

Tasks.s: Tasks.c Tasks.h timer.h
	$(XCC) -S $(CFLAGS) Tasks.c

Tasks.o: Tasks.s
	$(AS) $(ASFLAGS) -o Tasks.o Tasks.s

KernelTester.s: KernelTester.c KernelTester.h ContextSwitch.h
	$(XCC) -S $(CFLAGS) KernelTester.c

KernelTester.o: KernelTester.s
	$(AS) $(ASFLAGS) -o KernelTester.o KernelTester.s

TaskDescriptor.s: TaskDescriptor.c TaskDescriptor.h
	$(XCC) -S $(CFLAGS) TaskDescriptor.c

TaskDescriptor.o: TaskDescriptor.s
	$(AS) $(ASFLAGS) -o TaskDescriptor.o TaskDescriptor.s

Scheduler.s: Scheduler.c Scheduler.h TaskDescriptor.h
	$(XCC) -S $(CFLAGS) Scheduler.c

Scheduler.o: Scheduler.s
	$(AS) $(ASFLAGS) -o Scheduler.o Scheduler.s

Kernel.s: Kernel.c Kernel.h TaskDescriptor.h ContextSwitch.h Log.h
	$(XCC) -S $(CFLAGS) Kernel.c

Kernel.o: Kernel.s
	$(AS) $(ASFLAGS) -o Kernel.o Kernel.s

ContextSwitch.o: ContextSwitch.s ContextSwitch.h
	$(AS) $(ASFLAGS) -o ContextSwitch.o ContextSwitch.s

Kernel.elf: Kernel.o ContextSwitch.o Scheduler.o TaskDescriptor.o Tasks.o KernelTester.o Interrupt.o NameServer.o ClockServer.o LockServer.o Notifier.o timer.o IO.o UI.o TrainTask.o trainAPI.o StringUtil/StringUtil.o Log.o TrackUtil/track_data.o TrackUtil/TrackInformation.o TrackUtil/SwitchProfile.o TrackUtil/TrainProfile.o TrackUtil/TrackMonitor.o TrackUtil/Router.o TrackUtil/Reservation.o
	$(LD) $(LDFLAGS) -o $@ Kernel.o ContextSwitch.o Scheduler.o TaskDescriptor.o Tasks.o KernelTester.o Interrupt.o NameServer.o ClockServer.o LockServer.o Notifier.o timer.o IO.o UI.o TrainTask.o trainAPI.o StringUtil/StringUtil.o Log.o TrackUtil/track_data.o TrackUtil/TrackInformation.o TrackUtil/SwitchProfile.o TrackUtil/TrainProfile.o TrackUtil/TrackMonitor.o TrackUtil/Router.o TrackUtil/Reservation.o -lbwio -lgcc
	cp Kernel.elf /u/cs452/public/tftp/ARM/G2_Kernel.elf

clean:
	-rm -f *.elf *.o Kernel.s Scheduler.s Tasks.s TaskDescriptor.s NameServer.s Tasks.s Interrupt.s ClockServer.s Notifier.s timer.s IO.s UI.s TrainTask.s trainAPI.s *.map
	cd StringUtil; $(MAKE) clean
	cd TrackUtil; $(MAKE) clean
