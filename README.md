# Tricorn Visualizer

This tool is supposed to be used in the commandline and implements different functions to calculate the tricorn fractal.

To read further about the tricorn fractal read [here](https://en.wikipedia.org/wiki/Tricorn_(mathematics)).

You can either create .bmp-pictures of an area in the set or benchmark the different implementations against each other.

---

## Arguments of the program

After calling the program, there will be a list of which parameters were used.

`-V_<INT>`   The number defines which implementation of the tricorn function is used. The possible values are between 0 and 4 (included). By default Version 0 is used. To read more about the different versions, read below.

`-B<UNSIGNED LONG>` / `-B`   Activates the benchmarking. With a number directly after the B the function is called that many times. It is recommended to use this flag as the first one. The value of the benchmarks should be in the range of an unsigned long. The default value is 200 calls for benchmarking if benchmarking is active.

`-s_<FLOAT>,<FLOAT>`   The option defines the start-point of the calculation area. This point will be the left-lower corner of the area. The first float is the real-part of the complex number, and the second float is the imaginary-part. The default starting point is '-2.0 - 1.0i':

`d_<UNSIGNED INT>,<UNSIGNED INT>` This will set the width and height of the calculated area. The first number is the width, and the second is the height. These dimensions define the measurement points, or pixels. Therefore the actual width in the complex area is measured by width*resolution. If you start at a point and want to measure to another point , you should consider this. To calculate the width you need to put in, you can calculate the actual width of the complex area as follows:  `width(input) = width(actual)/resolution` The default value of this is 2000,2000.

`-n_<UNSIGNED LONG>` The option n sets the number of iterations for every complex point to the given number. The value is by default 100.

`-r_<FLOAT>` This value sets the resolution for the given float. You can consider the resolution to be the difference between two neighboring measurement points.By default this value will be 0.002 . You can only put in values between 6,000,000 and 1*10^-35 .

`-o_<STRING>` With this option, the name of the output file will be set to the given name, with a .bmp-ending. The value is by default 'out'. The String can be maximal 24 character long.

`-c_<INT>` Defines the coloration scheme of the output. The value can only be between 0 and 3 (included) and is 0 by default. To read more about the different colorations, read below.


## Different Implementations and Optimizations

`0` This is the default implementation of the tricorn function. It uses no particular opimization.

`1` This version is an improvement over the default version and uses a principle called area-deletion. Some complex numbers that are for sure part of the tricorn set aren't calculated. This can be useful for calls with a high number of iterations. However, for calls with more measurement points and fewer iterations, this could not be beneficial.

`2` This version is a further developed implementation of version 1. Even more complex numbers that are for sure part of the tricorn set aren't calculated. This can be useful for calls with a high number of iterations. However, for calls with more measurement points and fewer iterations, this could not be beneficial.

`3` This version is similar to the default one. However, it uses SSE intrinsics to implement a SIMD variant of the default implementation.

`4` This version is a mixture of the SIMD variant and the area-deletion version.
            

## Different Coloration Schemes

![color0](https://user-images.githubusercontent.com/83243533/236499400-ef37317c-78be-4630-933a-9f200eb5b92d.jpg)
`0` The first coloration scheme is in different blue-variations. The pixel is white if the correlating number is part of the tricorn set. The fewer iterations needed to calculate that the number is not limited, the darker the coloration.

![color1](https://user-images.githubusercontent.com/83243533/236499553-c3f0fa0e-f88f-4583-9fac-696a8e36fb2f.jpg)
`1` This scheme helps to distinguish the different areas better in comparison to the first-/blue-scheme.

![color2](https://user-images.githubusercontent.com/83243533/236499622-559cce05-25ef-423e-8e1f-5d4932cde8d9.jpg)
`2` This scheme is inverted. The points that are part of the tricorn set are colored black. The fewer iterations needed to escape, the lighter the color is.

![color3](https://user-images.githubusercontent.com/83243533/236499678-a553a1c2-fd2d-4b30-9ddc-0e7d3d2ea6e1.jpg)
`3` This scheme is a two-colored variant. It helps to distinguish which point is part  of the tricorn set and which is not. Points that are part of it are colored white, and ones that are not are colored black.
            
