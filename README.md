# Drachma
a.k.a. pr-cache-sim

## Limitations
- The simulator can only run module-bound (like CPU-bound) tasks.
  There current is no means to simulate IO-bound tasks.
- Storing computation results and forwarding to intended destinations is assumed to be ideal in that there is no cost associated and will not be a cause of blocking a dependent module's execution.

## Scheduling

### Current




### Desired
There needs to be a way to mix and match task, RR, and bitstream scheduling.
Some can literally be pipelined together, but some things will required unified portions (e.g. unified RR and bitstream scheduling, completely unified scheduler)
- See TODO (2)

*** Will support now

Task
  - FCFS - First Come, First Serve ***
  - SJN - Shortest Job Next
  - Priority
RR
  - LFB - Least Flexible, Bitstream (supports the least bitstreams)
  - LFT - Least Flexible, Task (supports the least task types)
Bitstream
  - FE - Fastest Execution (bitstream completes the task fastest)
  - SS - Smallest Size (faster transfer of the bitstream)
RR + Bitsteam
  - LFB+FE
  - LFT+FE ***
  - LFB+SS
  - LFT+SS

## TODO
1. If I'm going to support multiple tasks per bitstream, then I need to support latency for each task in the bitstream.
  This means taking speed away from the modules and assigning them to the tasks of the module.
  Consider working on (4) first before attempting to do this.
2. Scheduling
  1. Consider swapping RR scheduler with bitstream scheduler.
    Choosing an RR doesn't mean you've chosen a bitstream; there could be multiple bitstreams that support the same task in the same RR.
    The difference between these bitstreams can be the speed by which they 
  2. Support a combined task-RR scheduler.
    Among other things, this will make it possible for the task scheduler to prioritize new tasks that already have their corresponding bitstream present in an RR.
  3. ...
3. Add validation into fileHandler to ensure graph nodes have the "type" metadata.
4. Replace current config files format with YAML (need comments).
5. Support IO-bound tasks by supporting ranged execution cycles
need to add to graph node metadata (in tgffToJson) for static/ranged execution times (this stuff should be in lepton or drachma)
