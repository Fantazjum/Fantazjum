import cv2 as cv
import numpy as np
from matplotlib import pyplot as plt
image = cv.cvtColor(cv.imread("images/example.jpg"), cv.COLOR_BGR2RGB)
RGB = np.float32(image.reshape(-1,3))
RGB /= 255.
transform = [
    [0.393, 0.769, 0.189],
    [0.349, 0.689, 0.164],
    [0.272, 0.534, 0.131],
]
transform = np.asarray(transform)
newRGB = []
for row in RGB:
    col = np.asarray(row.reshape(3,1))
    sums = np.matmul(transform,col)
    sums = sums.reshape(1,3)
    newRGB.append(sums)
newRGB = np.clip(newRGB, 0.0, 1.0)
newRGB = np.asarray(newRGB).reshape(image.shape)
plt.imshow(newRGB)
