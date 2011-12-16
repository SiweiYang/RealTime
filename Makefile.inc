XCC     = gcc
AS	    = as
LD      = ld
ASFLAGS	= -mcpu=arm920t -mapcs-32
OPTMZFLAGS = -O1
# OPTMZFLAGS = -O1 -fthread-jumps -falign-functions -falign-jumps -falign-loops -falign-labels -fcaller-saves -fcrossjumping -fcse-follow-jumps -fcse-skip-blocks -fdelete-null-pointer-checks -fexpensive-optimizations -fgcse -fgcse-lm -foptimize-sibling-calls -fpeephole2 -fregmove -freorder-blocks -freorder-functions -frerun-cse-after-loop -fsched-interblock -fsched-spec -fschedule-insns -fschedule-insns2 -fstrict-aliasing -ftree-pre -finline-functions -funswitch-loops -fgcse-after-reload -ftree-vectorize
# OPTMZFLAGS = -O1 -finline-small-functions -findirect-inlining -fipa-sra -fpartial-inlining -fdevirtualize -fstrict-overflow #bad option
# OPTMZFLAGS = -O1 -ftree-switch-conversion -ftree-tail-merge -ftree-vrp #bad option
# OPTMZFLAGS = -O1 -fpredictive-commoning -fipa-cp-clone #bad option
# -mapcs: always generate a complete stack frame
