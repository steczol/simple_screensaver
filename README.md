# Screensaver

Repository to test the GStreamer pipeline by a screensaver-like input.
The image processing and the interface to GStreamer is provided by OpenCV library.

![example_gif](output.gif)



## Build

```bash
mkdir build; cd build; cmake ..; make
```

## Usage

This repository consists of two programs: a **```sender```** and a **```receiver```**.

### **Sender**

The program should have at least one output.
The desired output can be specified by appending the arguments ```--output-display``` or/and ```--output-gstreamer``` to the call.

The GStreamer pipeline can be defined by the argument ```--gst-pipeline```. By default the sender pipeline is defined as:

```bash
"appsrc ! videoconvert ! video/x-raw,format=YUY2,width=640,height=480,framerate=30/1 ! jpegenc ! rtpjpegpay ! udpsink host=127.0.0.1 port=5000"
```

The path to image used as logo can be defined by the argument ```--logo```.

### **Receiver**

The GStreamer pipeline definition can be specified by setting the argument ```--gst-pipeline```.
By default the receiver pipeline is defined as: 

```bash
"udpsrc port=5000 ! application/ x-rtp,media=video,payload=26,clock-rate=90000,encoding-name=JPEG, framerate=30/1 ! rtpjpegdepay ! jpegdec ! videoconvert ! appsink"
```

## Simplest working example:

**Receiver:**
```bash
./gst_receiver
```

**Sender:**
```bash
./gst_sender --output-gstreamer
```

## Most complicated working example:

**Receiver:**
```bash
./gst_receiver --gst-pipeline "udpsrc port=5000 ! application/x-rtp,media=video,payload=26,clock-rate=90000,encoding-name=JPEG,framerate=30/1 ! rtpjpegdepay ! jpegdec ! videoconvert ! appsink"
```

The command can be executed by the GStreamer itself:
```bash
gst-launch-1.0 udpsrc port=5000 ! application/x-rtp,media=video,payload=26,clock-rate=90000,encoding-name=JPEG,framerate=30/1 ! rtpjpegdepay ! jpegdec ! videoconvert ! ximagesink
```

**Sender:**
```bash
./gst_sender --logo ../dummylogo.png --scene-width 1280 --scene-height 960 --gst-pipeline "appsrc ! videoconvert ! video/x-raw,format=YUY2,width=640,height=480,framerate=30/1 ! jpegenc ! rtpjpegpay ! udpsink host=127.0.0.1 port=5000" --gst-fourcc 0 --gst-fps 30 --gst-width 640 --gst-height 480 --output-gstreamer --output-display
```

## Dummy Logo

![dummylogo](dummylogo.png "Dummy Logo")

[created here: https://www.freelogodesign.org]