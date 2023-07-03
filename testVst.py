import dawdreamer as daw
from scipy.io import wavfile
import glob
import os

files = glob.glob("./testResults/*")
for f in files:
  os.remove(f)

for sampleRate in [8000, 32000, 44100, 48000, 64000]:
  engine = daw.RenderEngine(sampleRate, 512)
  engine.set_bpm(120.)
  synth = engine.make_plugin_processor("synth", "./Releases/VST3/PianoForte.vst3")
  
  # (MIDI note, velocity, start sec, duration sec)
  for pianoNote in range(1, 88 + 1):
    synth.add_midi_note(pianoNote + 20, 127, 0, 5)
    engine.load_graph([
      (synth, []),
      # (effect, [synth.get_name()])  # effect needs 2 channels, and "synth" provides those 2.
      ])

    engine.render(10)  # render x seconds.
    audio = engine.get_audio()
    name = f'./testResults/{pianoNote:02d}@{sampleRate}.wav'
    wavfile.write(name, sampleRate, audio.transpose())
    print(name)

