# Env:
Provide a Sauvola solution in C++ and based on Opencv3.1
# Usage:

for real time program:

```
Mat Sauvola_coarse(Mat inpImg, int window_half_length, float k, float super_std)
```

`inpImg` is the grayscale image, `window_half_length` is half of the window size, which is `r/2` in the following text, `k` is the same as the `k` in the threshold calculation below. The output is the binary image after Sauvola thresholding. `super_std` is for avergae std, so we can accelerate this program.

if your senario is fixed, I recommend for first run fine-grained and then calculate the avergae std, use it as `super_std`.

for fine-grained operation:
```
Mat Sauvola_fine(Mat inpImg, int window_half_length, float k)
```



Example:

```
Mat imgSauvola = Sauvola(imgGray, 5, 0.1);

```

# Introduction to Sauvola:

Sauvola is particularly advantageous in dealing with binarization problems under uneven illumination conditions, and due to its variance, it is effective in processing patterns with gradient features.

Its performance is better than OTSU and Niblack, the latter of which can produce false noise.

# Sauvola Calculation Process:

## Mean

First, calculate the mean within the given neighborhood, which can be processed using the `fastMean` function in the code.

Note that I do not recommend using `fastMean` for programs with low resolution images (less than 340*180) and no real-time requirements, otherwise there will be a phenomenon of holes inside closed patterns.

$$
m(x,y)=\frac{1}{r^{2}}\sum_{i=x-\frac{r}{2}}^{x+\frac{r}{2}}\sum_{j=y-\frac{r}{2}}^{y+\frac{r}{2}}g(i,j)
$$

## Variance

$$
s(x,y)=\sqrt{\frac{1}{r^2}\sum_{i=x-\frac{r}{2}}^{x+\frac{r}{2}}\sum_{j=y-\frac{r}{2}}^{y+\frac{r}{2}}\left(g(i,j)-m(x,y)\right)^2}
$$

## Threshold

For grayscale images, `R` is set to 128, and `k`, which is tested by myself, only needs to be less than 0.5 to exclude uneven illumination.

$$
T(x,y)=m(x,y)\cdot\left[1+k\cdot\left(\frac{s(x,y)}{R}-1\right)\right]
$$
