Python 3.12.2 (tags/v3.12.2:6abddd9, Feb  6 2024, 21:26:36) [MSC v.1937 64 bit (AMD64)] on win32
Type "help", "copyright", "credits" or "license()" for more information.
>>> import cv2
... import urllib.request
... import numpy as np
... 
... # Replace the URL with the IP camera's stream URL
... url = 'http://192.168.101.10/cam-hi.jpg'
... cv2.namedWindow("live Cam Testing", cv2.WINDOW_AUTOSIZE)
... 
... 
... # Create a VideoCapture object
... cap = cv2.VideoCapture(url)
... 
... # Check if the IP camera stream is opened successfully
... if not cap.isOpened():
...     print("Failed to open the IP camera stream")
...     exit()
... 
... # Read and display video frames
... while True:
...     # Read a frame from the video stream
...     img_resp=urllib.request.urlopen(url)
...     imgnp=np.array(bytearray(img_resp.read()),dtype=np.uint8)
...     #ret, frame = cap.read()
...     im = cv2.imdecode(imgnp,-1)
... 
...     cv2.imshow('live Cam Testing',im)
...     key=cv2.waitKey(5)
...     if key==ord('q'):
...         break
...     
... 
... cap.release()
