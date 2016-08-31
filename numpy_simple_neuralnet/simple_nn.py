import numpy as np

#	sigmoid function to run on every input to our neural net
def nonlin(x, deriv=False):
	if deriv == True:
		return x * (1 - x)

	return 1 / (1 + np.exp(-x))

#	input data
X = np.array([[0,0,1],
			  [0,1,1],
			  [1,0,1],
			  [1,1,1]])

# 	output data
Y = np.array([[0],
			  [1],
			  [1],
			  [1]])

#	get the same sequence of generated numbers for testing purposes
np.random.seed(1)

#	synapses
#	since we want three layers of neurons in our network, we need two
#	each synapse is a matrix with a random weight assigned to it
syn0 = 2 * np.random.random((3,4)) - 1
syn1 = 2 * np.random.random((4,1)) - 1

# 	training
for i in xrange(60000):
	# 	prediction step, perform matrix multiplication between each layer
	#	and its synapse.  If it reaches a threshold from our sigmoid func
	#	then it gets passed to the next layer
	l0 = X
	l1 = nonlin(np.dot(l0, syn0))

	#	use output of that layer on our next layer to get a more fine prediction
	l2 = nonlin(np.dot(l1, syn1))

	# compare to the expected output data to get the error rate
	l2_error = Y - l2

	#	print out the error rate at some interval to make sure 
	#	it decreases each time
	if (i % 10000) == 0:
		print 'Error' + str(np.mean(np.abs(l2_error)))

	#	multiply error rate by the result of our sigmoid function
	#	used to get the derivative of our output prediction from layer 2
	#	gives us a delta that we can use to reduce the error rate of our 
	#	predictions when we update our synapses every iteration
	l2_delta = l2_error * nonlin(l2, deriv=True)

	# 	now we want to see how much l1 contributed to the error rate 
	#	in layer two aka back propagation, obtained by multiplying layer 2's
	#	delta by synapse 1's transpose
	l1_error = l2_delta.dot(syn1.T)

	# 	then we'll get layer 1's delta by multiplying its error by the result
	#	of our sigmoid function
	# 	used to get the derivative of layer one
	l1_delta = l1_error * nonlin(l1, deriv=True)

	#	update synapse weights to reduce the error rate of each iteration
	#	this is gradient descent, just multiply each layer by a delta
	syn1 += l1.T.dot(l2_delta)
	syn0 += l0.T.dot(l1_delta)

# 	print predicted output
print 'Output After Training'
print l2
print 'Actual Output'
print Y



