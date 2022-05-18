# v4lcapture-yuv2ppm

This is meant to capture output from a USB camera and output RGB24 (RGB888) output suitable for a dump framebuffer that uses this format, like Framebuffer::GFX

Contains
- v4lcapture_long.c is original from https://www.kernel.org/doc/html/v4.11/media/uapi/v4l/capture.c.html . It outputs mjpeg by default.
- v4lcapture.c is the short version that outputs YUYV' (YUYV 4:2:2) from a USB camera to stdout (run with -c 1 to ouput a single frame that you can feed to yuv2ppm)
- v4lcapture_single.c is further simplified and just outputs a single frame on stdout
- yuv2ppm supports both YUV422 (per the original code I found on archive.org) and 'YUYV' (YUYV 4:2:2) which is needed to process the USB camera output. Outputs a PPM in binary or ASCII form)

Code is hardcoded for 320x240 which is the smallest size allowed by my USB camera

Example usage:
```
./v4lcapture_single -d /dev/video4 > out.yuv
./yuv2ppm out.yuv
gthumb out.yuv.ppm

or
./v4lcapture -c 100 > out.yuv
mplayer -demuxer rawvideo -rawvideo w=320:h=240:format=yuy2 out.yuv
```


```0
sauron:~$ v4l2-ctl -d /dev/video4 --list-formats-ext
(...)
	[1]: 'YUYV' (YUYV 4:2:2)
		Size: Discrete 640x480
			Interval: Discrete 0.033s (30.000 fps)
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 320x240
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 640x480
			Interval: Discrete 0.033s (30.000 fps)
			Interval: Discrete 0.033s (30.000 fps)
```

Suggested reading/resources:
- https://forums.raspberrypi.com/viewtopic.php?t=89495
- https://www.fourcc.org/yuv-to-rgb-conversion/
- https://www.flir.com/support-center/iis/machine-vision/knowledge-base/understanding-yuv-data-formats/#:~:text=The%20YUV422%20data%20format%20shares,V2%20Y3%20U4%20Y4%20V4%E2%80%A6
- https://stackoverflow.com/questions/36228232/yuy2-vs-yuv-422
- https://stackoverflow.com/questions/53277170/how-to-view-raw-yuv-files-with-mplayer
