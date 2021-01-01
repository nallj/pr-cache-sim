
# Example Application File for Drachma

name: Random Application (a made-up application)

/#
  This is where you would put defining parameters to describe the resource partitioning of an
  application.

  All paramters are of the following format:
    <param name>: <param value>

  The following are acceptable definitions. Descriptions with a double astrisk are always required.
  Descriptions with a single asterisk are required under certain circumstance; they are followed
  by the condition under which it is required.
    
    name: "name of the application"**

    icap speed: "the speed in MHz of the ICAP"**
    icap width: "the number of bits the ICAP can
      transfer per clock cycle (at ICAP speed)"**

    prc speed: "the speed in MHz of the Partial
      Reconfiguration Controller"**
    
    [TODO] task scheduling:
     - fcfs (first come first serve)
      * no jobs: schedule first
      * jobs going 
     - sjn (shortest job next): naive
      * no jobs: schedule first
      * 4 jobs, 2 in contention
     - sjn (shortest job next): schedule contentious first
      * no jobs: schedule first
      * 4 jobs, 2 in contention
     - priority - requires that metadata has priortiy within them.

     - rr (round-robin) - doesn't make sense since this is for scheduling tasks on a single core; without preemption this wont work.
     - "Weighted shortest job first" (WSJF) - too complicated of a scheduling algorithm to implement; jobs get weighted with the cost of delay so that the highest valued jobs get done sooner.
     - shortest remaining time first (SRTF) - cant be done because it requires preemption

    [TODO] rr selection policy:
      - lowest flexibility (LFT_FE): choose the RR that can support the least bitstreams leaving the more
        flexible RR free
      * what about selection policies that take into account cache? a lower cost than pulling fresh is pulling from cache
      * what about a task that can be done by two different implementations (2 diff bitstreams)

    // TODO: probably not worth the time to develop; yeah, don't develop this
    task scheduling equivalence bias: left/right (when tasks are otherwise completely equivalent)

    static region speed: "the speed in MHz of the static
      regions"*
      > required if declaring static regions
    srX module count: "amount of modules residing
      in the static region X (where X <= 0)"
      there can be an arbitrary amount of SRs
      static regions

    rrX bitstream size: "amount of bytes required
      from the ICAP to reconfigure the
      reconfigurable region X (where X <= 0)"
      there can be an arbitrary amount of RRs
    rrX module Y speed: "the speed (in MHz) of module
      Y from reconfigurable region X"
      there can be an arbitrary amount of RMs per RR
#/

icap speed: 100.00
icap width: 32

prc speed: 100.00

task scheduling: fcfs
rr selection policy: lf


# Static Regions

static region speed: 100.00
sr0 module count: 2
sr1 module count: 1


# RR 0

# rr0 bitstream size: 1024
rr0 bitstream size: 64
rr0 module 0 speed: 100.00
rr0 module 1 speed: 100.00
rr0 module 2 speed: 100.00
rr0 module 3 speed: 100.00
rr0 module 4 speed: 100.00

rr0 module 0 task types: 0, 1
rr0 module 1 task types: 1, 2
rr0 module 2 task types: 2, 3, 1
rr0 module 3 task types: 4, 5
rr0 module 4 task types: 1, 4, 2


# RR 1

# rr1 bitstream size: 768
rr1 bitstream size: 32
rr1 module 0 speed: 100.00
rr1 module 1 speed: 100.00
rr1 module 2 speed: 100.00
rr1 module 3 speed: 100.00

rr1 module 0 task types: 6, 7
rr1 module 1 task types: 8, 9
rr1 module 2 task types: 6, 7, 8
rr1 module 3 task types: 7, 8


# RR 2

# rr2 bitstream size: 512
rr2 bitstream size: 16
rr2 module 0 speed: 100.00
rr2 module 1 speed: 100.00
rr2 module 2 speed: 100.00

rr2 module 0 task types: 9, 10
rr2 module 1 task types: 10, 11
rr2 module 2 task types: 9, 11


# RR 3

# rr3 bitstream size: 256
rr3 bitstream size: 8
rr3 module 0 speed: 100.00
rr3 module 1 speed: 100.00

rr3 module 0 task types: 11, 12
rr3 module 1 task types: 11, 13


# RR 4

# rr4 bitstream size: 256
rr4 bitstream size: 8
rr4 module 0 speed: 100.00
rr4 module 1 speed: 100.00

rr4 module 0 task types: 13, 14
rr4 module 1 task types: 11, 14
