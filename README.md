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

## Design Choices
- All task types within a module's design are capable of performing useful work at the same time.
  - While not a formal Drachma artifact, the individual implementations within a module of a single task can be thought of as a 'component' of the overrall module design.
- The speed of how quickly a task can be done by an RR is dictated by the module design. While all module designs for an RR must be the same size they can be different in their design, implementing as many different tasks as their theoretical bounds will allow. Consequently, each task within a bitstream's design will have its own speed.
- The memory hierarchy prescribed by the memory configuration file is presumed to be implemented directly as a hardware feature of the theoretical device, as opposed to being an emulated feature (e.g. using LUTs for bitstream caching).
<!--- The PRC (, ICAP, or both) has encyclopedic knowledge of where bitstreams are stored, even outside of the configuration library.-->
- The ICAP writes directly to the available cache levels. All cache levels are presumed to have a write clock that follows the speed of the ICAP. No buffering occurs and any data being shifted out of the ICAP becomes immediately available upon complete configuration of an RR with a bitstream.
- The theoretical device has special hardware connections allowing the PRC to instantly enqueue jobs directly into any RR controller's task queue.
- The RR controller is part of a component that "wraps" its respective RR, delivering inputs as jobs are popped from the task queue and saving outputs until they are delivered to the intended RR.
- The RR controller flushes its task queue when it accepts a new ICAP request.

## TODO
1. Write Tests - I greatly regret not doing this early on. In order of priority:
  1. Test each signal for correct propogation.
  1. Task scheduling algs.
  1. Cache replacement algs.
  1. Bitstream selection algs.
2. If I'm going to support multiple tasks per bitstream, then I need to support latency for each task in the bitstream.
  This means taking speed away from the modules and assigning them to the tasks of the module.
  Consider working on (4) first before attempting to do this.
3. Scheduling
  1. Consider swapping RR scheduler with bitstream scheduler.
    Choosing an RR doesn't mean you've chosen a bitstream; there could be multiple bitstreams that support the same task in the same RR.
    The difference between these bitstreams can be the speed by which they 
  2. Support a combined task-RR scheduler.
    Among other things, this will make it possible for the task scheduler to prioritize new tasks that already have their corresponding bitstream present in an RR.
  3. ...
4. Add validation into fileHandler to ensure graph nodes have the "type" metadata.
5. Replace current config files format with YAML (need comments).
6. Emulate IO-bound tasks by supporting ranged execution cycles
  - Need to add to graph node metadata (in tgffToJson) for static/ranged execution times (something like this should be in Lepton or older Drachma).
7. Use validation before attempting to deserialize YAML.
8. Need to move the concept of speed from the module level down to the task level. Installed modules should be able to perform all tasks in its design at the same time, not just the principle task the module was chosen for. Therefore it makes no sense to have it at the module level.
9. Rename/cleanup tasks
  - All 'PRR controller' type references to 'RR controller' (RRC).
  - Standardize either "T* name" or "T *name". Same for referenced.
  - All #define identifiers.
  - Screen and sentence-case all comments.
  - Remove unused variables.
  - Rename improper use of word "latency" in variable names.
  - Remove old, commented-out code.
  - Reorganize IN/OUT/INOUT/Internal signals in header files. Are there any true IN/OUT signals?
  - Remove unecessary includes.
