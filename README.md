
 ![CoreImage](http://i.imgur.com/Qh67mKG.jpg)

###Notice: Major refactoring in process

> Imagine being able to work in an environment where your conversations arent monitored continuously.
  Take that idea and apply it to your everyday workplace where having an outspoken opinion remains locked away due to fear of leaving a digital footprint.
  Whisper aims to help provide a platform for inter-domain level chat.
  Using dynamically generated dual encrypted one time sessions; we're giving you the tools to speak your mind without   worrying about who is listening in.

---
##For Users

You will probably never use this library directly.
Instead you might use the whisper-gui or whisper-cli verions; or potentially a GUI binding for another language.
Ultimately this repo is the core of the application and is meant for developers to hook into.
For an example of a user level implementation see: https://github.com/AlexsJones/whisper-cli


---
##For Developers

[![Build Status](https://travis-ci.org/AlexsJones/whisper-core.svg?branch=master)](https://travis-ci.org/AlexsJones/whisper-core)

Status: In active development, not ready for general consumption.

[Please see API documentation](https://github.com/AlexsJones/whisper-core/wiki/Core-API-use)

Whisper uses a combination of asymmetric 2048bit key RSA with a DES cipher for establishing sessions.
It has multi-protocol finding ability which includes multicasting upto top tier router domain and drop n' replace cyptography solutions.

The aim is to finish the kernel in C and bring it to other platforms with a variety of GUI and language hooks.

The core acts as the framework for higher level programs to hook into.

####Cryptography
![Crypto](http://i.imgur.com/Q7Wrb4L.png)

####Cryptography (Multisession)
![CryptoMulti](http://i.imgur.com/428nUmL.png)

### Table of Contents
  * [Installation](#installation)
  * [Additional Deps](#Deps)
  * [Protobuf](#protobuf)
  * [Components](#components)



### Requirements

```
cmake
build-essential
protobuf-c-compiler
lib-ssl
```

#### Installation <a id="installation"></a>


```
cmake . -DRUN_TESTS=ON #Or ignore the flag for no tests
make
sudo make install 
```

