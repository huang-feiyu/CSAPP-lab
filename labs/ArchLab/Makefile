####################################################
# Students' Makefile for the CS:APP Architecture Lab
####################################################

# Default team name and version number
TEAM = bovik
VERSION = 1

# Where are the different parts of the lab should be copied to when they
# are handed in.
HANDINDIR-PARTA = /afs/cs/academic/class/15349-s02/archlab/handin-parta
HANDINDIR-PARTB = /afs/cs/academic/class/15349-s02/archlab/handin-partb
HANDINDIR-PARTC = /afs/cs/academic/class/15349-s02/archlab/handin-partc

sim:
	(cd sim; make)

# Use this rule to hand in Part A ("make handin-parta")
handin-parta:
	cp sim/misc/sum.ys $(HANDINDIR-PARTA)/$(TEAM)-$(VERSION)-sum.ys
	cp sim/misc/rsum.ys $(HANDINDIR-PARTA)/$(TEAM)-$(VERSION)-rsum.ys
	cp sim/misc/copy.ys $(HANDINDIR-PARTA)/$(TEAM)-$(VERSION)-copy.ys

# Use this rule to handin Part B ("make handin-partb")
handin-partb:
	cp sim/seq/seq-full.hcl $(HANDINDIR-PARTB)/$(TEAM)-$(VERSION)-seq-full.hcl

# Use this rule to handin Part C ("make handin-partc")
handin-partc:
	cp sim/pipe/ncopy.ys $(HANDINDIR-PARTC)/$(TEAM)-$(VERSION)-ncopy.ys
	cp sim/pipe/pipe-full.hcl $(HANDINDIR-PARTC)/$(TEAM)-$(VERSION)-pipe-full.hcl

# test for Part A
parta:
	(cd sim; make; cd ..)
	clear && echo "" && echo "========== Part A =========="

	echo "" && echo "==== sum_list ===="
	./sim/misc/yas ./PartA/sum.ys && ./sim/misc/yis ./PartA/sum.yo
	echo "" && echo "==== rsum_list ===="
	./sim/misc/yas ./PartA/rsum.ys && ./sim/misc/yis ./PartA/rsum.yo
	echo "" && echo "==== copy_block ===="
	./sim/misc/yas ./PartA/copy.ys && ./sim/misc/yis ./PartA/copy.yo

# test for Part B
partb:
	(cd sim; make)
	(cd sim/seq; make clean; make VERSION=full)
	clear && echo "" && echo "========== Part B =========="

	echo "" && echo "==== benchmark ===="
	(cd sim/y86-code; make testssim; cd ..)
	echo "" && echo "==== regression ===="
	(cd sim/ptest; make SIM=../seq/ssim; cd ..)
	echo "" && echo "==== iaddq ===="
	(cd sim/ptest; make SIM=../seq/ssim TFLAGS=-i; cd ..)

# test for Part C
partc:
	(cd sim; make)
	(cd sim/pipe; make clean; make VERSION=full)
	clear && echo "" && echo "========== Part C =========="

	echo "" && echo "==== benchmark ===="
	(cd sim/y86-code; make testpsim; cd ..)
	echo "" && echo "==== regression ===="
	(cd sim/ptest; make SIM=../pipe/ssim; cd ..)
	echo "" && echo "==== iaddq ===="
	(cd sim/ptest; make SIM=../pipe/ssim TFLAGS=-i; cd ..)

clean:
	rm -f *~ *.o
	(cd sim; make clean; cd ..)
	clear
