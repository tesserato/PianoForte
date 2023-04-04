# Piano Forte Audio Plugin

### Author: [Carlos Tarjano](https://carlos-tarjano.web.app/)

Piano synthesiser based on a lightweight (~600KB, ~150k parameters) neural network and a novel representation for Quasi-Periodic signals.

Visit [OmnesSonos.web.app](https://omnessonos.web.app/)
for further info about the algorithm and applications to voice and other instruments.

# Download

## [VST3](https://github.com/tesserato/PianoForte/tree/main/Releases/VST3)

## [Standalone (Windows x64)](https://github.com/tesserato/PianoForte/tree/main/Releases/Standalone%20Plugin)

# Dependencies (Only if you intend to build from source)

[ONNX Runtime releases](https://github.com/microsoft/onnxruntime/releases)

From rep root:
` .\build.bat --enable_msvc_static_runtime --build_shared_lib --build --config Release`

Then
`.\build\Windows\Release> cmake.exe --build .`

And ad path in projucer

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


# Contribute

## Donations


### [Buy me a coffee](https://www.buymeacoffee.com/tesserato)

<img src="./images/bmc_qr.png" alt="Buy me a coffee" width="200"/>

### [Patreon.com/omnessonos](https://www.patreon.com/omnessonos)