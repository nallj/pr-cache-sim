
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

     - rr (round-robin) - doesn't make sense since this is for scheduling tasks on a single core; it is not trivial to 
     - "Weighted shortest job first" (WSJF) - too complicated of a scheduling algorithm to implement; jobs get weighted with the cost of delay so that the highest valued jobs get done sooner.
     - shortest remaining time first (SRTF) - cant be done because it requires preemption

    [TODO] prr selection policy:
      - lowest flexibility (LF): choose the PRR that can support the least bitstreams leaving the more
        flexible PRR free
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
prr selection policy: lf

static region speed: 100.00
sr0 module count: 2
sr1 module count: 1

# rr0 bitstream size: 1024
rr0 bitstream size: 64
rr0 module 0 speed: 100.00

#rr0 module 0 tasks: task_0

rr0 module 1 speed: 100.00
rr0 module 2 speed: 100.00
rr0 module 3 speed: 100.00
rr0 module 4 speed: 100.00

# rr1 bitstream size: 768
rr1 bitstream size: 32
rr1 module 0 speed: 100.00
rr1 module 1 speed: 100.00
rr1 module 2 speed: 100.00
rr1 module 3 speed: 100.00

# rr2 bitstream size: 512
rr2 bitstream size: 16
rr2 module 0 speed: 100.00
rr2 module 1 speed: 100.00
rr2 module 2 speed: 100.00

# rr3 bitstream size: 256
rr3 bitstream size: 8
rr3 module 0 speed: 100.00
rr3 module 1 speed: 100.00

# rr4 bitstream size: 256
rr4 bitstream size: 8
rr4 module 0 speed: 100.00
rr4 module 1 speed: 100.00





###### TODO SECTION ######



########## DO NOT USE ###################
############### NEW SECTION

# Task Declarations (what type of IP are used in this application?)

# T1: Blah 1
# ...
# T9: Blah 9

# Which RRs have bitstreams making IP available to the RR

# rr0 t1 t3 t7 t9
# ...
# rr4 t1 t2 t6   


