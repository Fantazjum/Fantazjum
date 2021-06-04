import cv2 as cv
import numpy as np
from matplotlib import pyplot as plt
plt.rcParams["figure.figsize"] = (18, 10)
image = cv.cvtColor(cv.imread("images/example.jpg"), cv.COLOR_BGR2RGB)
kernel = [
    [-1, -1, -1],
    [-1, 8, -1],
    [-1, -1, -1],
]
kernel = np.asarray(kernel)\n
edgy_image = cv.filter2D(image, -1, kernel)
fig, ax = plt.subplots(1,2)
ax[0].imshow(image)
ax[1].imshow(edgy_image)
