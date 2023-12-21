# Bass Synth Arch.


Let `x` be the fast-time state, then the oscillator is given by 
```
dx(t) = f(x, z; t)
 y(t) = g(x, z, t)
```
we generally roll this out over a buffer.

let `z` be the control state
```
dz_d(t) = h(z_d,z,u; t)
      z = j(z_d, u; t) 
```

### Fast Systems: 

#### Oscillators:
state + config + buffers[voice] -> buffers[voices] + state

Per Patch Config:
- active voices
- pitch
- tune
- osc type


Per Oscillator Config (derived from patch + note):
- phase increment
- osc params (attenuation, m, band limit factor)
- pulse width


Per oscillator State
- phase
- accumulator

#### Filter: 
buffers[voices] + config + state -> buffers[voices] + state

#### Mix: 
buffers[voices] + config +state -> buffers[stereo] + state 

#### FX: 
buffers[stereo] + config + state -> buffers[stereo] + state

### Slow Systems:
#### LFOs
#### Rate Limited param changes

### Patches
- New note-on generates activates a voice from a patch.
- Midi CC changes patch
- Patch changes generate voice updates

Patch is on non-rt, voice is on rt thread.


#### Patch Updates
- Patch values *not* changed via midi; i.e. `v_actual = v + f(midi_cc)`
- When a midi cc is changed, 
  - we work out the new desired state from patch + cc
  - then send the new desired state update to the rt thread
- When a patch is


## TODO
1. [DONE] Create parameter objects that have a to and from JSON method, and parameter 
   validation. 
2. [DONE] Define patch object with parameters for all the changeable quantities with
   a to and from json.
3. Make it so that when a patch object has things change, it sends a bunch of
   actions to the rt thread to execute.
4. Based on the messages defined by the patch object, make a distribution
   method to update oscillator, filter and envelope config
5. Test the whole synth.
6. Add some effects (chorus, phase, distortion)
7. Allow oversampling.
8. 