
#	Example Application File for Drachma

name: Random Application (a made-up application)

/#
	this is where you would put defining parameters
	to describe traits of an application's resource
	partitioning.

	all paramters are of the following format:
		<param name>: <param value>

	the following are acceptable definitions.
	descriptions with a double astrisk** are always required.
	descriptions with an asterisk* are required under certain circumstance
		
		name: "name of the application"**

		icap speed: "the speed in MHz of the ICAP"**
		icap width: "the number of bits the ICAP can
			transfer per clock cycle (at ICAP speed)"**

		prc speed: "the speed in MHz of the Partial
			Reconfiguration Controller"**

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

static region speed: 100.00
sr0 module count: 2
sr1 module count: 1

# rr0 bitstream size: 1024
rr0 bitstream size: 64
rr0 module 0 speed: 100.00
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

