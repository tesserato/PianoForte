# Piano Forte Audio Plugin

### Author: [Carlos Tarjano](https://carlos-tarjano.web.app/)

Piano synthesizer based on micro (~8KB, ~1500 parameters) neural networks and a novel representation for Quasi-Periodic signals.

Visit [OmnesSonos.web.app](https://omnessonos.web.app/)
for further info about the algorithm and intended applications to voice and other instruments.

# Download

## [VST3](https://github.com/tesserato/PianoForte/tree/main/Releases/VST3)

## [Standalone (Windows x64)](https://github.com/tesserato/PianoForte/tree/main/Releases/Standalone%20Plugin)

# Dependencies (Only if you intend to build from source)

[ONNX Runtime releases](https://github.com/microsoft/onnxruntime/releases)

`git clone --depth 1 --recursive https://github.com/Microsoft/onnxruntime`

`cd onnxruntime`

`.\build.bat --config Release --enable_msvc_static_runtime --disable_memleak_checker --skip_tests --build_shared_lib` 


And add path in Projucer as in the file "PianoForte.jucer"

More details [here](https://github.com/microsoft/onnxruntime/issues/8555)



# Raw samples used for training:

#### [University of Iowa Electronic Music Studios](https://theremin.music.uiowa.edu/MISpiano.html)
  Freely available

#### [The bitKlavier Grand – bitKlavier](https://bitklavier.com/the-bitklavier-grand/)
  [Creative Commons — Attribution 4.0 International — CC BY 4.0](https://creativecommons.org/licenses/by/4.0/)
  
  - **Recorded by:** Andrés Villalta
  - **Piano Played by:** Matt Wang
  - **Edited and Denoised by:** Jeff Gordon, Katie Chou, Christien Ayers, Matt Wang, Dan Trueman
  - **Producer:** Dan Trueman
  - With support from Princeton University’s Department of Music and Council for the Humanities

#### [Salamander Piano - SFZ Instruments](https://sfzinstruments.github.io/pianos/salamander)
  [Creative Commons — Attribution 3.0 Unported — CC BY 3.0](https://creativecommons.org/licenses/by/3.0/)
  
  - **Author:** Alexander Holm

# Other tools used:
- [JUCE](https://juce.com/)
- [Blender](https://www.blender.org/) and [Inkscape](https://inkscape.org/) for GUI elements
- [Python](https://www.python.org/), with [Pytorch](https://pytorch.org/) and [ONNX Runtime](https://onnxruntime.ai/) for NN design, training and implementation.
- [Audacity](https://www.audacityteam.org/) for audio pre processing