# Tensorflow Autoencoder
A simple type of neural network with only three layers, an input layer, a hidden layer, and an output layer.
What makes an autoencoder interesting is that the ouput neurons are directly connected to the input neurons.  The goal is to get the output values to match the input values.
The input is an n-dimensional input, and the hidden layer reduces this to an m-dimensional output via dimensionality reduction, then the output layer tries to reconstruct the original input based on this compressed data.  Here, the input layer is called the encoder, and the output layer is called the decoder.  The data is reconstructed via hidden weights in the hidden layer.

### This Simple Autoencoder
Can be used to generate hand written digits 

### Use Cases
	- Data Compression
	- Image Search (Fun fact, this is *probably* how Google implements image search.  Its web crawlers compress each image and store them in an easily searchable array for similarity points in the compression and then ranks results by percent similarity to the search image)
	- One-Class Classification where we have a single target class and we want to learn to detect objects of that class, and tries to detect anomalies that don't fit that target class

### Stacked Autoencoders
An extension of an autoencoder where successive autoencoders are wired together by connecting the output of one autoencoder to the input of the next.  This is useful for creating inital weights for deep nets that work much better than using initial random weights.

### Variational Autoencoders
Another subtype of autoencoders that are useful for outputting similar yet slightly different versions of the input data.  Such as slightly different face photos from a given seed original face photo.  This learns a distribution around data that it uses to output slightly different output.

### Other Notes
There are lots of techniques to prevent autoencoders from recontructing the original image, such as denoising, where the input is partially corrupted on purpose.  The idea being that if it can recontruct the image despite being corrupted, then it is a more robust decoder. 

### Credit
[this youtube video](https://www.youtube.com/watch?v=GWn7vD2Ud3M)
